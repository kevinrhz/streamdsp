/**
 * @file test_batch_executor.cpp
 * @brief Tests for BatchExecutor.
 * See SIGTEKX_LAB_SYLLABUS.md Lab 4 Part A for requirements.
 */

#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>

#include "batch_executor.hpp"
#include "stage_config.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace stx;

std::vector<float> make_cosine(int nfft, int channels, int freq_bin) {
    std::vector<float> signal(nfft * channels);
    for (int ch = 0; ch < channels; ++ch) {
        for (int i = 0; i < nfft; ++i) {
            signal[ch * nfft + i] = std::cos(2.0 * M_PI * freq_bin * i / nfft);
        }
    }
    return signal;
}

void test_initialize_and_submit() {
    StageConfig config;
    config.nfft = 256;
    config.channels = 1;

    BatchExecutor exec;
    exec.initialize(config);
    assert(exec.is_initialized());

    auto input = make_cosine(256, 1, 10);
    std::vector<float> output(config.num_output_bins());

    exec.submit(input.data(), output.data(), input.size());

    auto stats = exec.get_stats();
    assert(stats.frames_processed >= 1);
    assert(stats.latency_us > 0.0f);

    std::printf("  initialize_and_submit: PASSED (latency=%.1f us)\n", stats.latency_us);
}

void test_multiple_frames() {
    StageConfig config;
    config.nfft = 256;
    config.channels = 2;

    BatchExecutor exec;
    exec.initialize(config);

    for (int i = 0; i < 10; ++i) {
        auto input = make_cosine(256, 2, 10 + i);
        std::vector<float> output(config.num_output_bins() * config.channels);
        exec.submit(input.data(), output.data(), input.size());
    }

    auto stats = exec.get_stats();
    assert(stats.frames_processed == 10);
    std::printf("  multiple_frames: PASSED (frames=%zu)\n", stats.frames_processed);
}

void test_reset() {
    StageConfig config;
    config.nfft = 128;
    config.channels = 1;

    BatchExecutor exec;
    exec.initialize(config);
    assert(exec.is_initialized());

    exec.reset();
    assert(!exec.is_initialized());

    std::printf("  reset: PASSED\n");
}

int main() {
    std::printf("Lab 04 - BatchExecutor Tests:\n");
    test_initialize_and_submit();
    test_multiple_frames();
    test_reset();
    std::printf("Lab 04 - BatchExecutor: ALL TESTS PASSED\n");
    return 0;
}
