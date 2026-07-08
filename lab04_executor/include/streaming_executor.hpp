/**
 * @file streaming_executor.hpp
 * @brief Streaming executor with ring buffers and overlap-aware frame extraction.
 *
 * Implemented in src/streaming_executor.cpp.
 */
#pragma once

#include <atomic>
#include <memory>
#include <vector>

#include "stage_config.hpp"
#include "processing_stage.hpp"
#include "types.hpp"

namespace stx {

class StreamingExecutor {
public:
    StreamingExecutor();
    ~StreamingExecutor();

    StreamingExecutor(const StreamingExecutor&) = delete;
    StreamingExecutor& operator=(const StreamingExecutor&) = delete;
    StreamingExecutor(StreamingExecutor&&) noexcept;
    StreamingExecutor& operator=(StreamingExecutor&&) noexcept;

    void initialize(const StageConfig& config);
    void submit(const float* input, float* output, size_t num_samples);
    void shutdown();
    void reset();

    ProcessingStats get_stats() const;
    bool is_initialized() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

}  // namespace stx
