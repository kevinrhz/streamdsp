/**
 * @file owned_buffer.hpp
 * @brief RAII move-only wrapper for heap-allocated memory.
 *
 * Mirrors the pattern from sigtekx::DeviceBuffer<T> and sigtekx::PinnedHostBuffer<T>
 * in cpp/include/sigtekx/core/cuda_wrappers.hpp.
 *
 * YOUR TASK: Implement this class from scratch.
 * - Constructor: allocate with new T[count]
 * - Destructor: deallocate with delete[]
 * - Delete copy constructor and copy assignment
 * - Implement move constructor and move assignment using std::exchange
 * - Methods: get(), size(), bytes(), resize(), copy_from(), copy_to()
 */

#pragma once

#include <cstddef>
#include <cstring>
#include <utility>

namespace stx {

template <typename T>
class OwnedBuffer {
public:
    // TODO: Implement constructor, destructor, move semantics, methods

    OwnedBuffer() : ptr_(nullptr), size_(0) {}

    explicit OwnedBuffer(size_t count) : ptr_(nullptr), size_(0) {
        // YOUR CODE HERE
    }

    ~OwnedBuffer() {
        // YOUR CODE HERE
    }

    // Delete copy
    OwnedBuffer(const OwnedBuffer&) = delete;
    OwnedBuffer& operator=(const OwnedBuffer&) = delete;

    // Move constructor
    OwnedBuffer(OwnedBuffer&& other) noexcept
        : ptr_(nullptr), size_(0) {
        // YOUR CODE HERE — use std::exchange
    }

    // Move assignment
    OwnedBuffer& operator=(OwnedBuffer&& other) noexcept {
        // YOUR CODE HERE — check self-assignment, free current, transfer
        return *this;
    }

    T* get() noexcept { return ptr_; }
    const T* get() const noexcept { return ptr_; }
    size_t size() const noexcept { return size_; }
    size_t bytes() const noexcept { return size_ * sizeof(T); }

    void resize(size_t new_count) {
        // YOUR CODE HERE — free and reallocate (content NOT preserved)
    }

    void copy_from(const T* src, size_t count) {
        // YOUR CODE HERE
    }

    void copy_to(T* dst, size_t count) const {
        // YOUR CODE HERE
    }

private:
    T* ptr_;
    size_t size_;
};

}  // namespace stx
