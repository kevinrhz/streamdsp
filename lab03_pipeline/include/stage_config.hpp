#pragma once
namespace stx {
struct StageConfig {
    int nfft = 1024;
    int channels = 2;
    float overlap = 0.5f;
    int sample_rate_hz = 48000;
    enum class WindowType { RECTANGULAR, HANN };
    WindowType window_type = WindowType::HANN;
    enum class ScalePolicy { NONE, ONE_OVER_N };
    ScalePolicy scale_policy = ScalePolicy::ONE_OVER_N;
    int hop_size() const { return static_cast<int>(nfft * (1.0f - overlap)); }
    int num_output_bins() const { return nfft / 2 + 1; }
};
}  // namespace stx
