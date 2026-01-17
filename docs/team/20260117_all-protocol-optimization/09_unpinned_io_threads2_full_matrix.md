# Unpinned IO Threads=2 Full Matrix

## Setup

- BENCH_NO_TASKSET=1 (no CPU pinning)
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
| 64B | Throughput |   6.32 M/s |   5.95 M/s |   -5.79% |
| | Latency |     0.07 us |     0.07 us |   +0.00% (inv) |
| 256B | Throughput |   5.61 M/s |   5.39 M/s |   -3.80% |
| | Latency |     0.08 us |     0.07 us |  +12.50% (inv) |
| 1024B | Throughput |   2.22 M/s |   2.11 M/s |   -5.08% |
| | Latency |     0.09 us |     0.09 us |   +0.00% (inv) |
| 65536B | Throughput |   0.15 M/s |   0.15 M/s |   -4.15% |
| | Latency |     1.96 us |     1.96 us |   +0.00% (inv) |
| 131072B | Throughput |   0.09 M/s |   0.08 M/s |   -5.08% |
| | Latency |     3.55 us |     3.56 us |   -0.28% (inv) |
| 262144B | Throughput |   0.05 M/s |   0.05 M/s |   -3.03% |
| | Latency |     7.37 us |     6.87 us |   +6.78% (inv) |

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.80 M/s |   6.19 M/s |   +6.65% |
| | Latency |    46.32 us |    28.26 us |  +38.99% (inv) |
| 256B | Throughput |   3.19 M/s |   2.56 M/s |  -19.69% |
| | Latency |    51.34 us |    28.95 us |  +43.61% (inv) |
| 1024B | Throughput |   1.20 M/s |   1.01 M/s |  -15.65% |
| | Latency |    47.38 us |    29.24 us |  +38.29% (inv) |
| 65536B | Throughput |   0.08 M/s |   0.07 M/s |  -18.45% |
| | Latency |    84.23 us |    61.98 us |  +26.42% (inv) |
| 131072B | Throughput |   0.05 M/s |   0.04 M/s |  -22.30% |
| | Latency |   141.58 us |    91.07 us |  +35.68% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -21.77% |
| | Latency |   264.26 us |   222.80 us |  +15.69% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.28 M/s |   5.91 M/s |  +12.05% |
| | Latency |    64.04 us |    29.70 us |  +53.62% (inv) |
| 256B | Throughput |   3.04 M/s |   2.74 M/s |   -9.90% |
| | Latency |    66.00 us |    28.25 us |  +57.20% (inv) |
| 1024B | Throughput |   0.93 M/s |   1.10 M/s |  +17.59% |
| | Latency |    53.35 us |    28.44 us |  +46.69% (inv) |
| 65536B | Throughput |   0.02 M/s |   0.06 M/s | +203.96% |
| | Latency |    85.92 us |    76.96 us |  +10.43% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.04 M/s |  +53.85% |
| | Latency |   118.67 us |    69.31 us |  +41.59% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |  +21.49% |
| | Latency |   137.64 us |   138.95 us |   -0.95% (inv) |

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
| 64B | Throughput |   5.56 M/s |   5.55 M/s |   -0.35% |
| | Latency |     0.18 us |     0.18 us |   +0.00% (inv) |
| 256B | Throughput |   4.47 M/s |   4.45 M/s |   -0.30% |
| | Latency |     0.22 us |     0.22 us |   +0.00% (inv) |
| 1024B | Throughput |   2.04 M/s |   2.01 M/s |   -1.53% |
| | Latency |     0.49 us |     0.50 us |   -2.04% (inv) |
| 65536B | Throughput |   0.15 M/s |   0.15 M/s |   +3.30% |
| | Latency |     6.69 us |     6.48 us |   +3.14% (inv) |
| 131072B | Throughput |   0.09 M/s |   0.09 M/s |   -0.91% |
| | Latency |    11.01 us |    11.11 us |   -0.91% (inv) |
| 262144B | Throughput |   0.06 M/s |   0.05 M/s |  -12.95% |
| | Latency |    17.80 us |    20.45 us |  -14.89% (inv) |

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.21 M/s |   5.69 M/s |   +9.07% |
| | Latency |     0.19 us |     0.18 us |   +5.26% (inv) |
| 256B | Throughput |   3.14 M/s |   2.51 M/s |  -20.27% |
| | Latency |     0.32 us |     0.40 us |  -25.00% (inv) |
| 1024B | Throughput |   1.19 M/s |   1.03 M/s |  -13.46% |
| | Latency |     0.84 us |     0.97 us |  -15.48% (inv) |
| 65536B | Throughput |   0.08 M/s |   0.06 M/s |  -17.64% |
| | Latency |    12.74 us |    15.47 us |  -21.43% (inv) |
| 131072B | Throughput |   0.05 M/s |   0.04 M/s |  -21.57% |
| | Latency |    20.67 us |    26.35 us |  -27.48% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -20.14% |
| | Latency |    37.32 us |    46.74 us |  -25.24% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.00 M/s |   5.61 M/s |  +12.20% |
| | Latency |     0.20 us |     0.18 us |  +10.00% (inv) |
| 256B | Throughput |   2.90 M/s |   2.52 M/s |  -12.93% |
| | Latency |     0.35 us |     0.40 us |  -14.29% (inv) |
| 1024B | Throughput |   1.05 M/s |   1.09 M/s |   +3.97% |
| | Latency |     0.95 us |     0.92 us |   +3.16% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.07 M/s | +157.09% |
| | Latency |    37.49 us |    14.17 us |  +62.20% (inv) |
| 131072B | Throughput |   0.03 M/s |   0.04 M/s |  +40.24% |
| | Latency |    39.55 us |    28.20 us |  +28.70% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |   +9.10% |
| | Latency |    56.54 us |    51.83 us |   +8.33% (inv) |

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
| 64B | Throughput |   5.80 M/s |   5.60 M/s |   -3.56% |
| | Latency |     0.07 us |     0.08 us |  -14.29% (inv) |
| 256B | Throughput |   4.83 M/s |   5.14 M/s |   +6.37% |
| | Latency |     0.08 us |     0.08 us |   +0.00% (inv) |
| 1024B | Throughput |   2.22 M/s |   2.25 M/s |   +1.10% |
| | Latency |     0.10 us |     0.10 us |   +0.00% (inv) |
| 65536B | Throughput |   0.15 M/s |   0.15 M/s |   -0.54% |
| | Latency |     1.96 us |     2.00 us |   -2.04% (inv) |
| 131072B | Throughput |   0.08 M/s |   0.09 M/s |   +4.14% |
| | Latency |     3.56 us |     3.57 us |   -0.28% (inv) |
| 262144B | Throughput |   0.05 M/s |   0.08 M/s |  +61.65% |
| | Latency |     7.03 us |     7.18 us |   -2.13% (inv) |

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.94 M/s |   5.82 M/s |   -2.02% |
| | Latency |    47.44 us |    36.32 us |  +23.44% (inv) |
| 256B | Throughput |   3.24 M/s |   2.59 M/s |  -20.25% |
| | Latency |    47.75 us |    32.10 us |  +32.77% (inv) |
| 1024B | Throughput |   1.21 M/s |   1.03 M/s |  -15.04% |
| | Latency |    44.21 us |    31.67 us |  +28.36% (inv) |
| 65536B | Throughput |   0.08 M/s |   0.07 M/s |  -15.80% |
| | Latency |    98.78 us |    97.91 us |   +0.88% (inv) |
| 131072B | Throughput |   0.05 M/s |   0.04 M/s |  -18.08% |
| | Latency |   119.20 us |   129.41 us |   -8.57% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -17.66% |
| | Latency |    96.29 us |   121.18 us |  -25.85% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.94 M/s |   5.78 M/s |  +16.89% |
| | Latency |    46.21 us |    34.75 us |  +24.80% (inv) |
| 256B | Throughput |   2.85 M/s |   2.76 M/s |   -3.39% |
| | Latency |    47.40 us |    27.71 us |  +41.54% (inv) |
| 1024B | Throughput |   0.88 M/s |   1.12 M/s |  +28.05% |
| | Latency |    47.29 us |    29.97 us |  +36.63% (inv) |
| 65536B | Throughput |   0.02 M/s |   0.07 M/s | +207.21% |
| | Latency |    89.96 us |   122.81 us |  -36.52% (inv) |
| 131072B | Throughput |   0.03 M/s |   0.04 M/s |  +46.65% |
| | Latency |   113.53 us |    87.95 us |  +22.53% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |  +20.96% |
| | Latency |   186.40 us |   150.86 us |  +19.07% (inv) |

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
| 64B | Throughput |   4.60 M/s |   4.70 M/s |   +2.02% |
| | Latency |     0.11 us |     0.11 us |   +0.00% (inv) |
| 256B | Throughput |   3.62 M/s |   3.77 M/s |   +4.05% |
| | Latency |     0.11 us |     0.11 us |   +0.00% (inv) |
| 1024B | Throughput |   1.86 M/s |   1.83 M/s |   -1.60% |
| | Latency |     0.13 us |     0.14 us |   -7.69% (inv) |
| 65536B | Throughput |   0.14 M/s |   0.14 M/s |   -1.28% |
| | Latency |     1.95 us |     1.97 us |   -1.03% (inv) |
| 131072B | Throughput |   0.08 M/s |   0.08 M/s |   -0.99% |
| | Latency |     3.62 us |     3.53 us |   +2.49% (inv) |
| 262144B | Throughput |   0.05 M/s |   0.06 M/s |   +6.05% |
| | Latency |     6.81 us |     6.91 us |   -1.47% (inv) |

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.62 M/s |   5.35 M/s |   -4.81% |
| | Latency |    46.59 us |    40.62 us |  +12.81% (inv) |
| 256B | Throughput |   3.12 M/s |   2.60 M/s |  -16.41% |
| | Latency |    54.86 us |    46.69 us |  +14.89% (inv) |
| 1024B | Throughput |   1.24 M/s |   1.04 M/s |  -15.80% |
| | Latency |    75.41 us |    45.67 us |  +39.44% (inv) |
| 65536B | Throughput |   0.08 M/s |   0.06 M/s |  -24.32% |
| | Latency |   205.29 us |   173.71 us |  +15.38% (inv) |
| 131072B | Throughput |   0.05 M/s |   0.04 M/s |  -23.03% |
| | Latency |   165.42 us |    94.10 us |  +43.11% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -18.96% |
| | Latency |   182.91 us |   168.23 us |   +8.03% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.63 M/s |   5.64 M/s |  +22.03% |
| | Latency |    49.64 us |    43.93 us |  +11.50% (inv) |
| 256B | Throughput |   2.87 M/s |   2.70 M/s |   -5.91% |
| | Latency |    52.24 us |    34.51 us |  +33.94% (inv) |
| 1024B | Throughput |   0.97 M/s |   1.11 M/s |  +13.90% |
| | Latency |    51.71 us |    43.07 us |  +16.71% (inv) |
| 65536B | Throughput |   0.02 M/s |   0.07 M/s | +196.08% |
| | Latency |    72.16 us |    53.06 us |  +26.47% (inv) |
| 131072B | Throughput |   0.03 M/s |   0.04 M/s |  +43.90% |
| | Latency |    87.21 us |    67.70 us |  +22.37% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |  +33.46% |
| | Latency |   178.00 us |   194.19 us |   -9.10% (inv) |

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
| 64B | Throughput |   4.35 M/s |   4.66 M/s |   +7.12% |
| | Latency |     0.16 us |     0.17 us |   -6.25% (inv) |
| 256B | Throughput |   3.25 M/s |   3.16 M/s |   -2.98% |
| | Latency |     0.17 us |     0.17 us |   +0.00% (inv) |
| 1024B | Throughput |   1.81 M/s |   1.78 M/s |   -1.74% |
| | Latency |     0.19 us |     0.19 us |   +0.00% (inv) |
| 65536B | Throughput |   0.19 M/s |   0.19 M/s |   +1.48% |
| | Latency |     2.02 us |     2.01 us |   +0.50% (inv) |
| 131072B | Throughput |   0.09 M/s |   0.09 M/s |   +1.89% |
| | Latency |     3.96 us |     3.86 us |   +2.53% (inv) |
| 262144B | Throughput |   0.06 M/s |   0.05 M/s |  -13.06% |
| | Latency |     7.07 us |     7.02 us |   +0.71% (inv) |

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.85 M/s |   5.28 M/s |   +8.86% |
| | Latency |    23.24 us |    17.07 us |  +26.55% (inv) |
| 256B | Throughput |   2.83 M/s |   2.35 M/s |  -16.94% |
| | Latency |    24.09 us |    17.15 us |  +28.81% (inv) |
| 1024B | Throughput |   1.18 M/s |   1.02 M/s |  -14.10% |
| | Latency |    23.04 us |    17.23 us |  +25.22% (inv) |
| 65536B | Throughput |   0.08 M/s |   0.07 M/s |  -15.22% |
| | Latency |    41.35 us |    29.93 us |  +27.62% (inv) |
| 131072B | Throughput |   0.05 M/s |   0.04 M/s |  -19.43% |
| | Latency |    69.74 us |    49.23 us |  +29.41% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -20.31% |
| | Latency |   114.26 us |   143.42 us |  -25.52% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.52 M/s |   5.44 M/s |  +20.29% |
| | Latency |    24.53 us |    24.46 us |   +0.29% (inv) |
| 256B | Throughput |   2.67 M/s |   2.46 M/s |   -7.61% |
| | Latency |    23.78 us |    15.67 us |  +34.10% (inv) |
| 1024B | Throughput |   1.07 M/s |   1.11 M/s |   +3.69% |
| | Latency |    23.21 us |    16.01 us |  +31.02% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.07 M/s | +163.10% |
| | Latency |    41.18 us |    30.31 us |  +26.40% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.04 M/s | +103.97% |
| | Latency |    59.07 us |    94.33 us |  -59.69% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |  +32.06% |
| | Latency |    85.18 us |    92.90 us |   -9.06% (inv) |

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
| 64B | Throughput |   3.93 M/s |   3.89 M/s |   -1.20% |
| | Latency |     0.53 us |     0.48 us |   +9.43% (inv) |
| 256B | Throughput |   3.24 M/s |   3.18 M/s |   -2.09% |
| | Latency |     0.53 us |     0.46 us |  +13.21% (inv) |
| 1024B | Throughput |   1.79 M/s |   1.81 M/s |   +0.63% |
| | Latency |     0.56 us |     0.50 us |  +10.71% (inv) |
| 65536B | Throughput |   0.19 M/s |   0.19 M/s |   +4.67% |
| | Latency |     2.40 us |     2.39 us |   +0.42% (inv) |
| 131072B | Throughput |   0.09 M/s |   0.07 M/s |  -15.39% |
| | Latency |     4.14 us |     4.14 us |   +0.00% (inv) |
| 262144B | Throughput |   0.06 M/s |   0.05 M/s |  -22.85% |
| | Latency |     7.57 us |     7.42 us |   +1.98% (inv) |

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.77 M/s |   5.26 M/s |  +10.27% |
| | Latency |    24.83 us |    16.23 us |  +34.64% (inv) |
| 256B | Throughput |   2.73 M/s |   2.41 M/s |  -11.62% |
| | Latency |    25.07 us |    16.34 us |  +34.82% (inv) |
| 1024B | Throughput |   1.19 M/s |   1.01 M/s |  -15.10% |
| | Latency |    25.28 us |    16.43 us |  +35.01% (inv) |
| 65536B | Throughput |   0.08 M/s |   0.07 M/s |  -13.19% |
| | Latency |    39.03 us |    30.50 us |  +21.85% (inv) |
| 131072B | Throughput |   0.05 M/s |   0.04 M/s |  -18.27% |
| | Latency |    56.41 us |    50.47 us |  +10.53% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -18.98% |
| | Latency |   106.49 us |   161.69 us |  -51.84% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.38 M/s |   5.34 M/s |  +21.88% |
| | Latency |    24.25 us |    16.19 us |  +33.24% (inv) |
| 256B | Throughput |   2.73 M/s |   2.45 M/s |  -10.12% |
| | Latency |    24.10 us |    15.95 us |  +33.82% (inv) |
| 1024B | Throughput |   1.05 M/s |   1.09 M/s |   +3.24% |
| | Latency |    23.38 us |    15.85 us |  +32.21% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.07 M/s | +184.85% |
| | Latency |    34.76 us |    30.12 us |  +13.35% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.04 M/s |  +71.10% |
| | Latency |    75.29 us |    91.35 us |  -21.33% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |  +20.08% |
| | Latency |    97.54 us |    87.12 us |  +10.68% (inv) |
