# Lab 03 Results: Processing Pipeline

## Date Completed:

## Design Pattern Observations:
<!-- Strategy: what did the ProcessingStage interface buy you? -->
<!-- Pimpl: what compile-time dependency did it break? -->
<!-- Factory: why not just call make_unique directly? -->
<!-- Builder: what validation did it enable that raw construction wouldn't? -->

## Connection to SigTekX:
<!-- How do WindowStage/FFTStage/MagnitudeStage map to the real CUDA stages? -->
<!-- Why does the real FFTStage use cuFFT while yours uses naive DFT? -->
<!-- Why nfft/2+1 output bins? (Hermitian symmetry) -->
