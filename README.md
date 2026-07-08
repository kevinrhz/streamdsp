# streamdsp

## Purpose

A real-time streaming signal-processing pipeline, built from scratch in pure C++17 — no CUDA, no external DSP libraries. Every primitive (RAII resource wrappers, a lock-free SPSC ring buffer, a stage-based processing pipeline, threaded batch/streaming executors) is implemented from first principles rather than pulled from a framework.

## Quick Start

```bash
cd streamdsp
mkdir build && cd build
cmake ..
cmake --build .

# Run individual components
./lab01_test_owned_buffer
./lab02_test_ring_buffer
./lab03_test_stages
./lab03_test_pipeline
./lab04_test_batch
./lab04_test_streaming

# Or run all tests for a given component
cmake --build . --target lab01_run
cmake --build . --target lab02_run
cmake --build . --target lab03_run
cmake --build . --target lab04_run
```

## Components

| Component | What It Does |
|-----------|---------------|
| RAII wrappers (`Buffer<T>`, `File`) | Move-only resource ownership over heap memory and POSIX file descriptors |
| Lock-free SPSC ring buffer | Atomics-based circular buffer for single-producer/single-consumer streaming |
| Processing pipeline | Strategy + Pimpl + Factory + Builder — composable Window/FFT/Magnitude stages |
| Batch + streaming executors | Threaded execution with producer-consumer synchronization |

## How It Works

Each component has:
- **Header stubs** with `// TODO` markers showing exactly what to implement
- **Complete test suites** that pass once the implementation is correct
- **RESULTS.md** notes documenting design decisions and what was learned building each piece

The tests are written. The job is to make them pass.

## Roadmap

Capstone: wire the pipeline end-to-end into a WAV file reader, producing a frequency spectrum from real audio:

```
input.wav -> chunk reader -> RingBuffer -> WindowStage -> FFTStage -> MagnitudeStage -> spectrum output
```
