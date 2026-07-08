/**
 * @file test_stages.cpp
 * @brief Individual stage tests + factory tests.
 */

#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>

#include "processing_stage.hpp"
#include "stage_config.hpp"
#include "stage_factory.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace stx;

void test_window_stage_edges_near_zero() {
    // Hann window: edges should be near zero when applied to all-ones input
    StageConfig config;
    config.nfft = 8;
    config.channels = 1;

    WindowStage stage;
    stage.initialize(config);

    std::vector<float> input(8, 1.0f);
    std::vector<float> output(8, -1.0f);

    stage.process(input.data(), output.data(), 8);

    // Hann window at index 0 should be ~0
    assert(std::abs(output[0]) < 0.01f);
    std::printf("  window_stage_edges: PASSED\n");
}

void test_fft_stage_dc_signal() {
    // All-ones input -> DC bin should dominate
    StageConfig config;
    config.nfft = 8;
    config.channels = 1;

    FFTStage stage;
    stage.initialize(config);

    std::vector<float> input(8, 1.0f);
    int num_bins = config.num_output_bins();  // 5
    std::vector<float> output(num_bins * 2, 0.0f);  // complex pairs

    stage.process(input.data(), output.data(), 8);

    // DC bin (index 0): re should be ~N (8.0), im should be ~0
    float dc_re = output[0];
    float dc_im = output[1];
    float dc_mag = std::sqrt(dc_re * dc_re + dc_im * dc_im);
    assert(dc_mag > 7.0f);  // Should be close to 8.0
    std::printf("  fft_stage_dc: PASSED (DC magnitude=%.2f)\n", dc_mag);
}

void test_magnitude_stage_known_values() {
    // Input complex (3,4) -> magnitude should be 5.0
    StageConfig config;
    config.nfft = 2;  // Minimal
    config.channels = 1;
    config.scale_policy = StageConfig::ScalePolicy::NONE;

    MagnitudeStage stage;
    stage.initialize(config);

    // 2 complex pairs = 4 floats input
    float input[] = {3.0f, 4.0f, 0.0f, 1.0f};
    float output[2] = {-1.0f, -1.0f};

    stage.process(input, output, 2);  // 2 complex pairs

    assert(std::abs(output[0] - 5.0f) < 1e-5f);
    assert(std::abs(output[1] - 1.0f) < 1e-5f);
    std::printf("  magnitude_stage_known: PASSED\n");
}

void test_factory_create() {
    auto w = StageFactory::create(StageFactory::StageType::WINDOW);
    assert(w != nullptr);
    assert(w->name() == "WindowStage");

    auto f = StageFactory::create(StageFactory::StageType::FFT);
    assert(f != nullptr);
    assert(f->name() == "FFTStage");

    auto m = StageFactory::create(StageFactory::StageType::MAGNITUDE);
    assert(m != nullptr);
    assert(m->name() == "MagnitudeStage");

    std::printf("  factory_create: PASSED\n");
}

void test_factory_default_pipeline() {
    auto stages = StageFactory::create_default_pipeline();
    assert(stages.size() == 3);
    assert(stages[0]->name() == "WindowStage");
    assert(stages[1]->name() == "FFTStage");
    assert(stages[2]->name() == "MagnitudeStage");
    std::printf("  factory_default_pipeline: PASSED\n");
}

int main() {
    std::printf("Lab 03 - Stage Tests:\n");
    test_window_stage_edges_near_zero();
    test_fft_stage_dc_signal();
    test_magnitude_stage_known_values();
    test_factory_create();
    test_factory_default_pipeline();
    std::printf("Lab 03 - Stages: ALL TESTS PASSED\n");
    return 0;
}
