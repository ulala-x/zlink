# Benchmark summary (runs=10, reuse build)

Command: ./run_benchmarks.sh --runs 10 --reuse-build
Build dir: /home/ulalax/project/ulalax/zlink/build/bench
Patterns: PAIR, PUBSUB, DEALER_DEALER, DEALER_ROUTER, ROUTER_ROUTER, ROUTER_ROUTER_POLL
Transports: tcp, inproc, ipc
Sizes: 64B, 256B, 1024B, 65536B, 131072B, 262144B
Protocol: ZLINK_PROTOCOL unset => ZMP (assumed)

Notes:
- Diff (%) is zlink vs libzmq. Throughput: positive = faster. Latency: positive = lower latency.
- Full log: docs/team/20260122_asio-io-optimizations/results/bench_runs10_all_patterns.txt

Why performance recovered after removing mimalloc (likely causes):
- ZMQ workloads here are dominated by frequent small alloc/free for messages and buffers. Allocator overhead is a large fraction of total cost, so a slower allocator shows up directly as lower throughput and higher latency.
- zlink's allocator path previously routed msg/decoder allocations through mimalloc, bypassing libzmq's usual allocation/reuse behavior. That change increased allocator traffic in hot paths instead of reusing internal buffers.
- mimalloc uses per-thread heaps; cross-thread frees (common in I/O thread and socket thread handoff) can add bookkeeping and delayed free costs. This pattern is less friendly to mimalloc than a single-threaded or thread-local allocation pattern.
- glibc malloc with tcache performs very well for small objects; in this benchmark pattern it aligns better with libzmq's design, so switching back reduces overhead.
- The custom allocator wrapper added extra branches and indirection in tight loops, which can hurt instruction cache and branch prediction in micro-ops heavy paths.
- The inproc results also improved with mimalloc removed, which points to allocator overhead rather than network or ASIO as the dominant source.

Confidence:
- These are reasoned causes based on benchmark behavior; a perf/heap profile would confirm the exact hot spots.

Throughput highlights:
- Best: ROUTER_ROUTER tcp 131072B +18.50%
- Worst: PAIR tcp 262144B -15.33%
- Other notable gains: ROUTER_ROUTER_POLL inproc 65536B +15.20%, PUBSUB ipc 131072B +11.02%, ROUTER_ROUTER_POLL tcp 131072B +10.56%, PAIR inproc 65536B +10.43%
- Other notable regressions: PUBSUB tcp 262144B -8.51%, DEALER_ROUTER tcp 262144B -6.50%, DEALER_ROUTER tcp 131072B -5.77%, DEALER_ROUTER ipc 131072B -5.44%

Latency highlights:
- Best (lower latency): ROUTER_ROUTER_POLL ipc 131072B +21.09%, ROUTER_ROUTER ipc 131072B +17.77%
- Worst (higher latency): PAIR inproc 1024B -16.67%, PAIR tcp 262144B -16.00%, DEALER_DEALER tcp 262144B -15.27%
