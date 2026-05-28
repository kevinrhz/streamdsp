/**
 * @file buffer.hpp
 * @brief RAII move-only wrapper for heap-allocated memory.
 * Mirrors sigtekx::DeviceBuffer<T> and sigtekx::PinnedHostBuffer<T>.
 */

#pragma once

#include <cstddef>
#include <cstring>
#include <utility>

namespace stx {

template <typename T>
class Buffer {
public:
    // default - make an empty buffer
    Buffer() : ptr_(nullptr), size_(0) {}

    // sized - make a buffer of N elements
    explicit Buffer(size_t count) : ptr_(nullptr), size_(0) {
        ptr_ = new T[count];
        size_ = count;
    }

    ~Buffer() {
        delete[] ptr_;
    }

    // Delete copy constructor and assignment
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    // Move constructor
    Buffer(Buffer&& other) noexcept
        : ptr_(nullptr), size_(0) {
        ptr_ = std::exchange(other.ptr_, nullptr);
        size_ = std::exchange(other.size_, 0);
    }

    // Move assignment
    Buffer& operator=(Buffer&& other) noexcept {
        if (this == &other) return *this;

        delete[] ptr_;
        ptr_ = std::exchange(other.ptr_, nullptr);
        size_ = std::exchange(other.size_, 0);
        return *this;
    }

    T*       get();        // this = Buffer<float>*       → mutable context, returns writable ptr
    const T* get() const;  // this = const Buffer<float>* → const context, returns read-only ptr

    size_t size()  const noexcept { return size_; }
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
