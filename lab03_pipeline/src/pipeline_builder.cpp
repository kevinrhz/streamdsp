/**
 * @file pipeline_builder.cpp
 * @brief Fluent builder for composing processing pipelines.
 *
 * YOUR TASK: Implement the Pimpl-based builder.
 * - with_config(): store config
 * - add_window/fft/magnitude(): create stage via factory, push to internal vector
 * - validate(): check non-empty, nfft is power of 2, channels > 0, overlap in [0,1)
 * - build(): validate, move stages out, clear internal state
 *
 * Mirrors: cpp/src/core/pipeline_builder.cpp
 */

#include "pipeline_builder.hpp"
#include "stage_factory.hpp"
#include <stdexcept>

namespace stx {

class PipelineBuilder::Impl {
public:
    // TODO: Implement all methods
    StageConfig config_;
    std::vector<std::unique_ptr<ProcessingStage>> stages_;
};

PipelineBuilder::PipelineBuilder() : pImpl(std::make_unique<Impl>()) {}
PipelineBuilder::~PipelineBuilder() = default;

PipelineBuilder& PipelineBuilder::with_config(const StageConfig& config) {
    pImpl->config_ = config;
    return *this;
}

PipelineBuilder& PipelineBuilder::add_window(StageConfig::WindowType type) {
    // TODO
    return *this;
}

PipelineBuilder& PipelineBuilder::add_fft() {
    // TODO
    return *this;
}

PipelineBuilder& PipelineBuilder::add_magnitude() {
    // TODO
    return *this;
}

bool PipelineBuilder::validate(std::string& error_msg) const {
    // TODO: check empty pipeline, nfft power of 2, channels > 0, overlap range
    error_msg = "validate not implemented";
    return false;
}

std::vector<std::unique_ptr<ProcessingStage>> PipelineBuilder::build() {
    std::string error_msg;
    if (!validate(error_msg)) {
        throw std::runtime_error("Pipeline validation failed: " + error_msg);
    }
    auto result = std::move(pImpl->stages_);
    pImpl->stages_.clear();
    return result;
}

size_t PipelineBuilder::num_stages() const {
    return pImpl->stages_.size();
}

void PipelineBuilder::clear() {
    pImpl->stages_.clear();
}

}  // namespace stx
