# Processing Pipeline — Design Notes

## Date Completed:

## Design Pattern Observations:
<!-- Strategy: what did the ProcessingStage interface buy you? -->
<!-- Pimpl: what compile-time dependency did it break? -->
<!-- Factory: why not just call make_unique directly? -->
<!-- Builder: what validation did it enable that raw construction wouldn't? -->

## Design Precedent: GPU-Accelerated Pipelines
<!-- How do WindowStage/FFTStage/MagnitudeStage map to a CUDA-accelerated pipeline (cuFFT, etc.)? -->
<!-- Why would a GPU implementation use cuFFT while this one uses a naive DFT? -->
<!-- Why nfft/2+1 output bins? (Hermitian symmetry) -->
