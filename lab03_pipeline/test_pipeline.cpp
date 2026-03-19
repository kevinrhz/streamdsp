/**
 * @file test_pipeline.cpp
 * @brief Full pipeline integration test: Window -> FFT -> Magnitude.
 *
 * Generate a cosine at a known frequency, run through the full pipeline,
 * verify the magnitude peak appears in the correct bin.
 *
 * This mirrors: cpp/tests/core/test_processing_stage.cpp FullPipelineIntegration
 */

#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>

#include "pipeline_builder.hpp"
#include "stage_config.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace stx;

void test_builder_fluent_api() {
    StageConfig config;
    config.nfft = 256;
    config.channels = 1;

    PipelineBuilder builder;
    builder.with_config(config)
           .add_window(StageConfig::WindowType::HANN)
           .add_fft()
           .add_magnitude();

    assert(builder.num_stages() == 3);

    auto stages = builder.build();
    assert(stages.size() == 3);
    assert(builder.num_stages() == 0);  // Cleared after build

    std::printf("  builder_fluent_api: PASSED\n");
}

void test_builder_empty_throws() {
    PipelineBuilder builder;
    bool threw = false;
    try {
        builder.build();
    } catch (const std::runtime_error&) {
        threw = true;
    }
    assert(threw);
    std::printf("  builder_empty_throws: PASSED\n");
}

void test_full_pipeline_frequency_detection() {
    // Generate a cosine at bin 10 of a 256-point FFT
    const int nfft = 256;
    const int channels = 1;
    const int target_bin = 10;
    const int num_bins = nfft / 2 + 1;

    StageConfig config;
    config.nfft = nfft;
    config.channels = channels;
    config.scale_policy = StageConfig::ScalePolicy::ONE_OVER_N;

    // Build pipeline
    auto stages = PipelineBuilder()
        .with_config(config)
        .add_window(StageConfig::WindowType::HANN)
        .add_fft()
        .add_magnitude()
        .build();

    // Initialize all stages
    for (auto& stage : stages) {
        stage->initialize(config);
    }

    // Generate input: cosine at target_bin
    std::vector<float> input(nfft);
    for (int i = 0; i < nfft; ++i) {
        input[i] = std::cos(2.0 * M_PI * target_bin * i / nfft);
    }

    // Run Window (in-place)
    std::vector<float> windowed(nfft);
    stages[0]->process(input.data(), windowed.data(), nfft);

    // Run FFT (real -> complex)
    std::vector<float> fft_output(num_bins * 2);  // complex pairs
    stages[1]->process(windowed.data(), fft_output.data(), nfft);

    // Run Magnitude (complex -> real)
    std::vector<float> magnitude(num_bins);
    stages[2]->process(fft_output.data(), magnitude.data(), num_bins);

    // Find peak bin
    float max_mag = 0.0f;
    int peak_bin = -1;
    for (int k = 0; k < num_bins; ++k) {
        if (magnitude[k] > max_mag) {
            max_mag = magnitude[k];
            peak_bin = k;
        }
    }

    assert(peak_bin == target_bin);
    assert(max_mag > 0.01f);  // Should have a noticeable peak

    std::printf("  full_pipeline_frequency_detection: PASSED (peak at bin %d, mag=%.4f)\n",
                peak_bin, max_mag);
}

int main() {
    std::printf("Lab 03 - Pipeline Tests:\n");
    test_builder_fluent_api();
    test_builder_empty_throws();
    test_full_pipeline_frequency_detection();
    std::printf("Lab 03 - Pipeline: ALL TESTS PASSED\n");
    return 0;
}
