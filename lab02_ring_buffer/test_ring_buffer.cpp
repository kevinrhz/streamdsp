/**
 * @file test_ring_buffer.cpp
 * @brief Single-threaded correctness tests for RingBuffer<T>.
 */
#include <cassert>
#include <cstdio>
#include <vector>

#include "ring_buffer.hpp"

using stx::RingBuffer;

// TODO: Implement correctness tests for RingBuffer<T>.
// Start with: construction, push+readable, push-to-full, overflow throws,
// extract_frame, advance, wraparound, extract_batch overlap, peek_frame.
void test_construction() {
    RingBuffer<float> rb(1024);
    assert(rb.capacity() == 1024);
    assert(rb.readable() == 0);
    assert(rb.empty());
    assert(!rb.full());
}

void test_push() {
    RingBuffer<float> rb(8);
    float data[] = {1.0f, 2.0f, 3.0f, 4.0f};
    rb.push(data, 4);
    assert(rb.readable() == 4);
}

void test_extract() {
    RingBuffer<float> rb(8);
    float data[] = {1.0f, 2.0f, 3.0f, 4.0f};
    float resultData[4];
    rb.push(data, 4);

    // can_extract_frame()
    assert(rb.can_extract_frame(3) == true);
    assert(rb.can_extract_frame(4) == true);
    assert(rb.can_extract_frame(8) == false);
    
    // exrtract_frame()
    rb.extract_frame(resultData, 4);
    assert(rb.readable() == 4);    // didnt modify ring buffer
    assert(resultData[0] == 1.0f);
    assert(resultData[1] == 2.0f);
    assert(resultData[2] == 3.0f);
    assert(resultData[3] == 4.0f);
}

void test_advance() {
    RingBuffer<float> rb(8);
    float data[] = {1, 2, 3, 4};
    rb.push(data, 4);
    rb.advance(2);
    assert(rb.readable() == 2);    // 4 pushed, 2 consumed → 2 left
}


int main() {
    test_construction();
    test_push();
    test_extract();
    test_advance();

    std::printf("Lab 02 - RingBuffer: TESTS PASSED!\n");
    return 0;
}
