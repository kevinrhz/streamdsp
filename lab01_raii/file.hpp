/**
 * @file file.hpp
 * @brief RAII move-only wrapper around a POSIX file descriptor.
 * Mirrors the handle-wrapping pattern of sigtekx::CudaStream / CufftPlan.
 *
 * Open flags (combine with |):
 *   O_RDONLY  — read only
 *   O_WRONLY  — write only
 *   O_RDWR    — read + write
 *   O_CREAT   — create if missing  (requires mode arg, e.g. 0644)
 *   O_TRUNC   — truncate to zero on open
 *   O_APPEND  — all writes go to end of file
 *
 * Common mode: 0644 → owner rw, group/others r  (ignored unless O_CREAT)
 */

#pragma once

#include <unistd.h>  // read, write, close
#include <fcntl.h>   // open, O_* flags
#include <utility>

namespace stx {

class File {
public:
    File() : fd_(-1) {}

    // mode is only used when O_CREAT is in flags; safe to pass 0 otherwise
    File(const char* path, int flags, mode_t mode) : fd_(-1) {
        fd_ = open(path, flags, mode);
    }

    // guard required - close(-1) is an error, unlike delete[] nullptr which is safe
    ~File() {
        if (fd_ != -1) close(fd_);
    }

    File(const File&) = delete;
    File& operator=(const File&) = delete;

    // sentinel is -1 (int handle), not nullptr (pointer) - same exchange pattern as Buffer
    File(File&& other) noexcept : fd_(-1) {
        fd_ = std::exchange(other.fd_, -1);
    }

    File& operator=(File&& other) noexcept {
        if (this == &other) return *this;
        if (fd_ != -1) close(fd_);    // free current before taking other's
        fd_ = std::exchange(other.fd_, -1);
        return *this;
    }
    
    int  get()      const noexcept { return fd_; }
    bool is_valid() const noexcept { return fd_ != -1; }

    // :: prefix = global scope — avoids infinite recursion into our own method
    // write takes const void* (caller's buffer is read-only from our side)
    // read  takes void*       (we write into caller's buffer, must be mutable)
    ssize_t write(const void* buf, size_t n) { return ::write(fd_, buf, n); }
    ssize_t read(void* buf, size_t n)        { return ::read(fd_, buf, n);  }

private:
    int fd_ = -1;
};

} // namespace stx
