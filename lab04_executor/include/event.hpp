/**
 * @file event.hpp
 * @brief CPU-side event for thread synchronization.
 *
 * Mimics the role of CudaEvent in the real codebase: one thread records
 * the event (signals completion), another thread waits on it.
 *
 * This is given to you — it's a utility, not the learning objective.
 */
#pragma once

#include <condition_variable>
#include <mutex>

namespace stx {

class Event {
public:
    void record() {
        std::lock_guard<std::mutex> lk(mtx_);
        ready_ = true;
        cv_.notify_all();
    }

    void wait() {
        std::unique_lock<std::mutex> lk(mtx_);
        cv_.wait(lk, [this] { return ready_; });
    }

    void reset() {
        std::lock_guard<std::mutex> lk(mtx_);
        ready_ = false;
    }

    bool query() const {
        std::lock_guard<std::mutex> lk(mtx_);
        return ready_;
    }

private:
    mutable std::mutex mtx_;
    std::condition_variable cv_;
    bool ready_ = false;
};

}  // namespace stx
