/**
 * @file test_buffer.cpp
 * @brief Tests for Buffer<T> — RAII move-only heap wrapper.
 *
 * Run: ./lab01_test_buffer
 * All assertions must pass with zero output until the final line (silent success).
 *
 * Three kinds of check in here:
 *   1. static_assert  — compile-time claims about TYPES (copy deleted, move noexcept,
 *                       const-overload resolution). These fail the BUILD, not the run.
 *   2. assert         — runtime claims about VALUES.
 *   3. Tracked        — an instrumented element type with a live-object counter, so we can
 *                       prove resources are actually FREED and not merely dropped.
 */

#include <cassert>
#include <cstdio>
#include <type_traits>
#include <utility>

#include "buffer.hpp"

using stx::Buffer;

// ─────────────────────────────────────────────────────────────────────────────
// 1. COMPILE-TIME CONTRACT
//    These are the defining properties of an RAII move-only type. If any of them
//    regress, the build breaks
// ─────────────────────────────────────────────────────────────────────────────

// Move-only: copy must be gone.
static_assert(!std::is_copy_constructible_v<Buffer<float>>, "Buffer must not be copyable");
static_assert(!std::is_copy_assignable_v<Buffer<float>>,    "Buffer must not be copy-assignable");

// Move must exist AND be noexcept. The noexcept is what gives std::vector the strong
// exception guarantee when it reallocates (it can't roll back a half-finished move).
static_assert(std::is_nothrow_move_constructible_v<Buffer<float>>, "move ctor must be noexcept");
static_assert(std::is_nothrow_move_assignable_v<Buffer<float>>,    "move assign must be noexcept");

// The const overload pair. A value assertion CANNOT test this — both overloads return the
// same address, so `assert(c.get() == p)` would pass even if overload resolution picked
// wrong. The claim is about the RETURN TYPE, so it must be checked at compile time.
static_assert(std::is_same_v<decltype(std::declval<Buffer<float>&>().get()), float*>,
              "get() on a mutable Buffer must return T*");
static_assert(std::is_same_v<decltype(std::declval<const Buffer<float>&>().get()), const float*>,
              "get() const on a const Buffer must return const T*");

// A default-constructed Buffer must be a valid empty state (this is what a moved-from
// object becomes, so the destructor must handle it).
static_assert(std::is_default_constructible_v<Buffer<float>>, "Buffer must be default-constructible");

// ─────────────────────────────────────────────────────────────────────────────
// 2. INSTRUMENTED ELEMENT TYPE
//    Buffer<Tracked> lets us count live elements, so we can prove that delete[]
//    actually ran. Without this, a leak in move-assignment is INVISIBLE to a test:
//    the pointer gets overwritten and nothing observable changes.
// ─────────────────────────────────────────────────────────────────────────────

struct Tracked {
    static int live;
    Tracked()  { ++live; }
    ~Tracked() { --live; }
    Tracked(const Tracked&)            { ++live; }
    Tracked& operator=(const Tracked&) = default;
};
int Tracked::live = 0;

// ─────────────────────────────────────────────────────────────────────────────
// 3. RUNTIME TESTS
// ─────────────────────────────────────────────────────────────────────────────

void test_default_construction() {
    Buffer<float> buf;
    assert(buf.get() == nullptr);
    assert(buf.size() == 0);
    assert(buf.bytes() == 0);
}

void test_sized_construction() {
    Buffer<float> buf(256);
    assert(buf.get() != nullptr);
    assert(buf.size() == 256);
    assert(buf.bytes() == 256 * sizeof(float));
}

void test_const_access() {
    Buffer<float> buf(4);
    buf.get()[0] = 42.0f;  // write through the mutable overload

    const Buffer<float>& cbuf = buf;   // bind a const reference — picks get() const
    assert(cbuf.get() != nullptr);
    assert(cbuf.get()[0] == 42.0f);    // read through the const overload
    assert(cbuf.size() == 4);          // size()/bytes() are const-callable
    assert(cbuf.bytes() == 4 * sizeof(float));
    // cbuf.get()[0] = 1.0f;           // <- would not compile: const T* forbids the write.
}

void test_move_construction() {
    Buffer<float> buf1(128);
    float* original_ptr = buf1.get();

    Buffer<float> buf2(std::move(buf1));

    assert(buf2.get() == original_ptr);  // the pointer was transferred, not copied
    assert(buf2.size() == 128);
    assert(buf1.get() == nullptr);       // source nulled -> its destructor is a no-op
    assert(buf1.size() == 0);
}

void test_move_assignment() {
    Buffer<float> buf1(128);
    Buffer<float> buf2(64);
    float* ptr1 = buf1.get();

    buf2 = std::move(buf1);

    assert(buf2.get() == ptr1);
    assert(buf2.size() == 128);
    assert(buf1.get() == nullptr);
    assert(buf1.size() == 0);
}

// THE test for the #1 move-assignment bug. The move-assign operator must delete[] the
// resource it currently owns BEFORE stealing. Skip that step and you leak silently —
// no crash, no wrong value, nothing a normal test would ever notice.
void test_move_assignment_frees_existing_resource() {
    assert(Tracked::live == 0);
    {
        Buffer<Tracked> a(10);         // live == 10
        assert(Tracked::live == 10);

        Buffer<Tracked> b(3);          // live == 13
        assert(Tracked::live == 13);

        a = std::move(b);              // a's 10 elements MUST be destroyed here
        assert(Tracked::live == 3);    // ← fails (== 13) if move-assign forgets to delete[]

        assert(a.size() == 3);
        assert(b.get() == nullptr);
    }
    assert(Tracked::live == 0);        // and everything is cleaned up at scope exit
}

// The `if (this == &other) return *this;` guard. Without it, move-assign would delete[]
// its own pointer and then "steal" from the dangling pointer it just freed → use-after-free.
// The pointer laundering below is deliberate: written as `b = std::move(b)` the compiler
// would reject it with -Wself-move before it ever ran.
void test_self_move_assignment() {
    Buffer<float> b(32);
    float* original = b.get();
    b.get()[0] = 7.0f;

    Buffer<float>* alias = &b;
    b = std::move(*alias);             // self-move — must be a safe no-op

    assert(b.get() == original);       // resource NOT freed
    assert(b.size() == 32);            // size intact
    assert(b.get()[0] == 7.0f);        // contents intact (i.e. no use-after-free)
}

// A moved-from object is "valid but unspecified": it must be safe to destroy AND safe to
// assign a new value into. That's the whole invariant that makes moves safe.
void test_moved_from_is_reusable() {
    Buffer<float> a(16);
    Buffer<float> b(std::move(a));
    assert(a.get() == nullptr);

    a = Buffer<float>(8);              // assign into the moved-from husk
    assert(a.get() != nullptr);
    assert(a.size() == 8);
}

// Chained moves: each source must be left null, so no resource is destroyed twice.
void test_double_move() {
    Buffer<float> a(64);
    float* original = a.get();

    Buffer<float> b(std::move(a));
    Buffer<float> c(std::move(b));

    assert(c.get() == original);
    assert(a.get() == nullptr);
    assert(b.get() == nullptr);
    assert(a.size() == 0);
    assert(b.size() == 0);
}

void test_copy_roundtrip() {
    int source[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    Buffer<int> buf(10);
    buf.copy_from(source, 10);   // copy FROM src INTO me

    int dest[10] = {};
    buf.copy_to(dest, 10);       // copy FROM me OUT to dst

    for (int i = 0; i < 10; ++i) {
        assert(dest[i] == i);
    }
}

// resize() frees and reallocates — contents are NOT preserved, by design. It avoids a
// copy that the caller usually doesn't want. Tracked proves the old elements really die.
void test_resize_frees_old_allocation() {
    assert(Tracked::live == 0);
    {
        Buffer<Tracked> buf(100);
        assert(Tracked::live == 100);

        buf.resize(200);
        assert(Tracked::live == 200);   // <- 300 would mean the old 100 leaked
        assert(buf.size() == 200);

        buf.resize(0);
        assert(Tracked::live == 0);
        assert(buf.size() == 0);
    }
    assert(Tracked::live == 0);
}

// Destructor runs at the closing brace on every path — that is the one language guarantee
// RAII is built on. Tracked makes it observable rather than a matter of faith.
void test_destructor_releases() {
    assert(Tracked::live == 0);
    {
        Buffer<Tracked> buf(1024);
        assert(Tracked::live == 1024);
    }  // <- destructor here
    assert(Tracked::live == 0);
}

// Destroying a moved-from (null) Buffer must be safe. delete[] nullptr is well-defined
// and a no-op — which is exactly why nullptr is the right sentinel for this resource.
// (Contrast File, where close(-1) is an ERROR and the destructor must guard.)
void test_destroy_moved_from_is_safe() {
    Buffer<float> a(16);
    Buffer<float> b(std::move(a));
    (void)b;
    // a destructs here with ptr_ == nullptr -> delete[] nullptr -> no-op, no crash.
}

int main() {
    test_default_construction();
    test_sized_construction();
    test_const_access();
    test_move_construction();
    test_move_assignment();
    test_move_assignment_frees_existing_resource();
    test_self_move_assignment();
    test_moved_from_is_reusable();
    test_double_move();
    test_copy_roundtrip();
    test_resize_frees_old_allocation();
    test_destructor_releases();
    test_destroy_moved_from_is_safe();

    assert(Tracked::live == 0);  // nothing leaked across the whole suite

    std::printf("Lab 01 - Buffer: ALL TESTS PASSED\n");
    return 0;
}
