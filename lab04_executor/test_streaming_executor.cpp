/**
 * @file test_streaming_executor.cpp
 * @brief Tests for StreamingExecutor.
 */

#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>

#include "streaming_executor.hpp"
#include "stage_config.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace stx;

std::vector<float> make_cosine_stream(int num_samples, int channels, int freq_bin, int nfft) {
    std::vector<float> signal(num_samples * channels);
    for (int ch = 0; ch < channels; ++ch) {
        for (int i = 0; i < num_samples; ++i) {
            signal[ch * num_samples + i] = std::cos(2.0 * M_PI * freq_bin * i / nfft);
        }
    }
    return signal;
}

void test_streaming_single_frame() {
    // Push exactly nfft samples -> should produce 1 frame
    StageConfig config;
    config.nfft = 256;
    config.channels = 1;
    config.overlap = 0.0f;  // No overlap, hop = nfft

    StreamingExecutor exec;
    exec.initialize(config);
    assert(exec.is_initialized());

    auto input = make_cosine_stream(256, 1, 10, 256);
    std::vector<float> output(config.num_output_bins());

    exec.submit(input.data(), output.data(), input.size());

    auto stats = exec.get_stats();
    assert(stats.frames_processed >= 1);

    // Verify output has non-zero values
    bool has_nonzero = false;
    for (float v : output) {
        if (v > 1e-6f) { has_nonzero = true; break; }
    }
    assert(has_nonzero);

    std::printf("  streaming_single_frame: PASSED\n");
}

void test_streaming_overlap() {
    // With 50% overlap, second submit of nfft samples should produce 2 frames
    StageConfig config;
    config.nfft = 256;
    config.channels = 1;
    config.overlap = 0.5f;  // hop = 128

    StreamingExecutor exec;
    exec.initialize(config);

    auto input = make_cosine_stream(256, 1, 10, 256);
    std::vector<float> output(config.num_output_bins());

    // First submit: fills ring buffer, produces 1 frame
    exec.submit(input.data(), output.data(), input.size());

    // Second submit: 256 new samples + 128 residual -> enough for 2 more frames
    exec.submit(input.data(), output.data(), input.size());

    auto stats = exec.get_stats();
    assert(stats.frames_processed >= 2);

    std::printf("  streaming_overlap: PASSED (frames=%zu)\n", stats.frames_processed);
}

void test_streaming_arbitrary_chunks() {
    // Push small chunks (100 samples) until a frame can be extracted
    StageConfig config;
    config.nfft = 256;
    config.channels = 1;
    config.overlap = 0.0f;

    StreamingExecutor exec;
    exec.initialize(config);

    std::vector<float> output(config.num_output_bins());

    // Push 100 at a time (need 256 for first frame)
    for (int i = 0; i < 3; ++i) {
        auto chunk = make_cosine_stream(100, 1, 10, 256);
        exec.submit(chunk.data(), output.data(), chunk.size());
    }

    // After 300 samples, should have processed at least 1 frame (nfft=256)
    auto stats = exec.get_stats();
    assert(stats.frames_processed >= 1);

    std::printf("  streaming_arbitrary_chunks: PASSED (frames=%zu after 300 samples)\n",
                stats.frames_processed);
}

void test_streaming_shutdown() {
    StageConfig config;
    config.nfft = 128;
    config.channels = 1;

    StreamingExecutor exec;
    exec.initialize(config);

    auto input = make_cosine_stream(128, 1, 5, 128);
    std::vector<float> output(config.num_output_bins());
    exec.submit(input.data(), output.data(), input.size());

    exec.shutdown();
    // Should not deadlock or crash

    std::printf("  streaming_shutdown: PASSED\n");
}

int main() {
    std::printf("Lab 04 - StreamingExecutor Tests:\n");
    test_streaming_single_frame();
    test_streaming_overlap();
    test_streaming_arbitrary_chunks();
    test_streaming_shutdown();
    std::printf("Lab 04 - StreamingExecutor: ALL TESTS PASSED\n");
    return 0;
}
