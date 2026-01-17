# IO Threads=2 Full Matrix Rerun

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
| 64B | Throughput |   7.21 M/s |   7.98 M/s |  +10.64% |
| | Latency |     0.07 us |     0.07 us |   +0.00% (inv) |
| 256B | Throughput |   5.75 M/s |   6.31 M/s |   +9.81% |
| | Latency |     0.08 us |     0.07 us |  +12.50% (inv) |
| 1024B | Throughput |   3.10 M/s |   3.20 M/s |   +3.04% |
| | Latency |     0.09 us |     0.09 us |   +0.00% (inv) |
| 65536B | Throughput |   0.16 M/s |   0.17 M/s |   +9.71% |
| | Latency |     1.96 us |     1.95 us |   +0.51% (inv) |
| 131072B | Throughput |   0.11 M/s |   0.12 M/s |   +8.34% |
| | Latency |     3.53 us |     3.53 us |   +0.00% (inv) |
| 262144B | Throughput |   0.05 M/s |   0.07 M/s |  +33.11% |
| | Latency |     7.41 us |     6.78 us |   +8.50% (inv) |

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.28 M/s |   4.80 M/s |  +12.13% |
| | Latency |     6.22 us |     5.14 us |  +17.36% (inv) |
| 256B | Throughput |   0.71 M/s |   2.78 M/s | +289.62% |
| | Latency |     6.06 us |     5.11 us |  +15.68% (inv) |
| 1024B | Throughput |   0.54 M/s |   0.98 M/s |  +81.11% |
| | Latency |     5.98 us |     5.26 us |  +12.04% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.04 M/s |  +33.79% |
| | Latency |    13.95 us |    13.38 us |   +4.09% (inv) |
| 131072B | Throughput |   0.01 M/s |   0.02 M/s |  +37.37% |
| | Latency |    19.91 us |    21.49 us |   -7.94% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  +21.50% |
| | Latency |    32.18 us |    37.78 us |  -17.40% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.32 M/s |   5.01 M/s |   -5.81% |
| | Latency |     5.50 us |     4.47 us |  +18.73% (inv) |
| 256B | Throughput |   3.04 M/s |   2.92 M/s |   -4.09% |
| | Latency |     5.56 us |     4.51 us |  +18.88% (inv) |
| 1024B | Throughput |   1.18 M/s |   1.08 M/s |   -8.72% |
| | Latency |     5.49 us |     4.67 us |  +14.94% (inv) |
| 65536B | Throughput |   0.04 M/s |   0.03 M/s |   -8.63% |
| | Latency |    13.81 us |    12.82 us |   +7.17% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |  -18.35% |
| | Latency |    20.09 us |    20.77 us |   -3.38% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |   +7.31% |
| | Latency |    34.27 us |    38.13 us |  -11.26% (inv) |

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
| 64B | Throughput |   6.22 M/s |   6.92 M/s |  +11.24% |
| | Latency |     0.16 us |     0.14 us |  +12.50% (inv) |
| 256B | Throughput |   4.88 M/s |   5.50 M/s |  +12.53% |
| | Latency |     0.20 us |     0.18 us |  +10.00% (inv) |
| 1024B | Throughput |   2.95 M/s |   3.05 M/s |   +3.61% |
| | Latency |     0.34 us |     0.33 us |   +2.94% (inv) |
| 65536B | Throughput |   0.16 M/s |   0.17 M/s |   +6.16% |
| | Latency |     6.28 us |     5.92 us |   +5.73% (inv) |
| 131072B | Throughput |   0.11 M/s |   0.11 M/s |   +4.05% |
| | Latency |     9.06 us |     8.71 us |   +3.86% (inv) |
| 262144B | Throughput |   0.07 M/s |   0.07 M/s |   +2.28% |
| | Latency |    15.15 us |    14.81 us |   +2.24% (inv) |

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   3.93 M/s |   4.41 M/s |  +12.19% |
| | Latency |     0.25 us |     0.22 us |  +12.00% (inv) |
| 256B | Throughput |   0.72 M/s |   2.54 M/s | +254.10% |
| | Latency |     1.39 us |     0.39 us |  +71.94% (inv) |
| 1024B | Throughput |   0.53 M/s |   0.91 M/s |  +71.02% |
| | Latency |     1.89 us |     1.10 us |  +41.80% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.03 M/s |  +25.63% |
| | Latency |    37.04 us |    29.48 us |  +20.41% (inv) |
| 131072B | Throughput |   0.01 M/s |   0.02 M/s |  +40.33% |
| | Latency |    79.81 us |    56.87 us |  +28.74% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  +18.28% |
| | Latency |   125.17 us |   105.82 us |  +15.46% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.62 M/s |   4.37 M/s |   -5.35% |
| | Latency |     0.22 us |     0.23 us |   -4.55% (inv) |
| 256B | Throughput |   2.98 M/s |   2.87 M/s |   -3.65% |
| | Latency |     0.34 us |     0.35 us |   -2.94% (inv) |
| 1024B | Throughput |   1.09 M/s |   1.08 M/s |   -1.03% |
| | Latency |     0.92 us |     0.93 us |   -1.09% (inv) |
| 65536B | Throughput |   0.04 M/s |   0.04 M/s |   -0.48% |
| | Latency |    28.17 us |    28.31 us |   -0.50% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |  -26.95% |
| | Latency |    42.22 us |    57.79 us |  -36.88% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  +11.40% |
| | Latency |   116.17 us |   104.29 us |  +10.23% (inv) |

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
| 64B | Throughput |   6.57 M/s |   8.02 M/s |  +22.07% |
| | Latency |     0.08 us |     0.08 us |   +0.00% (inv) |
| 256B | Throughput |   5.81 M/s |   6.11 M/s |   +5.15% |
| | Latency |     0.08 us |     0.10 us |  -25.00% (inv) |
| 1024B | Throughput |   2.95 M/s |   2.80 M/s |   -5.28% |
| | Latency |     0.09 us |     0.10 us |  -11.11% (inv) |
| 65536B | Throughput |   0.15 M/s |   0.17 M/s |   +7.50% |
| | Latency |     1.91 us |     2.01 us |   -5.24% (inv) |
| 131072B | Throughput |   0.10 M/s |   0.12 M/s |  +10.86% |
| | Latency |     3.55 us |     3.60 us |   -1.41% (inv) |
| 262144B | Throughput |   0.07 M/s |   0.07 M/s |   +0.45% |
| | Latency |     6.99 us |     7.15 us |   -2.29% (inv) |

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.40 M/s |   4.78 M/s |   +8.63% |
| | Latency |     5.97 us |     5.08 us |  +14.91% (inv) |
| 256B | Throughput |   2.49 M/s |   2.64 M/s |   +5.67% |
| | Latency |     6.10 us |     5.17 us |  +15.25% (inv) |
| 1024B | Throughput |   0.52 M/s |   0.98 M/s |  +88.29% |
| | Latency |     6.05 us |     5.25 us |  +13.22% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.04 M/s |  +25.84% |
| | Latency |    14.44 us |    13.25 us |   +8.24% (inv) |
| 131072B | Throughput |   0.01 M/s |   0.02 M/s |  +36.80% |
| | Latency |    19.94 us |    21.52 us |   -7.92% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  +16.26% |
| | Latency |    32.37 us |    38.14 us |  -17.83% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.03 M/s |   4.67 M/s |   -7.03% |
| | Latency |     5.48 us |     4.47 us |  +18.43% (inv) |
| 256B | Throughput |   3.11 M/s |   2.80 M/s |  -10.09% |
| | Latency |     5.71 us |     4.70 us |  +17.69% (inv) |
| 1024B | Throughput |   1.16 M/s |   1.10 M/s |   -5.91% |
| | Latency |     5.61 us |     4.73 us |  +15.69% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.04 M/s |   +7.18% |
| | Latency |    14.21 us |    12.89 us |   +9.29% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |  -19.49% |
| | Latency |    20.15 us |    20.72 us |   -2.83% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  +24.99% |
| | Latency |    33.35 us |    36.26 us |   -8.73% (inv) |

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
| 64B | Throughput |   6.50 M/s |   6.74 M/s |   +3.66% |
| | Latency |     0.11 us |     0.11 us |   +0.00% (inv) |
| 256B | Throughput |   5.58 M/s |   5.84 M/s |   +4.71% |
| | Latency |     0.11 us |     0.11 us |   +0.00% (inv) |
| 1024B | Throughput |   2.94 M/s |   3.23 M/s |   +9.96% |
| | Latency |     0.13 us |     0.13 us |   +0.00% (inv) |
| 65536B | Throughput |   0.15 M/s |   0.16 M/s |   +4.08% |
| | Latency |     1.91 us |     1.93 us |   -1.05% (inv) |
| 131072B | Throughput |   0.11 M/s |   0.11 M/s |   +3.77% |
| | Latency |     3.55 us |     3.50 us |   +1.41% (inv) |
| 262144B | Throughput |   0.07 M/s |   0.07 M/s |   +0.84% |
| | Latency |     6.79 us |     6.90 us |   -1.62% (inv) |

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   3.70 M/s |   3.86 M/s |   +4.16% |
| | Latency |     6.05 us |     5.14 us |  +15.04% (inv) |
| 256B | Throughput |   2.29 M/s |   2.43 M/s |   +6.01% |
| | Latency |     6.08 us |     5.14 us |  +15.46% (inv) |
| 1024B | Throughput |   0.47 M/s |   0.96 M/s | +104.65% |
| | Latency |     6.20 us |     5.24 us |  +15.48% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.03 M/s |  +26.05% |
| | Latency |    13.88 us |    13.10 us |   +5.62% (inv) |
| 131072B | Throughput |   0.01 M/s |   0.02 M/s |  +32.20% |
| | Latency |    19.91 us |    21.28 us |   -6.88% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |   +8.35% |
| | Latency |    31.33 us |    37.02 us |  -18.16% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.05 M/s |   3.87 M/s |   -4.30% |
| | Latency |     5.49 us |     4.49 us |  +18.21% (inv) |
| 256B | Throughput |   2.79 M/s |   2.54 M/s |   -8.65% |
| | Latency |     5.57 us |     4.59 us |  +17.59% (inv) |
| 1024B | Throughput |   1.05 M/s |   1.02 M/s |   -2.11% |
| | Latency |     5.51 us |     4.61 us |  +16.33% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.03 M/s |   +5.76% |
| | Latency |    13.76 us |    12.93 us |   +6.03% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |  -18.02% |
| | Latency |    21.00 us |    21.18 us |   -0.86% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  +10.14% |
| | Latency |    33.73 us |    36.25 us |   -7.47% (inv) |

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
| 64B | Throughput |   5.55 M/s |   5.62 M/s |   +1.30% |
| | Latency |     0.16 us |     0.17 us |   -6.25% (inv) |
| 256B | Throughput |   4.43 M/s |   5.21 M/s |  +17.58% |
| | Latency |     0.16 us |     0.16 us |   +0.00% (inv) |
| 1024B | Throughput |   2.97 M/s |   3.08 M/s |   +3.70% |
| | Latency |     0.19 us |     0.19 us |   +0.00% (inv) |
| 65536B | Throughput |   0.16 M/s |   0.16 M/s |   +5.78% |
| | Latency |     2.00 us |     2.03 us |   -1.50% (inv) |
| 131072B | Throughput |   0.11 M/s |   0.11 M/s |   +3.68% |
| | Latency |     3.67 us |     3.74 us |   -1.91% (inv) |
| 262144B | Throughput |   0.06 M/s |   0.07 M/s |   +4.64% |
| | Latency |     7.13 us |     7.01 us |   +1.68% (inv) |

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   3.22 M/s |   3.61 M/s |  +11.98% |
| | Latency |     5.28 us |     5.25 us |   +0.57% (inv) |
| 256B | Throughput |   0.72 M/s |   2.37 M/s | +228.85% |
| | Latency |     5.44 us |     5.26 us |   +3.31% (inv) |
| 1024B | Throughput |   0.94 M/s |   0.93 M/s |   -1.46% |
| | Latency |     5.47 us |     5.35 us |   +2.19% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.03 M/s |  +16.75% |
| | Latency |    13.01 us |    13.58 us |   -4.38% (inv) |
| 131072B | Throughput |   0.01 M/s |   0.02 M/s |  +26.09% |
| | Latency |    19.58 us |    21.87 us |  -11.70% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  +35.17% |
| | Latency |    37.02 us |    38.31 us |   -3.48% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   3.81 M/s |   3.66 M/s |   -3.87% |
| | Latency |     4.66 us |     4.58 us |   +1.72% (inv) |
| 256B | Throughput |   2.54 M/s |   2.44 M/s |   -3.78% |
| | Latency |     4.76 us |     4.74 us |   +0.42% (inv) |
| 1024B | Throughput |   1.06 M/s |   1.01 M/s |   -4.92% |
| | Latency |     4.76 us |     4.67 us |   +1.89% (inv) |
| 65536B | Throughput |   0.04 M/s |   0.03 M/s |   -5.50% |
| | Latency |    12.68 us |    13.19 us |   -4.02% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |  -19.47% |
| | Latency |    19.29 us |    21.24 us |  -10.11% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  +15.22% |
| | Latency |    39.67 us |    36.35 us |   +8.37% (inv) |

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
| 64B | Throughput |   5.53 M/s |   5.15 M/s |   -6.83% |
| | Latency |     0.54 us |     0.48 us |  +11.11% (inv) |
| 256B | Throughput |   4.66 M/s |   4.58 M/s |   -1.70% |
| | Latency |     0.54 us |     0.47 us |  +12.96% (inv) |
| 1024B | Throughput |   2.85 M/s |   2.99 M/s |   +5.12% |
| | Latency |     0.55 us |     0.50 us |   +9.09% (inv) |
| 65536B | Throughput |   0.12 M/s |   0.15 M/s |  +21.13% |
| | Latency |     2.41 us |     2.39 us |   +0.83% (inv) |
| 131072B | Throughput |   0.09 M/s |   0.10 M/s |   +7.47% |
| | Latency |     4.15 us |     4.14 us |   +0.24% (inv) |
| 262144B | Throughput |   0.06 M/s |   0.06 M/s |   +7.19% |
| | Latency |     7.55 us |     7.36 us |   +2.52% (inv) |

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   3.53 M/s |   3.57 M/s |   +0.98% |
| | Latency |     5.86 us |     5.31 us |   +9.39% (inv) |
| 256B | Throughput |   2.39 M/s |   2.34 M/s |   -2.13% |
| | Latency |     5.90 us |     5.30 us |  +10.17% (inv) |
| 1024B | Throughput |   0.51 M/s |   0.87 M/s |  +70.52% |
| | Latency |     5.95 us |     5.47 us |   +8.07% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.03 M/s |  +26.64% |
| | Latency |    13.34 us |    13.70 us |   -2.70% (inv) |
| 131072B | Throughput |   0.01 M/s |   0.02 M/s |  +36.10% |
| | Latency |    21.79 us |    22.20 us |   -1.88% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  +30.64% |
| | Latency |    37.06 us |    37.55 us |   -1.32% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   3.66 M/s |   3.63 M/s |   -1.03% |
| | Latency |     5.32 us |     5.00 us |   +6.02% (inv) |
| 256B | Throughput |   2.53 M/s |   2.42 M/s |   -4.50% |
| | Latency |     5.38 us |     5.08 us |   +5.58% (inv) |
| 1024B | Throughput |   0.97 M/s |   1.01 M/s |   +3.48% |
| | Latency |     5.49 us |     5.13 us |   +6.56% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.03 M/s |  +17.15% |
| | Latency |    14.62 us |    13.39 us |   +8.41% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |   -7.95% |
| | Latency |    20.74 us |    21.60 us |   -4.15% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  +27.11% |
| | Latency |    34.71 us |    37.41 us |   -7.78% (inv) |
