#pragma once

#include <chrono>
#include <cstddef>

namespace stx {

/**
 * @class ChronoTimer
 * @brief RAII timer that records elapsed time on destruction.
 *
 * Usage:
 *   float elapsed_us;
 *   {
 *       ChronoTimer t(elapsed_us);
 *       // ... work ...
 *   }
 *   // elapsed_us now holds wall-clock time in microseconds
 */
class ChronoTimer {
public:
    explicit ChronoTimer(float& out_us)
        : out_(out_us), start_(std::chrono::steady_clock::now()) {} // Important: initialize out_(out_us), not assign out_ = out_ms. Do same with start_ just to be consistent, non breaking.

    ~ChronoTimer() {
        auto end = std::chrono::steady_clock::now();
        out_ = std::chrono::duration<float, std::micro>(end - start_).count();
        // Equivalent...
        // auto raw = end - start_;                                         1. native duration
        // auto in_us = std::chrono::duration<float, std::micro>(raw);      2. convert to us
        // float value = in_us.count();                                     3. unwrap
        // out_ = value;                                                    4. store

    }

    ChronoTimer(const ChronoTimer&) = delete;
    ChronoTimer& operator=(const ChronoTimer&) = delete;

private:
    // Order matters: Members are initialized in the order they're declared in the class, not the order they appear in the initializer list.
    // If start_ was declared before out_ in the class, it would be initialized first — even though out_(out_us) appears first in the initializer list.
    float& out_;
    std::chrono::steady_clock::time_point start_;
};

/**
 * @brief Compute throughput in GiB/s given bytes transferred and time in us.
 */
inline float throughput_gibps(size_t bytes, float latency_us) {
    if (latency_us < 1e-9f) return 0.0f;
    float secs = latency_us * 1e-6f;
    return (static_cast<float>(bytes) / (1024.0f * 1024.0f * 1024.0f)) / secs;
}

}  // namespace stx
