# zlink vs libzmq Performance Benchmark Results

## Test Environment

| Item | Value |
|------|-------|
| OS | Linux (WSL2) 6.6.87.2-microsoft-standard-WSL2 |
| CPU | Intel Core Ultra 7 265K |
| Architecture | x86_64 |
| C++ Standard | C++20 |
| CPU Affinity | `taskset -c 1` (pinned to CPU 1) |
| Iterations | 3 runs per configuration (min/max trimmed) |
| Date | 2026-01-13 |

## Methodology

- Command: `benchwithzmq/run_benchmarks.sh --with-libzmq --runs 3`
- Each configuration runs 3 iterations; min/max are dropped (median kept).
- Throughput diff = `(zlink - libzmq) / libzmq`.
- Latency diff uses inverse `(libzmq - zlink) / libzmq` (positive means lower latency for zlink).

## Summary

- Throughput diff range: -20.38% to +33.20% across all patterns/transports/sizes.
- Latency inverse diff range: -47.16% to +10.00% (positive means lower latency for zlink).
- Results captured on WSL2; expect variance across runs and hosts.

## Phase 1 Zero-Copy Sanity Check (Runs=1)

- Command: `taskset -c 1 python3 benchwithzmq/run_comparison.py DEALER_ROUTER --build-dir build-bench-asio --runs 1 --refresh-libzmq`
- Results: `benchwithzmq/benchmark_result.txt`
- Note: Throughput improves for some large sizes, but latency is still worse
  than libzmq across TCP/IPC/inproc. Full 3-run sweep is still needed.

## Detailed Results

## PATTERN: PAIR
  > Benchmarking libzmq for PAIR...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing inproc | 64B: 1 2 3 Done
    Testing inproc | 256B: 1 2 3 Done
    Testing inproc | 1024B: 1 2 3 Done
    Testing inproc | 65536B: 1 2 3 Done
    Testing inproc | 131072B: 1 2 3 Done
    Testing inproc | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 256B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 65536B: 1 2 3 Done
    Testing ipc | 131072B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for PAIR...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing inproc | 64B: 1 2 3 Done
    Testing inproc | 256B: 1 2 3 Done
    Testing inproc | 1024B: 1 2 3 Done
    Testing inproc | 65536B: 1 2 3 Done
    Testing inproc | 131072B: 1 2 3 Done
    Testing inproc | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 256B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 65536B: 1 2 3 Done
    Testing ipc | 131072B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   6.38 M/s |   5.93 M/s |   -7.11% |
| | Latency |     4.66 us |     5.31 us |  -13.95% (inv) |
| 256B | Throughput |   3.82 M/s |   3.39 M/s |  -11.30% |
| | Latency |     4.65 us |     5.42 us |  -16.56% (inv) |
| 1024B | Throughput |   1.46 M/s |   1.28 M/s |  -12.40% |
| | Latency |     4.82 us |     5.44 us |  -12.86% (inv) |
| 65536B | Throughput |   0.04 M/s |   0.04 M/s |   -3.94% |
| | Latency |    11.94 us |    14.45 us |  -21.02% (inv) |
| 131072B | Throughput |   0.03 M/s |   0.02 M/s |  -10.52% |
| | Latency |    17.74 us |    24.13 us |  -36.02% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  +30.27% |
| | Latency |    29.43 us |    41.07 us |  -39.55% (inv) |

### Transport: inproc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   8.17 M/s |   8.35 M/s |   +2.12% |
| | Latency |     0.07 us |     0.07 us |   +0.00% (inv) |
| 256B | Throughput |   8.47 M/s |   8.40 M/s |   -0.79% |
| | Latency |     0.07 us |     0.07 us |   +0.00% (inv) |
| 1024B | Throughput |   4.84 M/s |   4.63 M/s |   -4.41% |
| | Latency |     0.09 us |     0.09 us |   +0.00% (inv) |
| 65536B | Throughput |   0.16 M/s |   0.15 M/s |   -2.21% |
| | Latency |     1.96 us |     1.93 us |   +1.53% (inv) |
| 131072B | Throughput |   0.10 M/s |   0.10 M/s |   -1.73% |
| | Latency |     3.47 us |     3.58 us |   -3.17% (inv) |
| 262144B | Throughput |   0.06 M/s |   0.06 M/s |   -7.74% |
| | Latency |     6.82 us |     6.71 us |   +1.61% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   6.29 M/s |   6.12 M/s |   -2.69% |
| | Latency |     4.52 us |     4.80 us |   -6.19% (inv) |
| 256B | Throughput |   4.11 M/s |   3.64 M/s |  -11.51% |
| | Latency |     4.47 us |     4.89 us |   -9.40% (inv) |
| 1024B | Throughput |   1.60 M/s |   1.52 M/s |   -4.97% |
| | Latency |     4.66 us |     4.89 us |   -4.94% (inv) |
| 65536B | Throughput |   0.04 M/s |   0.04 M/s |   -9.25% |
| | Latency |    11.96 us |    14.48 us |  -21.07% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |   -2.18% |
| | Latency |    17.93 us |    23.68 us |  -32.07% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  +13.48% |
| | Latency |    29.84 us |    41.22 us |  -38.14% (inv) |

## PATTERN: PUBSUB
  > Benchmarking libzmq for PUBSUB...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing inproc | 64B: 1 2 3 Done
    Testing inproc | 256B: 1 2 3 Done
    Testing inproc | 1024B: 1 2 3 Done
    Testing inproc | 65536B: 1 2 3 Done
    Testing inproc | 131072B: 1 2 3 Done
    Testing inproc | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 256B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 65536B: 1 2 3 Done
    Testing ipc | 131072B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for PUBSUB...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing inproc | 64B: 1 2 3 Done
    Testing inproc | 256B: 1 2 3 Done
    Testing inproc | 1024B: 1 2 3 Done
    Testing inproc | 65536B: 1 2 3 Done
    Testing inproc | 131072B: 1 2 3 Done
    Testing inproc | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 256B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 65536B: 1 2 3 Done
    Testing ipc | 131072B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.18 M/s |   4.00 M/s |   -4.09% |
| | Latency |     0.24 us |     0.23 us |   +4.17% (inv) |
| 256B | Throughput |   2.32 M/s |   2.49 M/s |   +7.29% |
| | Latency |     0.43 us |     0.40 us |   +6.98% (inv) |
| 1024B | Throughput |   0.88 M/s |   0.94 M/s |   +5.93% |
| | Latency |     1.13 us |     1.07 us |   +5.31% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.03 M/s |   -4.82% |
| | Latency |    30.70 us |    32.26 us |   -5.08% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |   -7.13% |
| | Latency |    50.74 us |    54.63 us |   -7.67% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |   -9.11% |
| | Latency |    94.85 us |   104.35 us |  -10.02% (inv) |

### Transport: inproc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   6.10 M/s |   6.08 M/s |   -0.40% |
| | Latency |     0.16 us |     0.16 us |   +0.00% (inv) |
| 256B | Throughput |   4.79 M/s |   5.01 M/s |   +4.48% |
| | Latency |     0.21 us |     0.20 us |   +4.76% (inv) |
| 1024B | Throughput |   3.01 M/s |   2.82 M/s |   -6.31% |
| | Latency |     0.33 us |     0.36 us |   -9.09% (inv) |
| 65536B | Throughput |   0.16 M/s |   0.16 M/s |   -0.36% |
| | Latency |     6.38 us |     6.40 us |   -0.31% (inv) |
| 131072B | Throughput |   0.11 M/s |   0.11 M/s |   +3.43% |
| | Latency |     9.45 us |     9.14 us |   +3.28% (inv) |
| 262144B | Throughput |   0.07 M/s |   0.07 M/s |   -0.43% |
| | Latency |    15.26 us |    15.33 us |   -0.46% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.44 M/s |   4.30 M/s |   -3.28% |
| | Latency |     0.23 us |     0.23 us |   +0.00% (inv) |
| 256B | Throughput |   2.73 M/s |   2.60 M/s |   -4.56% |
| | Latency |     0.37 us |     0.38 us |   -2.70% (inv) |
| 1024B | Throughput |   1.02 M/s |   0.98 M/s |   -3.31% |
| | Latency |     0.98 us |     1.02 us |   -4.08% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.03 M/s |   -4.07% |
| | Latency |    29.06 us |    30.29 us |   -4.23% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |   -5.57% |
| | Latency |    50.12 us |    53.08 us |   -5.91% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |   -4.92% |
| | Latency |   100.02 us |   105.19 us |   -5.17% (inv) |

## PATTERN: DEALER_DEALER
  > Benchmarking libzmq for DEALER_DEALER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing inproc | 64B: 1 2 3 Done
    Testing inproc | 256B: 1 2 3 Done
    Testing inproc | 1024B: 1 2 3 Done
    Testing inproc | 65536B: 1 2 3 Done
    Testing inproc | 131072B: 1 2 3 Done
    Testing inproc | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 256B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 65536B: 1 2 3 Done
    Testing ipc | 131072B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for DEALER_DEALER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing inproc | 64B: 1 2 3 Done
    Testing inproc | 256B: 1 2 3 Done
    Testing inproc | 1024B: 1 2 3 Done
    Testing inproc | 65536B: 1 2 3 Done
    Testing inproc | 131072B: 1 2 3 Done
    Testing inproc | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 256B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 65536B: 1 2 3 Done
    Testing ipc | 131072B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   6.03 M/s |   5.76 M/s |   -4.60% |
| | Latency |     4.84 us |     5.31 us |   -9.71% (inv) |
| 256B | Throughput |   3.64 M/s |   3.37 M/s |   -7.23% |
| | Latency |     4.81 us |     5.40 us |  -12.27% (inv) |
| 1024B | Throughput |   1.43 M/s |   1.33 M/s |   -6.73% |
| | Latency |     4.91 us |     5.51 us |  -12.22% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.04 M/s |  +33.20% |
| | Latency |    12.17 us |    14.60 us |  -19.97% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |  +23.93% |
| | Latency |    18.24 us |    24.45 us |  -34.05% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  -13.75% |
| | Latency |    29.35 us |    43.19 us |  -47.16% (inv) |

### Transport: inproc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   7.84 M/s |   7.87 M/s |   +0.36% |
| | Latency |     0.07 us |     0.08 us |  -14.29% (inv) |
| 256B | Throughput |   8.03 M/s |   8.08 M/s |   +0.72% |
| | Latency |     0.07 us |     0.07 us |   +0.00% (inv) |
| 1024B | Throughput |   4.93 M/s |   4.82 M/s |   -2.20% |
| | Latency |     0.10 us |     0.09 us |  +10.00% (inv) |
| 65536B | Throughput |   0.16 M/s |   0.16 M/s |   -3.26% |
| | Latency |     1.93 us |     1.96 us |   -1.55% (inv) |
| 131072B | Throughput |   0.10 M/s |   0.11 M/s |   +3.69% |
| | Latency |     3.47 us |     3.77 us |   -8.65% (inv) |
| 262144B | Throughput |   0.06 M/s |   0.06 M/s |   -4.09% |
| | Latency |     6.85 us |     7.36 us |   -7.45% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   6.28 M/s |   5.93 M/s |   -5.54% |
| | Latency |     4.36 us |     4.85 us |  -11.24% (inv) |
| 256B | Throughput |   4.08 M/s |   3.75 M/s |   -8.09% |
| | Latency |     4.38 us |     4.83 us |  -10.27% (inv) |
| 1024B | Throughput |   1.66 M/s |   1.54 M/s |   -7.29% |
| | Latency |     4.39 us |     4.92 us |  -12.07% (inv) |
| 65536B | Throughput |   0.04 M/s |   0.04 M/s |   -3.49% |
| | Latency |    12.02 us |    14.47 us |  -20.38% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |  -12.37% |
| | Latency |    18.57 us |    23.53 us |  -26.71% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  -10.01% |
| | Latency |    29.72 us |    42.92 us |  -44.41% (inv) |

## PATTERN: DEALER_ROUTER
  > Benchmarking libzmq for DEALER_ROUTER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing inproc | 64B: 1 2 3 Done
    Testing inproc | 256B: 1 2 3 Done
    Testing inproc | 1024B: 1 2 3 Done
    Testing inproc | 65536B: 1 2 3 Done
    Testing inproc | 131072B: 1 2 3 Done
    Testing inproc | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 256B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 65536B: 1 2 3 Done
    Testing ipc | 131072B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for DEALER_ROUTER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing inproc | 64B: 1 2 3 Done
    Testing inproc | 256B: 1 2 3 Done
    Testing inproc | 1024B: 1 2 3 Done
    Testing inproc | 65536B: 1 2 3 Done
    Testing inproc | 131072B: 1 2 3 Done
    Testing inproc | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 256B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 65536B: 1 2 3 Done
    Testing ipc | 131072B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.59 M/s |   4.39 M/s |   -4.40% |
| | Latency |     4.85 us |     5.38 us |  -10.93% (inv) |
| 256B | Throughput |   3.14 M/s |   3.10 M/s |   -1.57% |
| | Latency |     4.86 us |     5.34 us |   -9.88% (inv) |
| 1024B | Throughput |   1.25 M/s |   1.31 M/s |   +5.17% |
| | Latency |     5.69 us |     5.43 us |   +4.57% (inv) |
| 65536B | Throughput |   0.02 M/s |   0.03 M/s |  +16.00% |
| | Latency |    12.33 us |    14.32 us |  -16.14% (inv) |
| 131072B | Throughput |   0.01 M/s |   0.01 M/s |   -0.24% |
| | Latency |    17.54 us |    24.11 us |  -37.46% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  -20.38% |
| | Latency |    28.96 us |    42.37 us |  -46.31% (inv) |

### Transport: inproc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   7.36 M/s |   6.20 M/s |  -15.76% |
| | Latency |     0.10 us |     0.11 us |  -10.00% (inv) |
| 256B | Throughput |   7.31 M/s |   5.92 M/s |  -19.04% |
| | Latency |     0.10 us |     0.10 us |   +0.00% (inv) |
| 1024B | Throughput |   4.43 M/s |   3.69 M/s |  -16.74% |
| | Latency |     0.13 us |     0.12 us |   +7.69% (inv) |
| 65536B | Throughput |   0.14 M/s |   0.14 M/s |   -0.31% |
| | Latency |     1.84 us |     1.89 us |   -2.72% (inv) |
| 131072B | Throughput |   0.10 M/s |   0.09 M/s |   -5.92% |
| | Latency |     3.45 us |     3.61 us |   -4.64% (inv) |
| 262144B | Throughput |   0.06 M/s |   0.06 M/s |   +0.45% |
| | Latency |     6.79 us |     6.96 us |   -2.50% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.88 M/s |   4.73 M/s |   -3.02% |
| | Latency |     4.27 us |     4.73 us |  -10.77% (inv) |
| 256B | Throughput |   3.62 M/s |   3.23 M/s |  -10.73% |
| | Latency |     4.37 us |     4.93 us |  -12.81% (inv) |
| 1024B | Throughput |   1.60 M/s |   1.45 M/s |   -9.04% |
| | Latency |     4.46 us |     4.82 us |   -8.07% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.03 M/s |   -0.73% |
| | Latency |    11.62 us |    14.11 us |  -21.43% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |   -2.21% |
| | Latency |    17.94 us |    23.53 us |  -31.16% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  -13.68% |
| | Latency |    30.00 us |    40.84 us |  -36.13% (inv) |

## PATTERN: ROUTER_ROUTER
  > Benchmarking libzmq for ROUTER_ROUTER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing inproc | 64B: 1 2 3 Done
    Testing inproc | 256B: 1 2 3 Done
    Testing inproc | 1024B: 1 2 3 Done
    Testing inproc | 65536B: 1 2 3 Done
    Testing inproc | 131072B: 1 2 3 Done
    Testing inproc | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 256B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 65536B: 1 2 3 Done
    Testing ipc | 131072B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for ROUTER_ROUTER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing inproc | 64B: 1 2 3 Done
    Testing inproc | 256B: 1 2 3 Done
    Testing inproc | 1024B: 1 2 3 Done
    Testing inproc | 65536B: 1 2 3 Done
    Testing inproc | 131072B: 1 2 3 Done
    Testing inproc | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 256B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 65536B: 1 2 3 Done
    Testing ipc | 131072B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   3.92 M/s |   4.16 M/s |   +6.15% |
| | Latency |     4.11 us |     4.09 us |   +0.49% (inv) |
| 256B | Throughput |   3.02 M/s |   2.90 M/s |   -4.12% |
| | Latency |     4.14 us |     4.17 us |   -0.72% (inv) |
| 1024B | Throughput |   1.31 M/s |   1.16 M/s |  -11.27% |
| | Latency |     4.04 us |     4.26 us |   -5.45% (inv) |
| 65536B | Throughput |   0.05 M/s |   0.04 M/s |  -11.55% |
| | Latency |    10.83 us |    13.73 us |  -26.78% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |   +0.49% |
| | Latency |    17.09 us |    23.95 us |  -40.14% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  -18.98% |
| | Latency |    37.32 us |    48.91 us |  -31.06% (inv) |

### Transport: inproc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.97 M/s |   5.09 M/s |   +2.46% |
| | Latency |     0.15 us |     0.16 us |   -6.67% (inv) |
| 256B | Throughput |   5.26 M/s |   5.28 M/s |   +0.31% |
| | Latency |     0.15 us |     0.16 us |   -6.67% (inv) |
| 1024B | Throughput |   3.67 M/s |   3.78 M/s |   +2.94% |
| | Latency |     0.17 us |     0.18 us |   -5.88% (inv) |
| 65536B | Throughput |   0.16 M/s |   0.15 M/s |   -2.59% |
| | Latency |     1.95 us |     1.96 us |   -0.51% (inv) |
| 131072B | Throughput |   0.11 M/s |   0.11 M/s |   +0.86% |
| | Latency |     3.65 us |     3.58 us |   +1.92% (inv) |
| 262144B | Throughput |   0.07 M/s |   0.06 M/s |   -2.10% |
| | Latency |     6.93 us |     6.89 us |   +0.58% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.34 M/s |   4.36 M/s |   +0.54% |
| | Latency |     3.73 us |     3.56 us |   +4.56% (inv) |
| 256B | Throughput |   3.29 M/s |   3.20 M/s |   -2.78% |
| | Latency |     3.72 us |     3.67 us |   +1.34% (inv) |
| 1024B | Throughput |   1.49 M/s |   1.39 M/s |   -6.88% |
| | Latency |     3.78 us |     3.83 us |   -1.32% (inv) |
| 65536B | Throughput |   0.04 M/s |   0.04 M/s |   -4.79% |
| | Latency |    13.23 us |    13.46 us |   -1.74% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |   -9.16% |
| | Latency |    26.20 us |    23.83 us |   +9.05% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |   +6.20% |
| | Latency |    36.70 us |    48.87 us |  -33.16% (inv) |
