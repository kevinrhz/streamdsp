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
    assert(rb.readable() == 2);    // 4 pushed and 2 consumed so 2 left
}

void test_wraparound() {
    RingBuffer<float> rb(8);

    // Push 6, then consume 6, now read_pos_ and write_pos_ are both at 6,
    // buffer is empty but the cursors sit near the end.
    float filler[] = {0,0,0,0,0,0};
    rb.push(filler, 6);
    rb.advance(6);
    // write_pos_ = 6, readable_ = 0

    // Push 4, this WRAPS and lands at 6,7,0,1
    float data[] = {10, 20, 30, 40};
    rb.push(data, 4);
    assert(rb.readable() == 4);

    // Extract across the boundary, should reassemble 10,20,30,40 in order
    float out[4] = {0};
    rb.extract_frame(out, 4);
    assert(out[0] == 10);   // was at index 6
    assert(out[1] == 20);   // index 7
    assert(out[2] == 30);   // wrapped to index 0
    assert(out[3] == 40);   // index 1
}

void test_wraparound_repeated() {
    // Regression: advance() must wrap read_pos_ with % capacity, exactly like push().
    // Without the modulo, read_pos_ grows past capacity_ and the NEXT extract_frame
    // computes first_chunk = capacity_ - pos, which underflows size_t to a huge
    // number -> memcpy of ~18 quintillion bytes -> segfault.
    RingBuffer<float> rb(8);

    // First trip to the boundary
    float a[] = {1, 2, 3, 4, 5, 6};
    rb.push(a, 6);
    rb.advance(6);            // read_pos_ = 6

    // Wrap the write, then advance the read PAST the boundary a second time
    float b[] = {10, 20, 30, 40};
    rb.push(b, 4);           // wraps: write_pos_ = 2, data at slots 6,7,0,1
    rb.advance(4);           // read_pos_ = (6+4) % 8 = 2
    assert(rb.readable() == 0);

    // Push into the freed space and read it back. Proves read_pos_ landed at 2,
    // not 10 — extract_frame reads valid contiguous data instead of underflowing.
    float c[] = {50, 60, 70, 80};
    rb.push(c, 4);           // no wrap: slots 2,3,4,5, write_pos_ = 6
    assert(rb.readable() == 4);

    float out[4] = {0};
    rb.extract_frame(out, 4);
    assert(out[0] == 50);
    assert(out[1] == 60);
    assert(out[2] == 70);
    assert(out[3] == 80);
}

int main() {
    test_construction();
    test_push();
    test_extract();
    test_advance();
    test_wraparound();
    test_wraparound_repeated();

    std::printf("Lab 02 - RingBuffer: TESTS PASSED!\n");
    return 0;
}
