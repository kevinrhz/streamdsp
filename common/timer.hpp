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
        : out_(out_us), start_(std::chrono::steady_clock::now()) {}

    ~ChronoTimer() {
        auto end = std::chrono::steady_clock::now();
        out_ = std::chrono::duration<float, std::micro>(end - start_).count();
    }

    ChronoTimer(const ChronoTimer&) = delete;
    ChronoTimer& operator=(const ChronoTimer&) = delete;

private:
    float& out_;
    std::chrono::steady_clock::time_point start_;
};

/**
 * @brief Compute throughput in GB/s given bytes transferred and time in us.
 */
inline float throughput_gbps(size_t bytes, float latency_us) {
    if (latency_us < 1e-9f) return 0.0f;
    float secs = latency_us * 1e-6f;
    return (static_cast<float>(bytes) / (1024.0f * 1024.0f * 1024.0f)) / secs;
}

}  // namespace stx
