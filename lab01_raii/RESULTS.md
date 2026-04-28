# Lab 01 Results: RAII Resource Wrappers

> Formatted with AI assistance. Personal answers and reasoning are in the
> mastery quiz transcript: `grades/2026-04-27_lab01_mastery.md`.

## Date Completed
April 27, 2026

## Status
- `Buffer<T>`, `File`, `ResourceException` + `CHECK_POSIX` — all implemented and passing tests
- Mastery quiz taken: **62% — pass with significant gaps** (full Q&A in grades file)

## Key Observations
- All three resource types collapse to the same RAII shape: acquire-once-or-fail in the constructor, release-exactly-once in the destructor, copy deleted, move transfers ownership.
- Difference between the wrappers is only the resource representation: heap pointer (`T*`) vs opaque kernel handle (`int fd_`) vs error envelope (`std::runtime_error` subclass).
- `CHECK_POSIX` is mostly a preprocessor exercise — `#call`, `__FILE__`, `__LINE__`, and `do/while(0)` together convert a C-style return-code call into a typed C++ exception with source-level context.

## What I Learned About Move Semantics
- Move ctor steals from a fresh object — no existing resource to clean up.
- Move-assign must (1) self-assignment check, (2) free the existing resource, (3) steal from `other`, (4) return `*this`.
- `std::exchange(other.x, sentinel)` is the one-liner that both takes the value and leaves `other` in a destructible empty state — prevents double-free.
- `std::move` is a cast, not an action. It reclassifies an lvalue as an rvalue so the rvalue-reference overload becomes callable.
- `noexcept` on the move ctor is what makes the class usable inside `std::vector` (vector falls back to copy on grow if move can throw, and copy is `= delete` here).

## Connection to SigTekX
| This lab | SigTekX equivalent | Resource managed |
|---|---|---|
| `Buffer<T>` | `DeviceBuffer<T>` / `PinnedHostBuffer<T>` | heap allocation (`new T[]` ↔ `cudaMalloc`) |
| `File` | `CudaStream`, `CufftPlan`, `CudaEvent` | opaque handle with sentinel (`-1` ↔ `nullptr`) |
| `ResourceException` + `CHECK_POSIX` | `CudaException` + `SIGTEKX_CUDA_CHECK` / `CUFFT_CHECK` | C-style error code → typed C++ exception with file:line |

Same pattern across all of them because they share one shape: acquire-or-fail, release-exactly-once, opaque to the user.

## Gaps to revisit (from quiz)
1. `= delete` is a compile-time prohibition, not an optimization
2. `explicit` blocks the constructor as an implicit conversion (`Buffer b = 128`), not argument type conversion
3. Move-assign must free the existing resource before stealing
4. Preprocessor: `#param` stringification, `__FILE__`/`__LINE__` are compile-time, `do/while(0)` is the multi-statement macro idiom
5. Member initializer list is mandatory for base classes, references, const members, and types without default ctors
6. `const` after a method = `this` is `const`; only const methods callable on const objects
7. `noexcept` move ctor unlocks STL container compatibility
8. Pointer const placement — read right-to-left around `*`
9. lvalue vs rvalue references and what `std::move` actually does (biggest gap)
10. `|` is bitwise OR (bitmask), `0644` is octal Unix permissions
11. POSIX semantics — fd is an index into a per-process table, not a pointer
