/**
 * @file fft_stage.cpp
 * @brief Pimpl implementation of FFTStage (naive DFT, no cuFFT).
 *
 * YOUR TASK:
 * 1. Define FFTStage::Impl with config storage.
 * 2. initialize(): store config.
 * 3. process(): for each channel, compute naive DFT (O(N^2)):
 *    For bin k in [0, nfft/2]:
 *      X[k].re = sum_n input[n] * cos(2*PI*k*n/N)
 *      X[k].im = sum_n input[n] * -sin(2*PI*k*n/N)
 *    Output layout: interleaved [re0, im0, re1, im1, ...] per channel
 *    Total output floats = num_output_bins * channels * 2
 *
 * This is intentionally slow — the point is the architecture, not the algorithm.
 * Mirrors: cpp/src/core/processing_stage.cpp FFTStage::Impl (which uses cuFFT)
 */

#include "processing_stage.hpp"

#include <cmath>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace stx {

class FFTStage::Impl {
public:
    // TODO: Implement initialize() and process()
};

FFTStage::FFTStage() : pImpl(std::make_unique<Impl>()) {}
FFTStage::~FFTStage() = default;

void FFTStage::initialize(const StageConfig& config) {
    pImpl->initialize(config);
}

void FFTStage::process(const float* input, float* output, size_t num_elements) {
    pImpl->process(input, output, num_elements);
}

}  // namespace stx
