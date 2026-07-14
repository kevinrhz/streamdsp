/**
 * @file test_file.cpp
 * @brief Tests for File (RAII fd wrapper) and ResourceException + CHECK_POSIX.
 *
 * Run: ./lab01_test_file
 *
 * File is the SAME RAII pattern as Buffer<T>, over a different resource:
 *   Buffer: heap pointer, sentinel nullptr, delete[] nullptr is a safe no-op
 *   File:   kernel fd,    sentinel -1,      close(-1) is an ERROR -> destructor must guard
 *
 * Because the resource is a kernel handle rather than memory, we can actually ASK the
 * kernel whether an fd is still open (fcntl F_GETFD). That lets us test things that are
 * invisible in Buffer: that move-assignment really closes the old fd, and that a
 * moved-from File does NOT double-close.
 */

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <type_traits>
#include <utility>

#include "file.hpp"
#include "resource_exception.hpp"

using stx::File;

static const char* kPath = "/tmp/stx_test.bin";

// Ask the kernel: is this fd still open in our process's fd table?
// F_GETFD is the cheapest possible query — it just reads the close-on-exec flag.
// Returns false (and sets errno == EBADF) if the fd has been closed.
static bool fd_is_open(int fd) {
    return ::fcntl(fd, F_GETFD) != -1;
}

// ─────────────────────────────────────────────────────────────────────────────
// COMPILE-TIME CONTRACT
// ─────────────────────────────────────────────────────────────────────────────

static_assert(!std::is_copy_constructible_v<File>, "File must not be copyable");
static_assert(!std::is_copy_assignable_v<File>,    "File must not be copy-assignable");
static_assert(std::is_nothrow_move_constructible_v<File>, "move ctor must be noexcept");
static_assert(std::is_nothrow_move_assignable_v<File>,    "move assign must be noexcept");
static_assert(std::is_default_constructible_v<File>, "File must have a valid empty state");

// ─────────────────────────────────────────────────────────────────────────────
// CONSTRUCTION
// ─────────────────────────────────────────────────────────────────────────────

void test_default_construction() {
    File f;
    assert(f.is_valid() == false);
    assert(f.get() == -1);          // the sentinel, not 0 — fd 0 is stdin and is VALID
}

void test_path_construction() {
    File f(kPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    assert(f.is_valid() == true);
    assert(f.get() != -1);
    assert(f.get() >= 3);           // 0/1/2 are stdin/stdout/stderr
}

void test_failed_open_is_invalid_not_throwing() {
    // The constructor calls open() directly and does NOT use CHECK_POSIX, so a bad path
    // yields an invalid File rather than an exception. Callers must check is_valid().
    // (Documenting the actual contract — see RESULTS.md for whether it SHOULD throw.)
    File f("/nonexistent/bad/path", O_RDONLY, 0);
    assert(f.is_valid() == false);
    assert(f.get() == -1);
}

// ─────────────────────────────────────────────────────────────────────────────
// MOVE SEMANTICS — the parts Buffer cannot test
// ─────────────────────────────────────────────────────────────────────────────

void test_move_construction() {
    File f1(kPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    int original_fd = f1.get();

    File f2(std::move(f1));

    assert(f2.get() == original_fd);   // handle transferred
    assert(f1.get() == -1);            // source reset to the sentinel
    assert(f1.is_valid() == false);
    assert(fd_is_open(original_fd));   // and the fd is still open — nothing was closed
}

// If the move constructor forgot to reset the source to -1, BOTH objects would close the
// same fd on destruction. The second close() would fail with EBADF — and worse, in a real
// program that fd number could have been recycled by another open() in between, so you'd
// be closing SOMEONE ELSE'S FILE. This is the fd equivalent of a double-free.
void test_moved_from_does_not_double_close() {
    File f2;
    int fd;
    {
        File f1(kPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        fd = f1.get();
        f2 = std::move(f1);
    }  // <- f1 destructs HERE. It must NOT close fd, because f2 now owns it.

    assert(fd_is_open(fd));      // fails if f1's destructor closed the fd it gave away
    assert(f2.get() == fd);
}

// THE test for the #1 move-assignment bug, in its fd form. Move-assign must release the
// resource it currently owns BEFORE stealing. In Buffer this leaks memory invisibly; here
// it leaks an fd, and we can prove it by asking the kernel.
void test_move_assignment_closes_existing_fd() {
    File f1(kPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    File f2(kPath, O_RDONLY, 0);

    int fd1 = f1.get();
    int fd2 = f2.get();
    assert(fd1 != fd2);          // two separate opens -> two distinct fds
    assert(fd_is_open(fd2));

    f2 = std::move(f1);          // f2's OWN fd (fd2) must be closed before taking fd1

    assert(f2.get() == fd1);
    assert(f1.get() == -1);
    assert(fd_is_open(fd1));     // the stolen fd is alive
    assert(!fd_is_open(fd2));    // <- fails if move-assign leaked f2's original fd
}

void test_self_move_assignment() {
    File f(kPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    int fd = f.get();

    File* alias = &f;
    f = std::move(*alias);       // must be a safe no-op, not close-then-steal-from-closed

    assert(f.get() == fd);
    assert(f.is_valid());
    assert(fd_is_open(fd));      // fails if the self-check is missing -> fd closed, then kept
}

void test_destroy_moved_from_is_safe() {
    File f1(kPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    File f2(std::move(f1));
    // f1 destructs at scope exit with fd_ == -1. The destructor's `if (fd_ != -1)` guard is
    // what makes that safe — close(-1) would fail with EBADF. This is the asymmetry with
    // Buffer, where delete[] nullptr needs no guard.
    assert(f1.get() == -1);
    (void)f2;
}

void test_moved_from_is_reusable() {
    File a(kPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    File b(std::move(a));
    assert(!a.is_valid());

    a = File(kPath, O_RDONLY, 0);   // assign into the moved-from husk
    assert(a.is_valid());
    (void)b;
}

// ─────────────────────────────────────────────────────────────────────────────
// I/O
// ─────────────────────────────────────────────────────────────────────────────

void test_write_read_roundtrip() {
    const char msg[] = "hello";   // sizeof == 6, includes the trailing '\0'

    {   // write phase — File closes the fd at the closing brace, flushing to the kernel
        File f(kPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        assert(f.is_valid());
        ssize_t n = f.write(msg, sizeof(msg));
        assert(n == static_cast<ssize_t>(sizeof(msg)));   // check the return value!
    }

    {   // read phase
        File f(kPath, O_RDONLY, 0);
        assert(f.is_valid());
        char buf[sizeof(msg)] = {};
        ssize_t n = f.read(buf, sizeof(buf));
        assert(n == static_cast<ssize_t>(sizeof(msg)));
        assert(std::memcmp(buf, msg, sizeof(msg)) == 0);
    }
}

void test_read_at_eof_returns_zero() {
    {
        File f(kPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);   // truncate to empty
        assert(f.is_valid());
    }
    File f(kPath, O_RDONLY, 0);
    char buf[8] = {};
    assert(f.read(buf, sizeof(buf)) == 0);   // 0 == EOF, and it is NOT an error
}

// ─────────────────────────────────────────────────────────────────────────────
// CHECK_POSIX / ResourceException
// ─────────────────────────────────────────────────────────────────────────────

void test_check_posix_throws_on_bad_path() {
    bool threw = false;
    try {
        CHECK_POSIX(open("/nonexistent/bad/path", O_RDONLY, 0));
    } catch (const stx::ResourceException& e) {
        threw = true;
        assert(e.error_code() == ENOENT);            // errno captured, and it's the right one

        // #call stringification: the message must quote the actual failing source expression.
        assert(std::strstr(e.what(), "open(") != nullptr);
        // __FILE__ / __LINE__ expand at the CALL SITE — that's why this must be a macro.
        assert(std::strstr(e.what(), "test_file.cpp") != nullptr);
        assert(std::strstr(e.what(), std::strerror(ENOENT)) != nullptr);
        assert(e.line() > 0);
    }
    assert(threw);
}

void test_check_posix_succeeds_on_good_call() {
    // Wrap the fd so it is actually closed. CHECK_POSIX returns nothing and swallows the
    // fd, so a bare `CHECK_POSIX(open(...))` LEAKS the descriptor — an easy trap, and a
    // slightly embarrassing one in a lab about resource ownership.
    int fd = open(kPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    CHECK_POSIX(fd);          // must not throw
    File owner;
    owner = File(kPath, O_RDONLY, 0);
    assert(owner.is_valid());
    close(fd);
}

void test_resource_exception_is_a_runtime_error() {
    // It must be catchable as std::runtime_error / std::exception, so generic handlers work.
    bool caught_as_base = false;
    try {
        CHECK_POSIX(open("/nonexistent/bad/path", O_RDONLY, 0));
    } catch (const std::runtime_error& e) {
        caught_as_base = true;
        assert(std::strlen(e.what()) > 0);
    }
    assert(caught_as_base);
}

int main() {
    test_default_construction();
    test_path_construction();
    test_failed_open_is_invalid_not_throwing();

    test_move_construction();
    test_moved_from_does_not_double_close();
    test_move_assignment_closes_existing_fd();
    test_self_move_assignment();
    test_destroy_moved_from_is_safe();
    test_moved_from_is_reusable();

    test_write_read_roundtrip();
    test_read_at_eof_returns_zero();

    test_check_posix_throws_on_bad_path();
    test_check_posix_succeeds_on_good_call();
    test_resource_exception_is_a_runtime_error();

    std::remove(kPath);

    std::printf("Lab 01 - File: ALL TESTS PASSED\n");
    return 0;
}
