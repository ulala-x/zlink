# IO Threads Baseline (BENCH_IO_THREADS=2)

## Setup

- BENCH_IO_THREADS=2
- BENCH_TRANSPORTS=inproc,tcp,ipc
- BENCH_MSG_SIZES=64,256,1024,65536,131072,262144
- runs=3, refresh libzmq baseline
- build dir: build/bin


## PATTERN: PAIR


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
| 64B | Throughput |   7.34 M/s |   7.78 M/s |   +6.06% |
| | Latency |     0.07 us |     0.07 us |   +0.00% (inv) |
| 256B | Throughput |   5.59 M/s |   6.40 M/s |  +14.58% |
| | Latency |     0.08 us |     0.07 us |  +12.50% (inv) |
| 1024B | Throughput |   2.96 M/s |   3.34 M/s |  +12.63% |
| | Latency |     0.09 us |     0.09 us |   +0.00% (inv) |
| 65536B | Throughput |   0.16 M/s |   0.17 M/s |   +5.53% |
| | Latency |     1.88 us |     2.03 us |   -7.98% (inv) |
| 131072B | Throughput |   0.11 M/s |   0.11 M/s |   -0.54% |
| | Latency |     3.57 us |     3.54 us |   +0.84% (inv) |
| 262144B | Throughput |   0.06 M/s |   0.07 M/s |   +7.98% |
| | Latency |     7.24 us |     6.80 us |   +6.08% (inv) |

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.39 M/s |   4.62 M/s |   +5.35% |
| | Latency |     5.98 us |     5.18 us |  +13.38% (inv) |
| 256B | Throughput |   0.75 M/s |   2.73 M/s | +266.07% |
| | Latency |     6.23 us |     5.14 us |  +17.50% (inv) |
| 1024B | Throughput |   0.53 M/s |   0.97 M/s |  +82.25% |
| | Latency |     6.06 us |     5.26 us |  +13.20% (inv) |
| 65536B | Throughput |   0.02 M/s |   0.03 M/s |  +23.70% |
| | Latency |    13.75 us |    13.22 us |   +3.85% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |   +7.71% |
| | Latency |    20.61 us |    20.56 us |   +0.24% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  +37.40% |
| | Latency |    32.23 us |    37.57 us |  -16.57% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.99 M/s |   4.92 M/s |   -1.43% |
| | Latency |     5.50 us |     4.39 us |  +20.18% (inv) |
| 256B | Throughput |   2.98 M/s |   2.91 M/s |   -2.32% |
| | Latency |     5.41 us |     4.47 us |  +17.38% (inv) |
| 1024B | Throughput |   1.19 M/s |   1.06 M/s |  -10.59% |
| | Latency |     5.43 us |     4.64 us |  +14.55% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.03 M/s |   +4.42% |
| | Latency |    13.89 us |    12.62 us |   +9.14% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |  -33.90% |
| | Latency |    19.41 us |    21.28 us |   -9.63% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  +15.86% |
| | Latency |    33.84 us |    36.23 us |   -7.06% (inv) |

## PATTERN: PUBSUB


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
| 64B | Throughput |   5.81 M/s |   6.14 M/s |   +5.76% |
| | Latency |     0.17 us |     0.16 us |   +5.88% (inv) |
| 256B | Throughput |   5.51 M/s |   5.78 M/s |   +4.92% |
| | Latency |     0.18 us |     0.17 us |   +5.56% (inv) |
| 1024B | Throughput |   2.96 M/s |   3.14 M/s |   +6.27% |
| | Latency |     0.34 us |     0.32 us |   +5.88% (inv) |
| 65536B | Throughput |   0.15 M/s |   0.18 M/s |  +15.92% |
| | Latency |     6.57 us |     5.67 us |  +13.70% (inv) |
| 131072B | Throughput |   0.10 M/s |   0.12 M/s |  +12.05% |
| | Latency |     9.53 us |     8.50 us |  +10.81% (inv) |
| 262144B | Throughput |   0.06 M/s |   0.07 M/s |   +7.91% |
| | Latency |    15.64 us |    14.49 us |   +7.35% (inv) |

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   3.81 M/s |   4.47 M/s |  +17.25% |
| | Latency |     0.26 us |     0.22 us |  +15.38% (inv) |
| 256B | Throughput |   0.69 M/s |   2.63 M/s | +281.56% |
| | Latency |     1.45 us |     0.38 us |  +73.79% (inv) |
| 1024B | Throughput |   0.46 M/s |   0.98 M/s | +114.58% |
| | Latency |     2.18 us |     1.02 us |  +53.21% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.03 M/s |  +26.40% |
| | Latency |    38.59 us |    30.53 us |  +20.89% (inv) |
| 131072B | Throughput |   0.01 M/s |   0.02 M/s |  +47.45% |
| | Latency |    79.00 us |    53.58 us |  +32.18% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  +25.97% |
| | Latency |   124.84 us |    99.10 us |  +20.62% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.72 M/s |   4.45 M/s |   -5.73% |
| | Latency |     0.21 us |     0.22 us |   -4.76% (inv) |
| 256B | Throughput |   2.91 M/s |   2.82 M/s |   -2.96% |
| | Latency |     0.34 us |     0.35 us |   -2.94% (inv) |
| 1024B | Throughput |   1.14 M/s |   1.06 M/s |   -6.86% |
| | Latency |     0.88 us |     0.95 us |   -7.95% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.04 M/s |   +8.46% |
| | Latency |    30.69 us |    28.30 us |   +7.79% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |  -12.49% |
| | Latency |    43.16 us |    49.32 us |  -14.27% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  +13.43% |
| | Latency |   120.20 us |   105.97 us |  +11.84% (inv) |

## PATTERN: DEALER_DEALER


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
| 64B | Throughput |   6.90 M/s |   6.28 M/s |   -8.93% |
| | Latency |     0.09 us |     0.07 us |  +22.22% (inv) |
| 256B | Throughput |   6.02 M/s |   6.31 M/s |   +4.77% |
| | Latency |     0.08 us |     0.10 us |  -25.00% (inv) |
| 1024B | Throughput |   3.22 M/s |   3.30 M/s |   +2.47% |
| | Latency |     0.10 us |     0.11 us |  -10.00% (inv) |
| 65536B | Throughput |   0.16 M/s |   0.18 M/s |   +8.97% |
| | Latency |     2.06 us |     2.04 us |   +0.97% (inv) |
| 131072B | Throughput |   0.11 M/s |   0.12 M/s |   +4.65% |
| | Latency |     3.53 us |     3.54 us |   -0.28% (inv) |
| 262144B | Throughput |   0.07 M/s |   0.07 M/s |   -0.85% |
| | Latency |     6.91 us |     7.08 us |   -2.46% (inv) |

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.12 M/s |   4.66 M/s |  +13.26% |
| | Latency |     5.91 us |     5.02 us |  +15.06% (inv) |
| 256B | Throughput |   2.67 M/s |   2.72 M/s |   +1.80% |
| | Latency |     5.82 us |     5.07 us |  +12.89% (inv) |
| 1024B | Throughput |   0.49 M/s |   0.99 M/s | +103.32% |
| | Latency |     5.88 us |     5.22 us |  +11.22% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.03 M/s |  +22.16% |
| | Latency |    13.56 us |    13.06 us |   +3.69% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |  +13.18% |
| | Latency |    19.53 us |    21.36 us |   -9.37% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  +29.57% |
| | Latency |    31.28 us |    36.80 us |  -17.65% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.15 M/s |   4.81 M/s |   -6.59% |
| | Latency |     5.50 us |     4.37 us |  +20.55% (inv) |
| 256B | Throughput |   3.01 M/s |   2.92 M/s |   -2.95% |
| | Latency |     5.55 us |     4.39 us |  +20.90% (inv) |
| 1024B | Throughput |   1.16 M/s |   1.10 M/s |   -4.83% |
| | Latency |     5.64 us |     4.57 us |  +18.97% (inv) |
| 65536B | Throughput |   0.04 M/s |   0.04 M/s |   +0.78% |
| | Latency |    13.71 us |    12.62 us |   +7.95% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |  -21.56% |
| | Latency |    19.28 us |    20.51 us |   -6.38% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  +24.08% |
| | Latency |    32.79 us |    36.73 us |  -12.02% (inv) |

## PATTERN: DEALER_ROUTER


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
| 64B | Throughput |   6.82 M/s |   6.65 M/s |   -2.54% |
| | Latency |     0.10 us |     0.10 us |   +0.00% (inv) |
| 256B | Throughput |   5.55 M/s |   5.77 M/s |   +3.93% |
| | Latency |     0.11 us |     0.11 us |   +0.00% (inv) |
| 1024B | Throughput |   3.30 M/s |   2.99 M/s |   -9.34% |
| | Latency |     0.12 us |     0.13 us |   -8.33% (inv) |
| 65536B | Throughput |   0.17 M/s |   0.17 M/s |   +5.60% |
| | Latency |     1.81 us |     1.82 us |   -0.55% (inv) |
| 131072B | Throughput |   0.11 M/s |   0.12 M/s |   +5.10% |
| | Latency |     3.44 us |     3.36 us |   +2.33% (inv) |
| 262144B | Throughput |   0.07 M/s |   0.07 M/s |   +1.09% |
| | Latency |     6.58 us |     6.69 us |   -1.67% (inv) |

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   3.94 M/s |   3.92 M/s |   -0.61% |
| | Latency |     5.91 us |     4.93 us |  +16.58% (inv) |
| 256B | Throughput |   2.44 M/s |   2.53 M/s |   +3.80% |
| | Latency |     5.91 us |     5.04 us |  +14.72% (inv) |
| 1024B | Throughput |   0.48 M/s |   0.96 M/s |  +98.29% |
| | Latency |     5.98 us |     5.11 us |  +14.55% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.04 M/s |  +30.00% |
| | Latency |    13.77 us |    13.02 us |   +5.45% (inv) |
| 131072B | Throughput |   0.01 M/s |   0.02 M/s |  +29.56% |
| | Latency |    19.54 us |    21.40 us |   -9.52% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  +22.03% |
| | Latency |    30.82 us |    38.63 us |  -25.34% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.18 M/s |   4.03 M/s |   -3.43% |
| | Latency |     5.37 us |     4.32 us |  +19.55% (inv) |
| 256B | Throughput |   2.73 M/s |   2.63 M/s |   -3.74% |
| | Latency |     5.41 us |     4.35 us |  +19.59% (inv) |
| 1024B | Throughput |   1.11 M/s |   1.05 M/s |   -5.45% |
| | Latency |     5.43 us |     4.55 us |  +16.21% (inv) |
| 65536B | Throughput |   0.04 M/s |   0.03 M/s |  -15.55% |
| | Latency |    13.63 us |    12.58 us |   +7.70% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |  -19.62% |
| | Latency |    19.55 us |    20.50 us |   -4.86% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  +21.45% |
| | Latency |    32.78 us |    35.61 us |   -8.63% (inv) |

## PATTERN: ROUTER_ROUTER


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
| 64B | Throughput |   5.62 M/s |   6.30 M/s |  +12.00% |
| | Latency |     0.17 us |     0.17 us |   +0.00% (inv) |
| 256B | Throughput |   4.68 M/s |   4.57 M/s |   -2.38% |
| | Latency |     0.16 us |     0.16 us |   +0.00% (inv) |
| 1024B | Throughput |   2.79 M/s |   3.02 M/s |   +8.35% |
| | Latency |     0.18 us |     0.19 us |   -5.56% (inv) |
| 65536B | Throughput |   0.15 M/s |   0.17 M/s |  +13.53% |
| | Latency |     2.00 us |     1.99 us |   +0.50% (inv) |
| 131072B | Throughput |   0.11 M/s |   0.11 M/s |   -0.34% |
| | Latency |     3.70 us |     3.76 us |   -1.62% (inv) |
| 262144B | Throughput |   0.07 M/s |   0.07 M/s |   -1.58% |
| | Latency |     6.83 us |     6.89 us |   -0.88% (inv) |

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   3.23 M/s |   3.51 M/s |   +8.47% |
| | Latency |     5.22 us |     5.13 us |   +1.72% (inv) |
| 256B | Throughput |   2.33 M/s |   2.32 M/s |   -0.36% |
| | Latency |     5.15 us |     5.20 us |   -0.97% (inv) |
| 1024B | Throughput |   0.48 M/s |   0.94 M/s |  +95.80% |
| | Latency |     5.31 us |     5.43 us |   -2.26% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.04 M/s |  +24.95% |
| | Latency |    12.65 us |    13.39 us |   -5.85% (inv) |
| 131072B | Throughput |   0.01 M/s |   0.02 M/s |  +46.22% |
| | Latency |    20.73 us |    19.98 us |   +3.62% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  +52.85% |
| | Latency |    36.05 us |    43.21 us |  -19.86% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   3.75 M/s |   3.71 M/s |   -1.08% |
| | Latency |     4.72 us |     4.48 us |   +5.08% (inv) |
| 256B | Throughput |   2.48 M/s |   2.46 M/s |   -0.80% |
| | Latency |     4.92 us |     4.54 us |   +7.72% (inv) |
| 1024B | Throughput |   1.07 M/s |   1.02 M/s |   -4.41% |
| | Latency |     4.77 us |     4.67 us |   +2.10% (inv) |
| 65536B | Throughput |   0.04 M/s |   0.03 M/s |   -5.49% |
| | Latency |    11.85 us |    12.88 us |   -8.69% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |  -17.38% |
| | Latency |    24.01 us |    20.86 us |  +13.12% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  +24.28% |
| | Latency |    37.91 us |    36.06 us |   +4.88% (inv) |

## PATTERN: ROUTER_ROUTER_POLL


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
| 64B | Throughput |   5.66 M/s |   5.52 M/s |   -2.48% |
| | Latency |     0.52 us |     0.47 us |   +9.62% (inv) |
| 256B | Throughput |   4.64 M/s |   4.61 M/s |   -0.64% |
| | Latency |     0.51 us |     0.47 us |   +7.84% (inv) |
| 1024B | Throughput |   2.94 M/s |   2.96 M/s |   +0.58% |
| | Latency |     0.54 us |     0.51 us |   +5.56% (inv) |
| 65536B | Throughput |   0.13 M/s |   0.15 M/s |  +12.47% |
| | Latency |     2.37 us |     2.38 us |   -0.42% (inv) |
| 131072B | Throughput |   0.09 M/s |   0.10 M/s |   +6.66% |
| | Latency |     4.06 us |     4.16 us |   -2.46% (inv) |
| 262144B | Throughput |   0.06 M/s |   0.07 M/s |  +10.77% |
| | Latency |     7.39 us |     7.03 us |   +4.87% (inv) |

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   3.35 M/s |   3.70 M/s |  +10.45% |
| | Latency |     5.79 us |     4.07 us |  +29.71% (inv) |
| 256B | Throughput |   2.21 M/s |   2.33 M/s |   +5.17% |
| | Latency |     5.77 us |     4.22 us |  +26.86% (inv) |
| 1024B | Throughput |   0.90 M/s |   0.93 M/s |   +3.19% |
| | Latency |     5.82 us |     4.34 us |  +25.43% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.03 M/s |  +27.11% |
| | Latency |    13.20 us |    12.48 us |   +5.45% (inv) |
| 131072B | Throughput |   0.01 M/s |   0.02 M/s |  +36.40% |
| | Latency |    19.19 us |    21.71 us |  -13.13% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  +32.58% |
| | Latency |    35.89 us |    37.26 us |   -3.82% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   3.90 M/s |   3.65 M/s |   -6.32% |
| | Latency |     5.07 us |     4.83 us |   +4.73% (inv) |
| 256B | Throughput |   2.51 M/s |   2.46 M/s |   -1.96% |
| | Latency |     5.22 us |     4.86 us |   +6.90% (inv) |
| 1024B | Throughput |   1.06 M/s |   1.00 M/s |   -6.05% |
| | Latency |     5.25 us |     5.02 us |   +4.38% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.03 M/s |   +0.32% |
| | Latency |    13.07 us |    13.30 us |   -1.76% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |  -12.28% |
| | Latency |    19.92 us |    19.96 us |   -0.20% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  +26.07% |
| | Latency |    37.45 us |    36.14 us |   +3.50% (inv) |
