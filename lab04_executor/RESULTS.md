# Threaded Executors — Design Notes

## Date Completed:

## Batch Executor Observations:
<!-- How does round-robin buffer selection prevent data corruption? -->
<!-- What did the 3-thread simulation teach you about stream parallelism? -->

## Streaming Executor Observations:
<!-- How does the ring buffer integrate with the processing pipeline? -->
<!-- What happens with high overlap (0.875)? How many frames per submit? -->
<!-- How does the batched path differ from N sequential process calls? -->

## Design Precedent: GPU Stream Parallelism
<!-- Map these CPU threads to CUDA streams in a GPU pipeline -->
<!-- Map the Event class to a CUDA event -->
<!-- What does this streaming executor NOT have that a GPU implementation would? (DMA, pinned memory, etc.) -->
