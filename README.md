# SigTekX Reconstruction Labs

## Purpose

Rebuild the core components of SigTekX from scratch in pure C++17 — no CUDA, no AI assistance, no looking at the original code until you're stuck for 30+ minutes. When every test passes and you can rewrite each lab from a blank file, you own your project.

## Quick Start

```bash
cd sigtekx-reconstruction
mkdir build && cd build
cmake ..
cmake --build .

# Run individual labs
./lab01_test_owned_buffer
./lab02_test_ring_buffer
./lab03_test_stages
./lab03_test_pipeline
./lab04_test_batch
./lab04_test_streaming

# Or run all tests for a lab
cmake --build . --target lab01_run
cmake --build . --target lab02_run
cmake --build . --target lab03_run
cmake --build . --target lab04_run
```

## Lab Order

| Lab | What You Build | SigTekX Equivalent | Days |
|-----|---------------|-------------------|------|
| 01 | RAII wrappers (OwnedBuffer, OwnedFile) | `cuda_wrappers.hpp` (DeviceBuffer, PinnedHostBuffer, CudaStream) | 1.5 |
| 02 | Lock-free SPSC ring buffer with atomics | `ring_buffer.hpp` | 2 |
| 03 | Processing pipeline (Strategy + Pimpl + Factory + Builder) | `processing_stage.hpp/cpp`, `pipeline_builder.hpp/cpp` | 2 |
| 04 | Batch + Streaming executors with threading | `batch_executor.cpp`, `streaming_executor.cpp` | 2 |

## How It Works

Each lab has:
- **Header stubs** with `// TODO` markers showing exactly what to implement
- **Complete test suites** that will pass once your implementation is correct
- **RESULTS.md** templates to document what you learned

The tests are written. Your job is to make them pass.

## Docs

- `docs/SIGTEKX_STUDY_GUIDE.md` — What to understand about each SigTekX component
- `docs/SIGTEKX_LAB_SYLLABUS.md` — Detailed requirements for each lab
