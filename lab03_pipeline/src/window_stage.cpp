/**
 * @file window_stage.cpp
 * @brief Pimpl implementation of WindowStage.
 *
 * YOUR TASK:
 * 1. Define WindowStage::Impl with a std::vector<float> for window coefficients.
 * 2. initialize(): compute Hann coefficients: 0.5 * (1 - cos(2*PI*i/N))
 * 3. process(): element-wise multiply output[i] = input[i] * window[i % nfft]
 *
 * Mirrors: cpp/src/core/processing_stage.cpp WindowStage::Impl
 */

#include "processing_stage.hpp"

#include <cmath>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace stx {

class WindowStage::Impl {
public:
    // TODO: Implement initialize() and process()
};

WindowStage::WindowStage() : pImpl(std::make_unique<Impl>()) {}
WindowStage::~WindowStage() = default;

void WindowStage::initialize(const StageConfig& config) {
    pImpl->initialize(config);  // TODO: won't compile until Impl has this method
}

void WindowStage::process(const float* input, float* output, size_t num_elements) {
    pImpl->process(input, output, num_elements);  // TODO: same
}

}  // namespace stx
