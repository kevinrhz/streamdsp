/**
 * @file test_ring_buffer.cpp
 * @brief Single-threaded correctness tests for RingBuffer<T>.
 */
#include <cassert>
#include <cstdio>
#include <vector>

#include "ring_buffer.hpp"

using stx::RingBuffer;

// TODO: Implement all tests from SIGTEKX_LAB_SYLLABUS.md Lab 2.
// Start with: construction, push+available, push-to-full, overflow throws,
// extract_frame, advance, wraparound, extract_batch overlap, peek_frame.
void test_construction() {
    RingBuffer<float> rbuf(1024);
    assert(rbuf.capacity() == 1024);
    assert(rbuf.available() == 0);
    assert(rbuf.empty());
    assert(!rbuf.full());
}

void test_push() {
    RingBuffer<float> rbuf(8);
    float data[] = {1.0f, 2.0f, 3.0f, 4.0f};
    rbuf.push(data, 4);
    assert(rbuf.available() == 4);
}

int main() {
    test_construction();
    test_push();

    std::printf("Lab 02 - RingBuffer: TESTS PASSED!\n");
    return 0;
}
