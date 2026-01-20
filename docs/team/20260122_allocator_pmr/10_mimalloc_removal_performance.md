# Mimalloc removal and performance recovery

Context
- Benchmark tool: benchwithzmq/run_benchmarks.sh
- Command: ./run_benchmarks.sh --runs 10 --reuse-build
- Build dir: /home/ulalax/project/ulalax/zlink/build/bench
- Patterns: PAIR, PUBSUB, DEALER_DEALER, DEALER_ROUTER, ROUTER_ROUTER, ROUTER_ROUTER_POLL
- Transports: tcp, inproc, ipc
- Sizes: 64B, 256B, 1024B, 65536B, 131072B, 262144B
- Protocol: ZLINK_PROTOCOL unset (default ZMP)
- Full logs: docs/team/20260122_asio-io-optimizations/results/bench_runs10_all_patterns.txt
- Summary: docs/team/20260122_asio-io-optimizations/results/bench_runs10_all_patterns_summary.md

Observed outcome
- After removing mimalloc integration and using std::malloc/free in the allocator path, zlink results are close to libzmq across most patterns and sizes.
- Remaining deltas are mixed and within typical benchmark variance for this suite (some wins, some regressions), with no consistent across-the-board penalty.

Why performance recovered (likely causes)
- Hot-path allocation rate: ZMQ workloads here are dominated by frequent small alloc/free for message frames and decoding buffers. Allocator cost shows up directly in throughput and latency.
- Bypassed reuse path: The mimalloc-backed allocator route replaced libzmq's normal allocation/reuse behavior, increasing allocator traffic instead of reusing buffers in tight loops.
- Cross-thread free cost: This code frequently frees on a different thread than it allocates (I/O thread vs socket thread). mimalloc's per-thread heap model incurs extra bookkeeping and remote free handling in this pattern.
- glibc tcache fit: The default allocator is already very optimized for small, short-lived allocations and aligns well with libzmq's design, so it can be faster for this workload.
- Extra indirection: The allocator wrapper adds branches and indirection in tight loops, which can hurt I-cache and branch prediction when message rate is high.
- Inproc improvements: Gains were seen even for inproc (no network), pointing to allocator overhead rather than transport/ASIO effects as the primary driver.

Confidence and limits
- These are reasoned causes based on the benchmark deltas and the allocation pattern in the codebase; exact attribution needs profiling.
- To confirm, collect perf/flamegraph data in a build that can toggle mimalloc and compare allocator hot spots and cross-thread frees.
