# Unpinned IO Threads=2 Full Matrix (Cache-Keyed)

## Setup

- BENCH_NO_TASKSET=1 (no CPU pinning)
- BENCH_IO_THREADS=2
- BENCH_TRANSPORTS=inproc,tcp,ipc
- BENCH_MSG_SIZES=64,256,1024,65536,131072,262144
- runs=3, refresh libzmq baseline
- build dir: build/bin
- run_comparison.py cache key includes transports/sizes/runs/IO threads/buffers/msg_count/no_taskset


## PATTERN: PAIR
  > Benchmarking libzmq for PAIR...
    Testing inproc | 64B: 1 2 3 Done
    Testing inproc | 256B: 1 2 3 Done
    Testing inproc | 1024B: 1 2 3 Done
    Testing inproc | 65536B: 1 2 3 Done
    Testing inproc | 131072B: 1 2 3 Done
    Testing inproc | 262144B: 1 2 3 Done
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 256B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 65536B: 1 2 3 Done
    Testing ipc | 131072B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for PAIR...
    Testing inproc | 64B: 1 2 3 Done
    Testing inproc | 256B: 1 2 3 Done
    Testing inproc | 1024B: 1 2 3 Done
    Testing inproc | 65536B: 1 2 3 Done
    Testing inproc | 131072B: 1 2 3 Done
    Testing inproc | 262144B: 1 2 3 Done
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 256B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 65536B: 1 2 3 Done
    Testing ipc | 131072B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: inproc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   6.16 M/s |   6.05 M/s |   -1.85% |
| | Latency |     0.07 us |     0.07 us |   +0.00% (inv) |
| 256B | Throughput |   5.29 M/s |   5.42 M/s |   +2.36% |
| | Latency |     0.08 us |     0.08 us |   +0.00% (inv) |
| 1024B | Throughput |   2.07 M/s |   2.22 M/s |   +7.39% |
| | Latency |     0.10 us |     0.10 us |   +0.00% (inv) |
| 65536B | Throughput |   0.15 M/s |   0.14 M/s |   -6.35% |
| | Latency |     1.96 us |     1.92 us |   +2.04% (inv) |
| 131072B | Throughput |   0.08 M/s |   0.08 M/s |   +0.36% |
| | Latency |     3.61 us |     3.54 us |   +1.94% (inv) |
| 262144B | Throughput |   0.05 M/s |   0.05 M/s |   -5.78% |
| | Latency |     7.45 us |     6.84 us |   +8.19% (inv) |

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.74 M/s |   6.04 M/s |   +5.15% |
| | Latency |    46.36 us |    31.51 us |  +32.03% (inv) |
| 256B | Throughput |   3.23 M/s |   2.52 M/s |  -21.88% |
| | Latency |    46.46 us |    31.38 us |  +32.46% (inv) |
| 1024B | Throughput |   1.27 M/s |   1.02 M/s |  -19.86% |
| | Latency |    47.30 us |    39.35 us |  +16.81% (inv) |
| 65536B | Throughput |   0.08 M/s |   0.07 M/s |  -17.77% |
| | Latency |    82.74 us |    55.40 us |  +33.04% (inv) |
| 131072B | Throughput |   0.05 M/s |   0.04 M/s |  -20.35% |
| | Latency |   141.07 us |   139.25 us |   +1.29% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -21.35% |
| | Latency |   179.42 us |   212.93 us |  -18.68% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.19 M/s |   6.04 M/s |  +16.38% |
| | Latency |    46.61 us |    28.00 us |  +39.93% (inv) |
| 256B | Throughput |   2.87 M/s |   2.69 M/s |   -6.36% |
| | Latency |    44.68 us |    28.46 us |  +36.30% (inv) |
| 1024B | Throughput |   1.05 M/s |   1.11 M/s |   +5.90% |
| | Latency |    46.81 us |    27.66 us |  +40.91% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.07 M/s | +146.83% |
| | Latency |   111.23 us |    82.02 us |  +26.26% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.04 M/s |  +68.51% |
| | Latency |    79.03 us |   117.05 us |  -48.11% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |  +15.83% |
| | Latency |   113.34 us |   157.66 us |  -39.10% (inv) |

## PATTERN: PUBSUB
  > Benchmarking libzmq for PUBSUB...
    Testing inproc | 64B: 1 2 3 Done
    Testing inproc | 256B: 1 2 3 Done
    Testing inproc | 1024B: 1 2 3 Done
    Testing inproc | 65536B: 1 2 3 Done
    Testing inproc | 131072B: 1 2 3 Done
    Testing inproc | 262144B: 1 2 3 Done
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 256B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 65536B: 1 2 3 Done
    Testing ipc | 131072B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for PUBSUB...
    Testing inproc | 64B: 1 2 3 Done
    Testing inproc | 256B: 1 2 3 Done
    Testing inproc | 1024B: 1 2 3 Done
    Testing inproc | 65536B: 1 2 3 Done
    Testing inproc | 131072B: 1 2 3 Done
    Testing inproc | 262144B: 1 2 3 Done
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 256B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 65536B: 1 2 3 Done
    Testing ipc | 131072B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: inproc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.38 M/s |   5.38 M/s |   -0.09% |
| | Latency |     0.19 us |     0.19 us |   +0.00% (inv) |
| 256B | Throughput |   4.76 M/s |   4.29 M/s |   -9.79% |
| | Latency |     0.21 us |     0.23 us |   -9.52% (inv) |
| 1024B | Throughput |   2.04 M/s |   2.01 M/s |   -1.23% |
| | Latency |     0.49 us |     0.50 us |   -2.04% (inv) |
| 65536B | Throughput |   0.14 M/s |   0.15 M/s |   +7.26% |
| | Latency |     7.16 us |     6.67 us |   +6.84% (inv) |
| 131072B | Throughput |   0.09 M/s |   0.09 M/s |   -1.83% |
| | Latency |    11.36 us |    11.57 us |   -1.85% (inv) |
| 262144B | Throughput |   0.06 M/s |   0.05 M/s |  -11.78% |
| | Latency |    18.03 us |    20.43 us |  -13.31% (inv) |

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.09 M/s |   5.61 M/s |  +10.24% |
| | Latency |     0.20 us |     0.18 us |  +10.00% (inv) |
| 256B | Throughput |   3.10 M/s |   2.53 M/s |  -18.43% |
| | Latency |     0.32 us |     0.39 us |  -21.88% (inv) |
| 1024B | Throughput |   1.18 M/s |   1.02 M/s |  -13.79% |
| | Latency |     0.84 us |     0.97 us |  -15.48% (inv) |
| 65536B | Throughput |   0.08 M/s |   0.07 M/s |  -18.96% |
| | Latency |    12.35 us |    15.24 us |  -23.40% (inv) |
| 131072B | Throughput |   0.05 M/s |   0.04 M/s |  -22.34% |
| | Latency |    20.02 us |    25.78 us |  -28.77% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -20.35% |
| | Latency |    37.19 us |    46.69 us |  -25.54% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.02 M/s |   5.60 M/s |  +11.40% |
| | Latency |     0.20 us |     0.18 us |  +10.00% (inv) |
| 256B | Throughput |   2.95 M/s |   2.58 M/s |  -12.34% |
| | Latency |     0.34 us |     0.39 us |  -14.71% (inv) |
| 1024B | Throughput |   1.05 M/s |   1.08 M/s |   +2.92% |
| | Latency |     0.96 us |     0.93 us |   +3.12% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.06 M/s | +128.90% |
| | Latency |    36.51 us |    15.95 us |  +56.31% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.04 M/s |  +60.15% |
| | Latency |    43.01 us |    26.86 us |  +37.55% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |  +40.38% |
| | Latency |    65.84 us |    46.90 us |  +28.77% (inv) |

## PATTERN: DEALER_DEALER
  > Benchmarking libzmq for DEALER_DEALER...
    Testing inproc | 64B: 1 2 3 Done
    Testing inproc | 256B: 1 2 3 Done
    Testing inproc | 1024B: 1 2 3 Done
    Testing inproc | 65536B: 1 2 3 Done
    Testing inproc | 131072B: 1 2 3 Done
    Testing inproc | 262144B: 1 2 3 Done
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 256B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 65536B: 1 2 3 Done
    Testing ipc | 131072B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for DEALER_DEALER...
    Testing inproc | 64B: 1 2 3 Done
    Testing inproc | 256B: 1 2 3 Done
    Testing inproc | 1024B: 1 2 3 Done
    Testing inproc | 65536B: 1 2 3 Done
    Testing inproc | 131072B: 1 2 3 Done
    Testing inproc | 262144B: 1 2 3 Done
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 256B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 65536B: 1 2 3 Done
    Testing ipc | 131072B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: inproc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   6.06 M/s |   5.88 M/s |   -2.92% |
| | Latency |     0.08 us |     0.08 us |   +0.00% (inv) |
| 256B | Throughput |   4.88 M/s |   5.32 M/s |   +9.17% |
| | Latency |     0.08 us |     0.08 us |   +0.00% (inv) |
| 1024B | Throughput |   2.24 M/s |   2.13 M/s |   -4.60% |
| | Latency |     0.10 us |     0.10 us |   +0.00% (inv) |
| 65536B | Throughput |   0.16 M/s |   0.15 M/s |   -5.21% |
| | Latency |     1.95 us |     1.95 us |   +0.00% (inv) |
| 131072B | Throughput |   0.08 M/s |   0.09 M/s |   +1.76% |
| | Latency |     3.61 us |     3.58 us |   +0.83% (inv) |
| 262144B | Throughput |   0.05 M/s |   0.08 M/s |  +72.03% |
| | Latency |     7.01 us |     7.35 us |   -4.85% (inv) |

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.86 M/s |   5.92 M/s |   +1.06% |
| | Latency |    47.52 us |    36.22 us |  +23.78% (inv) |
| 256B | Throughput |   3.17 M/s |   2.56 M/s |  -19.10% |
| | Latency |    47.53 us |    34.69 us |  +27.01% (inv) |
| 1024B | Throughput |   1.21 M/s |   1.02 M/s |  -15.90% |
| | Latency |    46.35 us |    30.92 us |  +33.29% (inv) |
| 65536B | Throughput |   0.08 M/s |   0.06 M/s |  -20.78% |
| | Latency |    89.62 us |    98.22 us |   -9.60% (inv) |
| 131072B | Throughput |   0.05 M/s |   0.04 M/s |  -21.01% |
| | Latency |   196.11 us |   135.60 us |  +30.86% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -18.29% |
| | Latency |   257.63 us |   120.65 us |  +53.17% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.10 M/s |   5.77 M/s |  +13.10% |
| | Latency |    46.33 us |    33.31 us |  +28.10% (inv) |
| 256B | Throughput |   2.84 M/s |   2.69 M/s |   -5.40% |
| | Latency |    46.90 us |    29.49 us |  +37.12% (inv) |
| 1024B | Throughput |   1.06 M/s |   1.06 M/s |   +0.44% |
| | Latency |    47.25 us |    37.35 us |  +20.95% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.06 M/s | +101.91% |
| | Latency |   106.74 us |    52.55 us |  +50.77% (inv) |
| 131072B | Throughput |   0.03 M/s |   0.04 M/s |  +47.22% |
| | Latency |   121.85 us |    80.77 us |  +33.71% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |  +14.84% |
| | Latency |   151.73 us |   103.93 us |  +31.50% (inv) |

## PATTERN: DEALER_ROUTER
  > Benchmarking libzmq for DEALER_ROUTER...
    Testing inproc | 64B: 1 2 3 Done
    Testing inproc | 256B: 1 2 3 Done
    Testing inproc | 1024B: 1 2 3 Done
    Testing inproc | 65536B: 1 2 3 Done
    Testing inproc | 131072B: 1 2 3 Done
    Testing inproc | 262144B: 1 2 3 Done
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 256B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 65536B: 1 2 3 Done
    Testing ipc | 131072B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for DEALER_ROUTER...
    Testing inproc | 64B: 1 2 3 Done
    Testing inproc | 256B: 1 2 3 Done
    Testing inproc | 1024B: 1 2 3 Done
    Testing inproc | 65536B: 1 2 3 Done
    Testing inproc | 131072B: 1 2 3 Done
    Testing inproc | 262144B: 1 2 3 Done
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 256B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 65536B: 1 2 3 Done
    Testing ipc | 131072B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: inproc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.87 M/s |   4.65 M/s |   -4.54% |
| | Latency |     0.11 us |     0.11 us |   +0.00% (inv) |
| 256B | Throughput |   3.66 M/s |   3.73 M/s |   +1.77% |
| | Latency |     0.12 us |     0.11 us |   +8.33% (inv) |
| 1024B | Throughput |   1.81 M/s |   1.83 M/s |   +0.79% |
| | Latency |     0.12 us |     0.13 us |   -8.33% (inv) |
| 65536B | Throughput |   0.14 M/s |   0.14 M/s |   -1.01% |
| | Latency |     1.93 us |     1.85 us |   +4.15% (inv) |
| 131072B | Throughput |   0.08 M/s |   0.08 M/s |   +1.05% |
| | Latency |     3.68 us |     3.52 us |   +4.35% (inv) |
| 262144B | Throughput |   0.05 M/s |   0.05 M/s |   -6.12% |
| | Latency |     6.82 us |     7.20 us |   -5.57% (inv) |

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.54 M/s |   5.20 M/s |   -6.11% |
| | Latency |    79.04 us |    41.28 us |  +47.77% (inv) |
| 256B | Throughput |   3.06 M/s |   2.56 M/s |  -16.48% |
| | Latency |    46.91 us |    41.47 us |  +11.60% (inv) |
| 1024B | Throughput |   1.23 M/s |   1.04 M/s |  -15.56% |
| | Latency |    73.27 us |    56.49 us |  +22.90% (inv) |
| 65536B | Throughput |   0.08 M/s |   0.07 M/s |  -14.01% |
| | Latency |   147.88 us |   104.58 us |  +29.28% (inv) |
| 131072B | Throughput |   0.05 M/s |   0.04 M/s |  -22.64% |
| | Latency |    76.39 us |   136.77 us |  -79.04% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -19.63% |
| | Latency |   161.96 us |   159.54 us |   +1.49% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.87 M/s |   5.50 M/s |  +13.09% |
| | Latency |   101.68 us |    29.90 us |  +70.59% (inv) |
| 256B | Throughput |   2.93 M/s |   2.53 M/s |  -13.49% |
| | Latency |    91.65 us |    44.61 us |  +51.33% (inv) |
| 1024B | Throughput |   1.08 M/s |   0.96 M/s |  -10.55% |
| | Latency |    68.40 us |    37.56 us |  +45.09% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.06 M/s | +155.13% |
| | Latency |    79.43 us |    54.04 us |  +31.97% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.04 M/s |  +66.83% |
| | Latency |    79.35 us |    79.24 us |   +0.14% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |  +27.29% |
| | Latency |   126.75 us |    95.78 us |  +24.43% (inv) |

## PATTERN: ROUTER_ROUTER
  > Benchmarking libzmq for ROUTER_ROUTER...
    Testing inproc | 64B: 1 2 3 Done
    Testing inproc | 256B: 1 2 3 Done
    Testing inproc | 1024B: 1 2 3 Done
    Testing inproc | 65536B: 1 2 3 Done
    Testing inproc | 131072B: 1 2 3 Done
    Testing inproc | 262144B: 1 2 3 Done
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 256B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 65536B: 1 2 3 Done
    Testing ipc | 131072B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for ROUTER_ROUTER...
    Testing inproc | 64B: 1 2 3 Done
    Testing inproc | 256B: 1 2 3 Done
    Testing inproc | 1024B: 1 2 3 Done
    Testing inproc | 65536B: 1 2 3 Done
    Testing inproc | 131072B: 1 2 3 Done
    Testing inproc | 262144B: 1 2 3 Done
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 256B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 65536B: 1 2 3 Done
    Testing ipc | 131072B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: inproc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.29 M/s |   4.60 M/s |   +7.38% |
| | Latency |     0.15 us |     0.17 us |  -13.33% (inv) |
| 256B | Throughput |   3.22 M/s |   3.15 M/s |   -2.07% |
| | Latency |     0.16 us |     0.17 us |   -6.25% (inv) |
| 1024B | Throughput |   1.79 M/s |   1.78 M/s |   -0.60% |
| | Latency |     0.19 us |     0.18 us |   +5.26% (inv) |
| 65536B | Throughput |   0.18 M/s |   0.16 M/s |   -9.95% |
| | Latency |     2.02 us |     2.04 us |   -0.99% (inv) |
| 131072B | Throughput |   0.09 M/s |   0.09 M/s |   +5.56% |
| | Latency |     3.75 us |     3.71 us |   +1.07% (inv) |
| 262144B | Throughput |   0.06 M/s |   0.05 M/s |  -15.97% |
| | Latency |     7.05 us |     6.98 us |   +0.99% (inv) |

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.79 M/s |   5.29 M/s |  +10.57% |
| | Latency |    24.89 us |    17.41 us |  +30.05% (inv) |
| 256B | Throughput |   2.75 M/s |   2.27 M/s |  -17.52% |
| | Latency |    23.83 us |    19.70 us |  +17.33% (inv) |
| 1024B | Throughput |   1.20 M/s |   0.95 M/s |  -20.46% |
| | Latency |    22.58 us |    20.47 us |   +9.34% (inv) |
| 65536B | Throughput |   0.08 M/s |   0.07 M/s |  -11.10% |
| | Latency |    39.74 us |    34.03 us |  +14.37% (inv) |
| 131072B | Throughput |   0.05 M/s |   0.04 M/s |  -17.21% |
| | Latency |    45.61 us |    51.66 us |  -13.26% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -19.60% |
| | Latency |   106.15 us |    93.17 us |  +12.23% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.48 M/s |   5.37 M/s |  +19.98% |
| | Latency |    24.38 us |    17.08 us |  +29.94% (inv) |
| 256B | Throughput |   2.58 M/s |   2.37 M/s |   -7.90% |
| | Latency |    25.36 us |    15.19 us |  +40.10% (inv) |
| 1024B | Throughput |   1.03 M/s |   1.10 M/s |   +7.44% |
| | Latency |    28.06 us |    15.74 us |  +43.91% (inv) |
| 65536B | Throughput |   0.02 M/s |   0.07 M/s | +200.59% |
| | Latency |    37.87 us |    29.50 us |  +22.10% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.04 M/s |  +84.28% |
| | Latency |    56.95 us |    43.78 us |  +23.13% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |  +20.59% |
| | Latency |    75.55 us |   116.10 us |  -53.67% (inv) |

## PATTERN: ROUTER_ROUTER_POLL
  > Benchmarking libzmq for ROUTER_ROUTER_POLL...
    Testing inproc | 64B: 1 2 3 Done
    Testing inproc | 256B: 1 2 3 Done
    Testing inproc | 1024B: 1 2 3 Done
    Testing inproc | 65536B: 1 2 3 Done
    Testing inproc | 131072B: 1 2 3 Done
    Testing inproc | 262144B: 1 2 3 Done
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 256B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 65536B: 1 2 3 Done
    Testing ipc | 131072B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for ROUTER_ROUTER_POLL...
    Testing inproc | 64B: 1 2 3 Done
    Testing inproc | 256B: 1 2 3 Done
    Testing inproc | 1024B: 1 2 3 Done
    Testing inproc | 65536B: 1 2 3 Done
    Testing inproc | 131072B: 1 2 3 Done
    Testing inproc | 262144B: 1 2 3 Done
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 256B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 65536B: 1 2 3 Done
    Testing ipc | 131072B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: inproc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.11 M/s |   3.75 M/s |   -8.89% |
| | Latency |     0.54 us |     0.48 us |  +11.11% (inv) |
| 256B | Throughput |   3.23 M/s |   3.13 M/s |   -3.23% |
| | Latency |     0.53 us |     0.47 us |  +11.32% (inv) |
| 1024B | Throughput |   1.78 M/s |   1.75 M/s |   -1.90% |
| | Latency |     0.55 us |     0.49 us |  +10.91% (inv) |
| 65536B | Throughput |   0.18 M/s |   0.19 M/s |   +9.92% |
| | Latency |     2.42 us |     2.42 us |   +0.00% (inv) |
| 131072B | Throughput |   0.09 M/s |   0.08 M/s |  -12.15% |
| | Latency |     4.18 us |     4.11 us |   +1.67% (inv) |
| 262144B | Throughput |   0.06 M/s |   0.05 M/s |  -14.94% |
| | Latency |     7.57 us |     7.36 us |   +2.77% (inv) |

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.80 M/s |   5.44 M/s |  +13.31% |
| | Latency |    24.65 us |    16.63 us |  +32.54% (inv) |
| 256B | Throughput |   2.80 M/s |   2.35 M/s |  -16.07% |
| | Latency |    23.26 us |    16.65 us |  +28.42% (inv) |
| 1024B | Throughput |   1.16 M/s |   0.98 M/s |  -15.48% |
| | Latency |    26.40 us |    16.63 us |  +37.01% (inv) |
| 65536B | Throughput |   0.07 M/s |   0.07 M/s |   -4.04% |
| | Latency |    38.53 us |    33.48 us |  +13.11% (inv) |
| 131072B | Throughput |   0.05 M/s |   0.04 M/s |  -16.82% |
| | Latency |    51.36 us |    47.98 us |   +6.58% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -18.26% |
| | Latency |   112.43 us |   149.80 us |  -33.24% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.41 M/s |   5.53 M/s |  +25.38% |
| | Latency |    23.39 us |    15.57 us |  +33.43% (inv) |
| 256B | Throughput |   2.64 M/s |   2.40 M/s |   -9.16% |
| | Latency |    23.64 us |    15.42 us |  +34.77% (inv) |
| 1024B | Throughput |   1.05 M/s |   1.09 M/s |   +4.00% |
| | Latency |    24.13 us |    15.85 us |  +34.31% (inv) |
| 65536B | Throughput |   0.02 M/s |   0.07 M/s | +192.93% |
| | Latency |    42.34 us |    29.89 us |  +29.40% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.04 M/s |  +83.23% |
| | Latency |    44.62 us |    99.45 us | -122.88% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |  +34.58% |
| | Latency |    89.19 us |   100.27 us |  -12.42% (inv) |
