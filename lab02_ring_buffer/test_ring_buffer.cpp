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

int main() {
    // Smoke test: construction
    RingBuffer<float> buf(1024);
    assert(buf.capacity() == 1024);
    assert(buf.available() == 0);
    assert(buf.empty());
    assert(!buf.full());

    std::printf("Lab 02 - RingBuffer: IMPLEMENT REMAINING TESTS\n");
    return 0;
}
