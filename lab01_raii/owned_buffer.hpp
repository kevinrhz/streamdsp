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
        ptr_ = new T[count];
        size_ = count;
    }

    ~OwnedBuffer() {
        delete[] ptr_;
    }

    // Delete copy
    OwnedBuffer(const OwnedBuffer&) = delete;
    OwnedBuffer& operator=(const OwnedBuffer&) = delete;

    // Move constructor
    OwnedBuffer(OwnedBuffer&& other) noexcept
        : ptr_(nullptr), size_(0) {
        ptr_ = std::exchange(other.ptr_, nullptr);
        size_ = std::exchange(other.size_, 0);
    }

    // Move assignment
    OwnedBuffer& operator=(OwnedBuffer&& other) noexcept {
        if (this == &other) return *this;

        delete[] ptr_;
        ptr_ = std::exchange(other.ptr_, nullptr);
        size_ = std::exchange(other.size_, 0);
        return *this;
    }

    T* get() noexcept { return ptr_; }
    const T* get() const noexcept { return ptr_; }
    size_t size() const noexcept { return size_; }
    size_t bytes() const noexcept { return size_ * sizeof(T); }

    void resize(size_t new_count) {
        delete[] ptr_;
        ptr_ = new T[new_count];
        size_ = new_count;
    }

    void copy_from(const T* src, size_t count) {
        // copy FROM somewhere INTO ME
        std::memcpy(ptr_, src, count * sizeof(T));
    }

    void copy_to(T* dst, size_t count) const {
        // copy FROM ME out TO somewhere
        std::memcpy(dst, ptr_, count * sizeof(T));
    }

private:
    T* ptr_;
    size_t size_;
};

}  // namespace stx
