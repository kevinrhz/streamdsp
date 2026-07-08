/**
 * @file batch_executor.hpp
 * @brief Batch executor with round-robin buffers and simulated 3-thread pipeline.
 *
 * Implemented in src/batch_executor.cpp.
 */
#pragma once

#include <memory>
#include <vector>

#include "stage_config.hpp"
#include "processing_stage.hpp"
#include "types.hpp"

namespace stx {

class BatchExecutor {
public:
    BatchExecutor();
    ~BatchExecutor();

    // Disable copy, enable move
    BatchExecutor(const BatchExecutor&) = delete;
    BatchExecutor& operator=(const BatchExecutor&) = delete;
    BatchExecutor(BatchExecutor&&) noexcept;
    BatchExecutor& operator=(BatchExecutor&&) noexcept;

    void initialize(const StageConfig& config);
    void submit(const float* input, float* output, size_t num_samples);
    void reset();

    ProcessingStats get_stats() const;
    bool is_initialized() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

}  // namespace stx
