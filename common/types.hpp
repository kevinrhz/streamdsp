#pragma once

#include <cstddef>
#include <string>

namespace stx {

/**
 * @struct ProcessingStats
 * @brief Performance metrics for a processing operation.
 * Mirrors sigtekx::ProcessingStats from the real codebase.
 */
struct ProcessingStats {
    float latency_us = 0.0f;
    float throughput_gbps = 0.0f;
    size_t frames_processed = 0;
    bool is_warmup = false;
};

/**
 * @struct ReadSpan
 * @brief A contiguous span of elements within a ring buffer.
 * Mirrors sigtekx::RingBuffer::ReadSpan.
 */
template <typename T>
struct ReadSpan {
    const T* data = nullptr;
    size_t count = 0;
};

/**
 * @struct FrameView
 * @brief Zero-copy view into a ring buffer frame (may be split at wraparound).
 * Mirrors sigtekx::RingBuffer::FrameView.
 */
template <typename T>
struct FrameView {
    ReadSpan<T> first;
    ReadSpan<T> second;  // {nullptr, 0} if contiguous
    bool is_contiguous() const noexcept { return second.data == nullptr; }
};

}  // namespace stx
