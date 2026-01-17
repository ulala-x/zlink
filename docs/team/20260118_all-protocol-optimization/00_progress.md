# 20260118 all-protocol optimization progress

- Context: continuation of the all-protocol work in `docs/team/20260117_all-protocol-optimization/`.
- Goal: reduce the tcp 1024B gap without regressing 262144B, while keeping all patterns/protocols in scope.

## 2026-01-18

- Added env overrides for ASIO batch sizes to enable controlled experiments.
  - `ZMQ_ASIO_IN_BATCH_SIZE`
  - `ZMQ_ASIO_OUT_BATCH_SIZE`
- Ran tcp batch-size sweeps with `ZMQ_ASIO_TCP_MAX_TRANSFER=262144` at `BENCH_IO_THREADS=2` (unpinned).
- Findings captured in `01_tcp_batch_size_override_test.md`.

## Next

- Explore size-adaptive batching to keep 1024B gains while limiting 262144B regression.
- Re-run tcp matrix (all patterns) with the best candidate, then extend to ipc/inproc if results hold.
