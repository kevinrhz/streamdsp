/**
 * @file magnitude_stage.cpp
 * @brief Pimpl implementation of MagnitudeStage.
 *
 * YOUR TASK:
 * 1. Define MagnitudeStage::Impl with scale factor and config.
 * 2. initialize(): compute scale based on ScalePolicy (1.0 or 1/N).
 * 3. process(): input is interleaved complex [re0,im0,re1,im1,...].
 *    For each complex pair: output[i] = sqrt(re*re + im*im) * scale.
 *    num_elements = number of complex pairs (NOT floats).
 *    Output has num_elements floats.
 *
 * Mirrors: cpp/src/core/processing_stage.cpp MagnitudeStage::Impl
 */

#include "processing_stage.hpp"

#include <cmath>

namespace stx {

class MagnitudeStage::Impl {
public:
    // TODO: Implement initialize() and process()
};

MagnitudeStage::MagnitudeStage() : pImpl(std::make_unique<Impl>()) {}
MagnitudeStage::~MagnitudeStage() = default;

void MagnitudeStage::initialize(const StageConfig& config) {
    pImpl->initialize(config);
}

void MagnitudeStage::process(const float* input, float* output, size_t num_elements) {
    pImpl->process(input, output, num_elements);
}

}  // namespace stx
