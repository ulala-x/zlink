# Final Summary (ASIO batching/writev exploration)

## Scope

- Investigated TCP/IPC performance gaps vs libzmq across message sizes.
- Added writev-based header/body split for large messages to reduce copies.
- Reintroduced dual batch sizes (small/large) for adaptive batching.
- Explored writev thresholds and batch-size combinations.

## Key Implementation Changes

- Vector async writes (writev) for header+body when threshold is met.
- Dual encoders (small/large) with message-size based selection.
- Env overrides:
  - `ZMQ_ASIO_OUT_BATCH_SIZE_{TCP,IPC,INPROC}`
  - `ZMQ_ASIO_OUT_BATCH_SIZE_SMALL_{TCP,IPC,INPROC}`
  - `ZMQ_ASIO_OUT_WRITEV_THRESHOLD_{TCP,IPC,INPROC}`
- Benchmark CPU pinning now opt-in via `BENCH_USE_TASKSET=1`.

## Findings (High-Level)

- Small messages (256B/1KB) can improve with dual 4KB/32KB batching.
- Large messages (64KB+) remain mixed across patterns; regressions persist.
- Lowering writev threshold (8KB/16KB) helps some small sizes but does not
  remove large-message regressions consistently.
- Single 4KB batch size hurts small-message performance overall.

## Final Position

- Keep dual batching (4KB small / 32KB large).
- Default writev threshold at 8KB (override via env if needed).
- Do not merge to main; keep as experimental branch.
