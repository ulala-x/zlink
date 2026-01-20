# Allocator/PMR Migration Overview

NOTE: The PMR/mimalloc allocator experiment has been removed. The allocator
wrapper now uses std::malloc/free and hot-path wiring has been reverted. The
sections below include historical context for reference.

## Goal

- Maintain an internal allocator wrapper for high-frequency buffers.
- Keep allocation behavior aligned with std::malloc/free for now.

## Design

- Internal allocator wrapper (`src/allocator.hpp`, `src/allocator.cpp`).
- Current implementation forwards to std::malloc/free.

## Applied Scope

- Current: hot-path wiring reverted to std::malloc/free in msg/encoder/decoder.
- Historical (pre-removal): msg/encoder/decoder were wired to allocator wrapper.

## Notes

- Allocator wrapper remains for future experiments but is currently a thin pass-through.
- PMR pooling and mimalloc backend are no longer part of the build.

## Implementation Summary

- Allocator wrapper remains but uses std::malloc/free.
- Hot-path wiring to alloc/dealloc was reverted.

## Next Steps

- If allocator experiments resume, use a dedicated branch for comparisons.

## Build/Test

```bash
./build.sh
```

- Result: 61 tests passed, 4 fuzzers skipped.

Mimalloc is no longer used or built in this repository.

## Benchmarks (Runs=3)

Commands:

```bash
./benchwithzmq/run_benchmarks.sh --runs 3

BENCH_NO_TASKSET=1 \
  python3 benchwithzmq/run_comparison.py --build-dir build/bench

BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 \
  python3 benchwithzmq/run_comparison.py --build-dir build/bench

# libzmq vs libzmq variance check (runs=3)
./benchwithzmq/run_benchmarks_zmq.sh --runs 3 --reuse-build \
  --output docs/team/20260122_allocator_pmr/09_benchmark_libzmq_vs_libzmq.txt
```

Logs:

- `docs/team/20260122_allocator_pmr/01_benchmark_default.txt`
- `docs/team/20260122_allocator_pmr/02_benchmark_pmr_pool.txt`
- `docs/team/20260122_allocator_pmr/03_benchmark_tl_pool_mimalloc.txt`
- `docs/team/20260122_allocator_pmr/04_benchmark_io_yqueue_pool.txt` (reverted experiment)
- `docs/team/20260122_allocator_pmr/05_benchmark_tl_pool_no_mimalloc.txt`
- `docs/team/20260122_allocator_pmr/06_benchmark_tl_pool_mimalloc.txt`
- `docs/team/20260122_allocator_pmr/07_benchmark_tl_pool_no_mimalloc_io2.txt`
- `docs/team/20260122_allocator_pmr/08_benchmark_tl_pool_mimalloc_io2.txt`
- `docs/team/20260122_allocator_pmr/09_benchmark_libzmq_vs_libzmq.txt`
- `docs/team/20260122_allocator_pmr/24_benchmark_runs10_all_sizes_summary.md`

Summary:

- Default allocator wrapper: results mostly close to libzmq with mixed +/- by pattern.
- Historical TL pool + mimalloc: small/medium messages mixed; INPROC showed gains.
- Historical I/O direct encode + yqueue pool (reverted): large-message TCP/IPC regressions grew; small messages mixed.
- Historical no-mimalloc baseline: throughput diffs improved in 84/108 cases, 24 regressions; latency still dominated by zlink I/O path.
- libzmq vs libzmq variance (runs=3): throughput diff mean +0.88%, median +0.32% (min -18.33%, max +27.05%); latency diff mean +0.06%, median 0.00% (min -17.02%, max +16.50%).
