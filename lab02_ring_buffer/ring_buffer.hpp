/**
 * @file ring_buffer.hpp
 * @brief Lock-free SPSC ring buffer with atomic memory ordering.
 *
 * Single-producer/single-consumer circular buffer for streaming data — the
 * backbone of feeding continuous samples into a real-time processing pipeline.
 */

#pragma once

#include <atomic>
#include <cstddef>
#include <cassert>
#include <cstring>
#include <stdexcept>
#include <vector>

#include "types.hpp"  // For ReadSpan, FrameView

namespace stx {

template <typename T>
class RingBuffer {
public:
    explicit RingBuffer(size_t capacity)
        : capacity_(capacity), buffer_(capacity),
          write_pos_{0}, read_pos_{0}, readable_{0} {
        if (capacity == 0) {
            throw std::invalid_argument("Ring buffer capacity must be > 0");
        }
    }

    // TODO: Implement extract_batch(), peek_frame(), peek_frame_at_offset(), reset()
    //
    // See the study guide and lab syllabus for exact memory ordering
    // requirements on each atomic operation.

    void push(const T* data, size_t count) {
        // Aquire to see consumers latest advance
        size_t readable = readable_.load(std::memory_order_acquire);
        if (count > (capacity_ - readable))
            throw std::overflow_error("Ring buffer overflow");

        // Snapshot of own write position (relaxed - sole writer)
        size_t pos = write_pos_.load(std::memory_order_relaxed);

        // Copy payload into the ring
        std::memcpy(&buffer_[pos], data, count * sizeof(T));

        // Publish - release so consumer see the memcpy BEFORE the counters
        write_pos_.store(pos + count, std::memory_order_release);
        readable_.fetch_add(count, std::memory_order_release);
    }

    // Peek-copies frame_size elements — does NOT advance read_pos_/readable_.
    // Caller must: check can_extract_frame(frame_size), call extract_frame(),
    // then call advance(frame_size) to commit the read. Unchecked here by design.
    void extract_frame(T* output, size_t frame_size) const {
        size_t pos = read_pos_.load(std::memory_order_relaxed);
        std::memcpy(output, &buffer_[pos], frame_size * sizeof(T));
    }
    bool can_extract_frame(const size_t frame_size) const { return frame_size <= readable(); }

    // Commits a prior extract_frame() by advancing read_pos_ and shrinking readable_
    // Assumes the same precondition extract_frame() already trusted.
    void advance(size_t samples) {
        size_t readable = readable_.load(std::memory_order_acquire);
        assert(samples <= readable && "advance() called with samples exceeding readable elements");

        size_t pos = read_pos_.load(std::memory_order_relaxed);
        read_pos_.store(pos + samples, std::memory_order_release);
        readable_.fetch_sub(samples, std::memory_order_release);
    }

    size_t capacity() const { return capacity_; }
    size_t readable() const { return readable_.load(std::memory_order_acquire); }
    bool empty() const { return readable() == 0; }
    bool full() const { return readable() == capacity_; }

private:
    size_t              capacity_;
    std::vector<T>      buffer_;
    std::atomic<size_t> write_pos_;
    std::atomic<size_t> read_pos_;
    std::atomic<size_t> readable_;
};

}  // namespace stx
