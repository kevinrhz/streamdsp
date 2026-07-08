/**
 * @file resource_exception.hpp
 * @brief Exception type for failed POSIX calls + CHECK_POSIX macro.
 * Mirrors the typed error-check pattern common in CUDA codebases (e.g. a CUDA_CHECK
 * macro that turns a C-style error code into a typed exception with file:line context).
 *
 * Usage:
 *   CHECK_POSIX(open("/bad/path", O_RDONLY, 0));
 *   // throws ResourceException if open() returns -1
 */

#pragma once

#include <cerrno>    // errno
#include <cstring>   // strerror
#include <stdexcept> // std::runtime_error
#include <string>

namespace stx {

class ResourceException : public std::runtime_error {
public:
    // what() is inherited from std::runtime_error — returns the full message string
    ResourceException(int err_code, const char* call, const char* file, int line)
        : std::runtime_error(
              std::string(call) + " failed at " + file + ":" +
              std::to_string(line) + ": " + std::strerror(err_code)),
          err_code_(err_code), call_(call), file_(file), line_(line) {}

    int         error_code() const noexcept { return err_code_; }
    const char* call()       const noexcept { return call_; }
    const char* file()       const noexcept { return file_; }
    int         line()       const noexcept { return line_; }

private:
    int         err_code_;
    const char* call_;
    const char* file_;
    int         line_;
};

} // namespace stx

// CHECK_POSIX(call) — wrap any POSIX call that returns -1 on failure.
// #call stringifies the expression so the error message shows the actual code that failed.
// errno must be read immediately — the next system call will overwrite it.
// do/while(0) makes this safe inside if-statements without braces.
#define CHECK_POSIX(call)                                          \
    do {                                                           \
        if ((call) == -1)                                          \
            throw stx::ResourceException(errno, #call,            \
                                         __FILE__, __LINE__);      \
    } while (0)
