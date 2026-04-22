/**
 * @file test_buffer.cpp
 * @brief Tests for Buffer<T> — RAII move-only heap wrapper.
 *
 * Run: ./lab01_test_buffer
 * All assertions must pass with zero output (silent success).
 */

#include <cassert>
#include <cstdio>
#include <utility>

#include "buffer.hpp"

using stx::Buffer;

void test_construction() {
    Buffer<float> buf(256);
    assert(buf.get() != nullptr);
    assert(buf.size() == 256);
    assert(buf.bytes() == 256 * sizeof(float));
}

void test_default_construction() {
    Buffer<float> buf;
    assert(buf.get() == nullptr);
    assert(buf.size() == 0);
}

void test_move_construction() {
    Buffer<float> buf1(128);
    float* original_ptr = buf1.get();

    Buffer<float> buf2(std::move(buf1));

    assert(buf2.get() == original_ptr);
    assert(buf2.size() == 128);
    assert(buf1.get() == nullptr);  // Source is nulled
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
}

void test_copy_roundtrip() {
    // create 10 counts with data [0][1][2][3][4][5][6][7][8][9] (stack array)
    int source[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    // create 10 counts uninitialized buffer [?][?][?][?][?][?][?][?][?][?] (heap array)
    Buffer<int> buf(10);
    // copy from source data into uninitialized buffer
    buf.copy_from(source, 10);

    // create 10 counts of zero-initialized data [0][0][0][0][0][0][0][0][0][0] (stack array)
    int dest[10] = {};
    // copy to destination data with buffer data recently copied from source
    buf.copy_to(dest, 10);

    // check if the dest array has the correct values 0-9
    for (int i = 0; i < 10; ++i) {
        assert(dest[i] == i);
    }
}

void test_resize() {
    Buffer<float> buf(100);
    assert(buf.size() == 100);

    buf.resize(200);
    assert(buf.size() == 200);
    assert(buf.get() != nullptr);

    buf.resize(0);
    assert(buf.size() == 0);
}

void test_destructor_doesnt_crash() {
    {
        Buffer<float> buf(1024);
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

    std::printf("Lab 01 - Buffer: ALL TESTS PASSED\n");
    return 0;
}
