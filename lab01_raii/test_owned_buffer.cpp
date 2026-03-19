/**
 * @file test_owned_buffer.cpp
 * @brief Tests for OwnedBuffer<T> — RAII move-only heap wrapper.
 *
 * Run: ./lab01_test_owned_buffer
 * All assertions must pass with zero output (silent success).
 */

#include <cassert>
#include <cstdio>
#include <utility>
#include <vector>

#include "owned_buffer.hpp"

using stx::OwnedBuffer;

void test_construction() {
    OwnedBuffer<float> buf(256);
    assert(buf.get() != nullptr);
    assert(buf.size() == 256);
    assert(buf.bytes() == 256 * sizeof(float));
}

void test_default_construction() {
    OwnedBuffer<float> buf;
    assert(buf.get() == nullptr);
    assert(buf.size() == 0);
}

void test_move_construction() {
    OwnedBuffer<float> buf1(128);
    float* original_ptr = buf1.get();

    OwnedBuffer<float> buf2(std::move(buf1));

    assert(buf2.get() == original_ptr);
    assert(buf2.size() == 128);
    assert(buf1.get() == nullptr);  // Source is nulled
    assert(buf1.size() == 0);
}

void test_move_assignment() {
    OwnedBuffer<float> buf1(128);
    OwnedBuffer<float> buf2(64);
    float* ptr1 = buf1.get();

    buf2 = std::move(buf1);

    assert(buf2.get() == ptr1);
    assert(buf2.size() == 128);
    assert(buf1.get() == nullptr);
}

void test_copy_roundtrip() {
    OwnedBuffer<int> buf(10);
    int source[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    buf.copy_from(source, 10);

    int dest[10] = {};
    buf.copy_to(dest, 10);

    for (int i = 0; i < 10; ++i) {
        assert(dest[i] == i);
    }
}

void test_resize() {
    OwnedBuffer<float> buf(100);
    assert(buf.size() == 100);

    buf.resize(200);
    assert(buf.size() == 200);
    assert(buf.get() != nullptr);

    buf.resize(0);
    assert(buf.size() == 0);
}

void test_destructor_doesnt_crash() {
    {
        OwnedBuffer<float> buf(1024);
        // Destructor runs here — should not crash
    }
    // If we get here, destructor worked
}

int main() {
    test_construction();
    test_default_construction();
    test_move_construction();
    test_move_assignment();
    test_copy_roundtrip();
    test_resize();
    test_destructor_doesnt_crash();

    std::printf("Lab 01 - OwnedBuffer: ALL TESTS PASSED\n");
    return 0;
}
