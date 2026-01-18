# Allocator/PMR Migration Overview

## Goal

- Introduce a configurable allocator layer for high-frequency buffers.
- Enable optional PMR pooling without changing default behavior.

## Design

- New internal allocator wrapper (`src/allocator.hpp`, `src/allocator.cpp`).
- Default allocation uses `std::pmr::new_delete_resource` (behaviorally close to malloc/free).
- Optional pool resource via environment variable:
  - `ZMQ_PMR_POOL=1` enables `std::pmr::synchronized_pool_resource`.
- Optional thread-local pool:
  - `ZMQ_PMR_TL_POOL=1` enables `std::pmr::unsynchronized_pool_resource` for thread-local allocations.
- Optional allocator backend:
  - `-DZMQ_USE_MIMALLOC=ON` links mimalloc and uses it as the global PMR resource.
- Wrapper stores allocation size in a small header to allow size-less `dealloc()`.

## Applied Scope (initial)

- `msg_t` long message storage and long group storage.
- Encoder batch buffer (`encoder_base_t`).
- Decoder buffers (`c_single_allocator`, `shared_message_memory_allocator`).
  - Encoder/c_single_allocator use thread-local pool when enabled.

## Notes

- This change targets the hottest allocation paths first; other malloc/free sites remain unchanged.
- Pool resource choice is process-global (static). If we need per-context resources, we can revisit.
- Thread-local pool must only be used for allocations freed on the same thread.
- Monotonic resource is not applied yet; needs explicit reset points to avoid unbounded growth.

## Implementation Summary

- Added allocator wrapper and wired it into core msg/encoder/decoder paths.
- Switched `malloc/free` usage to `alloc/dealloc` in hot paths.
- Build system updated to compile allocator wrapper.
- Async output direct-encode and `yqueue_t` pool were tested and reverted due to regressions.

## Next Steps

- Extend allocator wrapper to remaining hot paths (e.g., trie/radix_tree).
- Add benchmarks comparing default vs `ZMQ_PMR_POOL=1`.

## Build/Test

```bash
./build.sh
```

- Result: 61 tests passed, 4 fuzzers skipped.

Mimalloc option:

```bash
cmake -B build -DZMQ_BUILD_TESTS=ON -DZMQ_USE_MIMALLOC=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

- Result: 61 tests passed, 4 fuzzers skipped.
- Note: mimalloc installed locally at `.deps/mimalloc/install` and used via `CMAKE_PREFIX_PATH`.

## Benchmarks (Runs=3)

Commands:

```bash
./benchwithzmq/run_benchmarks.sh --runs 3
ZMQ_PMR_POOL=1 ./benchwithzmq/run_benchmarks.sh --runs 3

BENCH_NO_TASKSET=1 ZMQ_PMR_TL_POOL=1 \
  python3 benchwithzmq/run_comparison.py --build-dir build/bench

BENCH_NO_TASKSET=1 ZMQ_PMR_TL_POOL=1 \
  python3 benchwithzmq/run_comparison.py --build-dir build/bench-mimalloc

BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 ZMQ_PMR_TL_POOL=1 \
  python3 benchwithzmq/run_comparison.py --build-dir build/bench

BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 ZMQ_PMR_TL_POOL=1 \
  python3 benchwithzmq/run_comparison.py --build-dir build/bench-mimalloc

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

Summary:

- Default allocator wrapper: results mostly close to libzmq with mixed +/- by pattern.
- `ZMQ_PMR_POOL=1`: small/medium message throughput regresses across TCP/IPC/INPROC; large messages are mixed.
- `ZMQ_PMR_TL_POOL=1` + mimalloc: small messages still regress; large messages mixed.
- I/O direct encode + yqueue pool (reverted): large-message TCP/IPC regressions grew in several patterns; small messages mixed.
- `ZMQ_PMR_TL_POOL=1` (no mimalloc): baseline for allocator-only changes, no CPU pinning.
- `ZMQ_PMR_TL_POOL=1` + mimalloc: throughput diffs improved vs no-mimalloc in 84/108 cases, 24 regressions; latency still dominated by zlink I/O path.
- libzmq vs libzmq variance (runs=3): throughput diff mean +0.88%, median +0.32% (min -18.33%, max +27.05%); latency diff mean +0.06%, median 0.00% (min -17.02%, max +16.50%).
