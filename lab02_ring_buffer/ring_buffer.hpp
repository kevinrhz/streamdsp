/**
 * @file ring_buffer.hpp
 * @brief Lock-free SPSC ring buffer with atomic memory ordering.
 *
 * Mirrors sigtekx::RingBuffer<T> from cpp/include/sigtekx/core/ring_buffer.hpp.
 *
 * YOUR TASK: Implement this class from scratch.
 * See SIGTEKX_LAB_SYLLABUS.md Lab 2 for full requirements.
 */

#pragma once

#include <atomic>
#include <cstddef>
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
          write_pos_{0}, read_pos_{0}, available_{0} {
        if (capacity == 0) {
            throw std::invalid_argument("Ring buffer capacity must be > 0");
        }
    }

    // TODO: Implement push(), extract_frame(), extract_batch(),
    //       peek_frame(), peek_frame_at_offset(), advance(),
    //       available(), capacity(), empty(), full(), reset(),
    //       can_extract_frame()
    //
    // See the study guide and lab syllabus for exact memory ordering
    // requirements on each atomic operation.

    size_t capacity() const { return capacity_; }
    size_t available() const { return available_.load(std::memory_order_acquire); }
    bool empty() const { return available() == 0; }
    bool full() const { return available() == capacity_; }

private:
    size_t capacity_;
    std::vector<T> buffer_;
    std::atomic<size_t> write_pos_;
    std::atomic<size_t> read_pos_;
    std::atomic<size_t> available_;
};

}  // namespace stx
