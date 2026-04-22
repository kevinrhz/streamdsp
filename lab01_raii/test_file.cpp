/**
 * @file test_file.cpp
 * @brief Tests for File and ResourceException.
 *
 * Run: ./lab01_test_file
 */

#include <cassert>
#include <cstdio>
#include <cstring>
#include "file.hpp"
// TODO: #include "resource_exception.hpp"

using stx::File;

void test_construction() {
    File f;
    assert(f.is_valid() == false);
    assert(f.get() == -1);
}

void test_default_construction() {
    File f("/tmp/stx_test.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    assert(f.is_valid() == true);
    assert(f.get() != -1);
}

void test_move_construction() {
    File f1("/tmp/stx_test.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    int original_fd = f1.get();

    File f2(std::move(f1));

    assert(f2.get() == original_fd);
    assert(f1.get() == -1);
}

void test_move_assignment() {
    File f1("/tmp/stx_test.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    File f2("/tmp/stx_test.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    int fd1 = f1.get();

    f2 = std::move(f1);

    assert(f2.get() == fd1);
    assert(f1.get() == -1);
}

void test_write_read_roundtrip() {
    // write phase
    {
        File f("/tmp/stx_test.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        f.write("hello", sizeof("hello"));
    }
    // read phase
    {
        File f("/tmp/stx_test.bin", O_RDONLY, 0);
        char buf[6] = {}; // size 6 to include 5 letters + null terminator \0
        f.read(buf, sizeof(buf));
        assert(std::memcmp(buf, "hello", sizeof("hello")) == 0);
    }
}

int main() {

    test_construction();
    test_default_construction();
    test_move_construction();
    test_move_assignment();
    test_write_read_roundtrip();

    std::printf("Lab 01 - File: TESTS PASSED!\n");
    return 0;
}
