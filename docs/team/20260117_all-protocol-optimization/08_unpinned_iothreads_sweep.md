# Unpinned IO Threads Sweep

## Setup

- BENCH_NO_TASKSET=1 (no CPU pinning)
- BENCH_TRANSPORTS=tcp,ipc
- BENCH_MSG_SIZES=64,1024,262144
- runs=3, refresh libzmq baseline
- build dir: build/bin


## IO_THREADS=1


### PATTERN: PAIR


## PATTERN: PAIR
  > Benchmarking libzmq for PAIR...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for PAIR...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   6.10 M/s |   6.21 M/s |   +1.67% |
| | Latency |    33.48 us |    31.17 us |   +6.90% (inv) |
| 1024B | Throughput |   1.07 M/s |   1.06 M/s |   -0.92% |
| | Latency |    32.13 us |    29.69 us |   +7.59% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -22.84% |
| | Latency |    80.27 us |   154.82 us |  -92.87% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   6.04 M/s |   5.96 M/s |   -1.29% |
| | Latency |    26.77 us |    28.43 us |   -6.20% (inv) |
| 1024B | Throughput |   1.16 M/s |   1.13 M/s |   -3.42% |
| | Latency |    27.30 us |    27.90 us |   -2.20% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |  -10.42% |
| | Latency |   102.03 us |   119.59 us |  -17.21% (inv) |

### PATTERN: PUBSUB


## PATTERN: PUBSUB
  > Benchmarking libzmq for PUBSUB...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for PUBSUB...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.41 M/s |   5.93 M/s |   +9.56% |
| | Latency |     0.18 us |     0.17 us |   +5.56% (inv) |
| 1024B | Throughput |   1.08 M/s |   1.05 M/s |   -2.91% |
| | Latency |     0.93 us |     0.95 us |   -2.15% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -21.41% |
| | Latency |    36.75 us |    46.77 us |  -27.27% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.91 M/s |   5.66 M/s |  +15.19% |
| | Latency |     0.20 us |     0.18 us |  +10.00% (inv) |
| 1024B | Throughput |   1.14 M/s |   1.11 M/s |   -2.67% |
| | Latency |     0.87 us |     0.90 us |   -3.45% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |   -9.56% |
| | Latency |    41.42 us |    45.80 us |  -10.57% (inv) |

### PATTERN: DEALER_DEALER


## PATTERN: DEALER_DEALER
  > Benchmarking libzmq for DEALER_DEALER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for DEALER_DEALER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   6.07 M/s |   6.03 M/s |   -0.56% |
| | Latency |    30.32 us |    43.44 us |  -43.27% (inv) |
| 1024B | Throughput |   1.07 M/s |   1.04 M/s |   -2.52% |
| | Latency |    31.29 us |    34.37 us |   -9.84% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -20.67% |
| | Latency |    80.17 us |   140.63 us |  -75.41% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.69 M/s |   6.04 M/s |   +6.20% |
| | Latency |    30.82 us |    54.62 us |  -77.22% (inv) |
| 1024B | Throughput |   1.12 M/s |   1.11 M/s |   -1.21% |
| | Latency |    54.78 us |    49.84 us |   +9.02% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |   -7.04% |
| | Latency |   120.26 us |    96.77 us |  +19.53% (inv) |

### PATTERN: DEALER_ROUTER


## PATTERN: DEALER_ROUTER
  > Benchmarking libzmq for DEALER_ROUTER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for DEALER_ROUTER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.28 M/s |   5.69 M/s |   +7.78% |
| | Latency |    43.62 us |    38.86 us |  +10.91% (inv) |
| 1024B | Throughput |   1.06 M/s |   1.03 M/s |   -2.94% |
| | Latency |    43.17 us |    41.91 us |   +2.92% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -17.47% |
| | Latency |   269.85 us |   195.10 us |  +27.70% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.47 M/s |   5.72 M/s |   +4.48% |
| | Latency |    42.61 us |    32.94 us |  +22.69% (inv) |
| 1024B | Throughput |   1.16 M/s |   1.13 M/s |   -2.22% |
| | Latency |    43.39 us |    40.96 us |   +5.60% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |   -9.17% |
| | Latency |   194.35 us |    93.96 us |  +51.65% (inv) |

### PATTERN: ROUTER_ROUTER


## PATTERN: ROUTER_ROUTER
  > Benchmarking libzmq for ROUTER_ROUTER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for ROUTER_ROUTER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.95 M/s |   5.36 M/s |   +8.15% |
| | Latency |    16.58 us |    17.84 us |   -7.60% (inv) |
| 1024B | Throughput |   1.02 M/s |   1.03 M/s |   +1.32% |
| | Latency |    16.64 us |    17.00 us |   -2.16% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -16.53% |
| | Latency |   110.47 us |   150.09 us |  -35.86% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.25 M/s |   5.00 M/s |   -4.83% |
| | Latency |    14.60 us |    15.62 us |   -6.99% (inv) |
| 1024B | Throughput |   1.13 M/s |   1.08 M/s |   -4.26% |
| | Latency |    15.10 us |    15.85 us |   -4.97% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |   -7.46% |
| | Latency |    92.80 us |    77.81 us |  +16.15% (inv) |

### PATTERN: ROUTER_ROUTER_POLL


## PATTERN: ROUTER_ROUTER_POLL
  > Benchmarking libzmq for ROUTER_ROUTER_POLL...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for ROUTER_ROUTER_POLL...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.62 M/s |   5.43 M/s |  +17.59% |
| | Latency |    17.54 us |    20.85 us |  -18.87% (inv) |
| 1024B | Throughput |   1.02 M/s |   1.00 M/s |   -1.89% |
| | Latency |    16.54 us |    17.16 us |   -3.75% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -14.95% |
| | Latency |   150.41 us |   150.20 us |   +0.14% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.58 M/s |   5.17 M/s |  +13.09% |
| | Latency |    15.11 us |    15.78 us |   -4.43% (inv) |
| 1024B | Throughput |   1.12 M/s |   1.07 M/s |   -4.52% |
| | Latency |    15.15 us |    16.41 us |   -8.32% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |   -3.24% |
| | Latency |   136.65 us |    90.09 us |  +34.07% (inv) |

## IO_THREADS=2


### PATTERN: PAIR


## PATTERN: PAIR
  > Benchmarking libzmq for PAIR...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for PAIR...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.88 M/s |   6.11 M/s |   +3.86% |
| | Latency |    46.88 us |    29.89 us |  +36.24% (inv) |
| 1024B | Throughput |   1.21 M/s |   1.06 M/s |  -11.69% |
| | Latency |    48.64 us |    29.94 us |  +38.45% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -20.59% |
| | Latency |   174.04 us |   138.48 us |  +20.43% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.01 M/s |   5.96 M/s |  +19.12% |
| | Latency |    44.86 us |    33.27 us |  +25.84% (inv) |
| 1024B | Throughput |   1.10 M/s |   1.12 M/s |   +2.52% |
| | Latency |    44.92 us |    63.01 us |  -40.27% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |  +10.49% |
| | Latency |   301.57 us |   107.73 us |  +64.28% (inv) |

### PATTERN: PUBSUB


## PATTERN: PUBSUB
  > Benchmarking libzmq for PUBSUB...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for PUBSUB...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.06 M/s |   5.68 M/s |  +12.29% |
| | Latency |     0.20 us |     0.18 us |  +10.00% (inv) |
| 1024B | Throughput |   1.19 M/s |   1.04 M/s |  -12.58% |
| | Latency |     0.84 us |     0.96 us |  -14.29% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -20.10% |
| | Latency |    36.59 us |    45.79 us |  -25.14% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.97 M/s |   5.66 M/s |  +13.85% |
| | Latency |     0.20 us |     0.18 us |  +10.00% (inv) |
| 1024B | Throughput |   1.11 M/s |   1.10 M/s |   -0.80% |
| | Latency |     0.90 us |     0.91 us |   -1.11% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |  +24.83% |
| | Latency |    62.30 us |    49.91 us |  +19.89% (inv) |

### PATTERN: DEALER_DEALER


## PATTERN: DEALER_DEALER
  > Benchmarking libzmq for DEALER_DEALER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for DEALER_DEALER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.70 M/s |   5.82 M/s |   +2.06% |
| | Latency |    53.17 us |    34.96 us |  +34.25% (inv) |
| 1024B | Throughput |   1.21 M/s |   1.02 M/s |  -15.71% |
| | Latency |    48.05 us |    34.05 us |  +29.14% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -20.51% |
| | Latency |   272.70 us |   232.58 us |  +14.71% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.11 M/s |   5.83 M/s |  +14.04% |
| | Latency |    49.10 us |    28.77 us |  +41.41% (inv) |
| 1024B | Throughput |   1.07 M/s |   1.10 M/s |   +2.81% |
| | Latency |    47.63 us |    41.90 us |  +12.03% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |  +28.21% |
| | Latency |   288.24 us |   152.62 us |  +47.05% (inv) |

### PATTERN: DEALER_ROUTER


## PATTERN: DEALER_ROUTER
  > Benchmarking libzmq for DEALER_ROUTER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for DEALER_ROUTER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.53 M/s |   5.41 M/s |   -2.16% |
| | Latency |    48.98 us |    39.05 us |  +20.27% (inv) |
| 1024B | Throughput |   1.25 M/s |   1.02 M/s |  -18.02% |
| | Latency |    63.05 us |    37.02 us |  +41.28% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -19.43% |
| | Latency |   131.64 us |   101.33 us |  +23.02% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.81 M/s |   5.65 M/s |  +17.56% |
| | Latency |    60.74 us |    52.79 us |  +13.09% (inv) |
| 1024B | Throughput |   1.10 M/s |   1.12 M/s |   +2.08% |
| | Latency |    46.99 us |    43.29 us |   +7.87% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |  +15.12% |
| | Latency |   128.33 us |   151.33 us |  -17.92% (inv) |

### PATTERN: ROUTER_ROUTER


## PATTERN: ROUTER_ROUTER
  > Benchmarking libzmq for ROUTER_ROUTER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for ROUTER_ROUTER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.84 M/s |   5.30 M/s |   +9.52% |
| | Latency |    23.46 us |    17.39 us |  +25.87% (inv) |
| 1024B | Throughput |   1.20 M/s |   0.99 M/s |  -17.01% |
| | Latency |    22.20 us |    17.09 us |  +23.02% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -18.30% |
| | Latency |   114.16 us |   143.71 us |  -25.88% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.41 M/s |   5.52 M/s |  +25.15% |
| | Latency |    23.57 us |    15.09 us |  +35.98% (inv) |
| 1024B | Throughput |   1.06 M/s |   1.10 M/s |   +3.65% |
| | Latency |    23.74 us |    16.35 us |  +31.13% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |  +33.45% |
| | Latency |    76.50 us |    85.76 us |  -12.10% (inv) |

### PATTERN: ROUTER_ROUTER_POLL


## PATTERN: ROUTER_ROUTER_POLL
  > Benchmarking libzmq for ROUTER_ROUTER_POLL...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for ROUTER_ROUTER_POLL...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.81 M/s |   5.23 M/s |   +8.71% |
| | Latency |    24.66 us |    17.44 us |  +29.28% (inv) |
| 1024B | Throughput |   1.19 M/s |   1.00 M/s |  -15.43% |
| | Latency |    25.60 us |    16.95 us |  +33.79% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -16.68% |
| | Latency |   129.67 us |   100.60 us |  +22.42% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.33 M/s |   5.52 M/s |  +27.58% |
| | Latency |    24.41 us |    15.66 us |  +35.85% (inv) |
| 1024B | Throughput |   1.07 M/s |   1.08 M/s |   +0.41% |
| | Latency |    24.79 us |    15.95 us |  +35.66% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |  +33.51% |
| | Latency |    84.70 us |    84.07 us |   +0.74% (inv) |

## IO_THREADS=3


### PATTERN: PAIR


## PATTERN: PAIR
  > Benchmarking libzmq for PAIR...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for PAIR...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.60 M/s |   6.01 M/s |   +7.26% |
| | Latency |    47.08 us |    36.57 us |  +22.32% (inv) |
| 1024B | Throughput |   1.22 M/s |   1.06 M/s |  -13.25% |
| | Latency |    47.95 us |    42.40 us |  +11.57% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -23.66% |
| | Latency |   190.14 us |   167.75 us |  +11.78% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.35 M/s |   5.70 M/s |   +6.50% |
| | Latency |    44.92 us |    28.28 us |  +37.04% (inv) |
| 1024B | Throughput |   1.09 M/s |   1.07 M/s |   -1.73% |
| | Latency |    45.29 us |    32.73 us |  +27.73% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |   +4.50% |
| | Latency |   195.02 us |   113.65 us |  +41.72% (inv) |

### PATTERN: PUBSUB


## PATTERN: PUBSUB
  > Benchmarking libzmq for PUBSUB...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for PUBSUB...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.34 M/s |   5.62 M/s |   +5.23% |
| | Latency |     0.19 us |     0.18 us |   +5.26% (inv) |
| 1024B | Throughput |   1.18 M/s |   1.01 M/s |  -14.91% |
| | Latency |     0.84 us |     0.99 us |  -17.86% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -19.95% |
| | Latency |    37.69 us |    47.08 us |  -24.91% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.03 M/s |   5.82 M/s |  +15.84% |
| | Latency |     0.20 us |     0.17 us |  +15.00% (inv) |
| 1024B | Throughput |   1.07 M/s |   1.09 M/s |   +2.19% |
| | Latency |     0.94 us |     0.92 us |   +2.13% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |   +8.21% |
| | Latency |    55.63 us |    51.40 us |   +7.60% (inv) |

### PATTERN: DEALER_DEALER


## PATTERN: DEALER_DEALER
  > Benchmarking libzmq for DEALER_DEALER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for DEALER_DEALER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.68 M/s |   5.88 M/s |   +3.57% |
| | Latency |    47.78 us |    29.81 us |  +37.61% (inv) |
| 1024B | Throughput |   1.21 M/s |   1.02 M/s |  -15.66% |
| | Latency |    47.80 us |    30.82 us |  +35.52% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -20.83% |
| | Latency |   101.54 us |   176.33 us |  -73.66% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.39 M/s |   5.95 M/s |  +10.32% |
| | Latency |    44.64 us |    28.21 us |  +36.81% (inv) |
| 1024B | Throughput |   1.07 M/s |   1.10 M/s |   +2.78% |
| | Latency |    71.04 us |    28.59 us |  +59.76% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |  +25.24% |
| | Latency |   162.30 us |   120.60 us |  +25.69% (inv) |

### PATTERN: DEALER_ROUTER


## PATTERN: DEALER_ROUTER
  > Benchmarking libzmq for DEALER_ROUTER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for DEALER_ROUTER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.55 M/s |   5.28 M/s |   -4.97% |
| | Latency |    58.37 us |    43.27 us |  +25.87% (inv) |
| 1024B | Throughput |   1.24 M/s |   1.00 M/s |  -18.87% |
| | Latency |    71.00 us |    47.76 us |  +32.73% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -20.01% |
| | Latency |   153.53 us |   131.61 us |  +14.28% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.26 M/s |   5.65 M/s |   +7.29% |
| | Latency |    45.38 us |    56.35 us |  -24.17% (inv) |
| 1024B | Throughput |   1.02 M/s |   1.12 M/s |   +9.91% |
| | Latency |    54.21 us |    38.99 us |  +28.08% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |  +16.14% |
| | Latency |   113.27 us |    90.13 us |  +20.43% (inv) |

### PATTERN: ROUTER_ROUTER


## PATTERN: ROUTER_ROUTER
  > Benchmarking libzmq for ROUTER_ROUTER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for ROUTER_ROUTER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.87 M/s |   5.24 M/s |   +7.74% |
| | Latency |    24.17 us |    16.56 us |  +31.49% (inv) |
| 1024B | Throughput |   1.17 M/s |   1.01 M/s |  -13.95% |
| | Latency |    22.79 us |    17.02 us |  +25.32% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -17.98% |
| | Latency |    80.63 us |   135.05 us |  -67.49% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.25 M/s |   5.23 M/s |  +22.98% |
| | Latency |    22.85 us |    15.28 us |  +33.13% (inv) |
| 1024B | Throughput |   1.05 M/s |   1.06 M/s |   +1.47% |
| | Latency |    23.20 us |    15.98 us |  +31.12% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |  +34.49% |
| | Latency |    69.30 us |    77.51 us |  -11.85% (inv) |

### PATTERN: ROUTER_ROUTER_POLL


## PATTERN: ROUTER_ROUTER_POLL
  > Benchmarking libzmq for ROUTER_ROUTER_POLL...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for ROUTER_ROUTER_POLL...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.51 M/s |   5.12 M/s |  +13.62% |
| | Latency |    24.32 us |    17.93 us |  +26.27% (inv) |
| 1024B | Throughput |   1.18 M/s |   0.92 M/s |  -22.23% |
| | Latency |    31.03 us |    17.31 us |  +44.22% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -19.96% |
| | Latency |   107.91 us |   157.67 us |  -46.11% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.20 M/s |   5.47 M/s |  +30.18% |
| | Latency |    23.15 us |    15.53 us |  +32.92% (inv) |
| 1024B | Throughput |   1.04 M/s |   1.02 M/s |   -1.72% |
| | Latency |    24.04 us |    17.24 us |  +28.29% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |  +19.91% |
| | Latency |    71.88 us |    90.77 us |  -26.28% (inv) |

## IO_THREADS=4


### PATTERN: PAIR


## PATTERN: PAIR
  > Benchmarking libzmq for PAIR...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for PAIR...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.78 M/s |   6.03 M/s |   +4.38% |
| | Latency |    52.54 us |    29.81 us |  +43.26% (inv) |
| 1024B | Throughput |   1.10 M/s |   1.02 M/s |   -7.40% |
| | Latency |    69.52 us |    35.74 us |  +48.59% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |   -8.72% |
| | Latency |   111.96 us |   155.71 us |  -39.08% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.03 M/s |   5.77 M/s |  +14.72% |
| | Latency |    61.69 us |    33.01 us |  +46.49% (inv) |
| 1024B | Throughput |   1.02 M/s |   1.03 M/s |   +1.31% |
| | Latency |    64.79 us |    45.52 us |  +29.74% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.02 M/s |  +42.22% |
| | Latency |   120.11 us |    95.24 us |  +20.71% (inv) |

### PATTERN: PUBSUB


## PATTERN: PUBSUB
  > Benchmarking libzmq for PUBSUB...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for PUBSUB...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.36 M/s |   5.66 M/s |   +5.67% |
| | Latency |     0.19 us |     0.18 us |   +5.26% (inv) |
| 1024B | Throughput |   1.15 M/s |   1.01 M/s |  -11.44% |
| | Latency |     0.87 us |     0.99 us |  -13.79% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -18.42% |
| | Latency |    38.00 us |    46.58 us |  -22.58% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.93 M/s |   5.86 M/s |  +18.86% |
| | Latency |     0.20 us |     0.17 us |  +15.00% (inv) |
| 1024B | Throughput |   0.94 M/s |   1.09 M/s |  +15.76% |
| | Latency |     1.07 us |     0.92 us |  +14.02% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |   +6.30% |
| | Latency |    53.36 us |    50.20 us |   +5.92% (inv) |

### PATTERN: DEALER_DEALER


## PATTERN: DEALER_DEALER
  > Benchmarking libzmq for DEALER_DEALER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for DEALER_DEALER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.64 M/s |   5.93 M/s |   +5.21% |
| | Latency |    49.33 us |    33.22 us |  +32.66% (inv) |
| 1024B | Throughput |   1.15 M/s |   1.02 M/s |  -11.87% |
| | Latency |    47.46 us |    35.30 us |  +25.62% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -19.97% |
| | Latency |   219.26 us |   105.60 us |  +51.84% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.93 M/s |   5.83 M/s |  +18.29% |
| | Latency |    48.17 us |    28.25 us |  +41.35% (inv) |
| 1024B | Throughput |   1.06 M/s |   0.97 M/s |   -7.93% |
| | Latency |    53.62 us |    32.03 us |  +40.26% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.02 M/s |  +18.06% |
| | Latency |   111.81 us |   111.45 us |   +0.32% (inv) |

### PATTERN: DEALER_ROUTER


## PATTERN: DEALER_ROUTER
  > Benchmarking libzmq for DEALER_ROUTER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for DEALER_ROUTER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.63 M/s |   5.47 M/s |   -2.88% |
| | Latency |    69.71 us |    44.05 us |  +36.81% (inv) |
| 1024B | Throughput |   1.09 M/s |   1.03 M/s |   -5.61% |
| | Latency |    54.48 us |    43.90 us |  +19.42% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -16.28% |
| | Latency |   150.68 us |   246.88 us |  -63.84% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.43 M/s |   5.52 M/s |   +1.70% |
| | Latency |    72.47 us |    39.25 us |  +45.84% (inv) |
| 1024B | Throughput |   1.07 M/s |   1.10 M/s |   +3.34% |
| | Latency |    47.06 us |    42.52 us |   +9.65% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |  +11.18% |
| | Latency |   129.89 us |   102.34 us |  +21.21% (inv) |

### PATTERN: ROUTER_ROUTER


## PATTERN: ROUTER_ROUTER
  > Benchmarking libzmq for ROUTER_ROUTER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for ROUTER_ROUTER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.55 M/s |   5.24 M/s |  +15.13% |
| | Latency |    22.83 us |    20.86 us |   +8.63% (inv) |
| 1024B | Throughput |   1.19 M/s |   1.01 M/s |  -15.67% |
| | Latency |    23.73 us |    16.91 us |  +28.74% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -20.89% |
| | Latency |    99.19 us |   164.01 us |  -65.35% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.46 M/s |   5.01 M/s |  +12.37% |
| | Latency |    23.09 us |    16.16 us |  +30.01% (inv) |
| 1024B | Throughput |   0.94 M/s |   1.05 M/s |  +11.92% |
| | Latency |    23.23 us |    18.44 us |  +20.62% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |  +22.91% |
| | Latency |    91.45 us |    76.06 us |  +16.83% (inv) |

### PATTERN: ROUTER_ROUTER_POLL


## PATTERN: ROUTER_ROUTER_POLL
  > Benchmarking libzmq for ROUTER_ROUTER_POLL...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done
  > Benchmarking zlink for ROUTER_ROUTER_POLL...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
    Testing ipc | 64B: 1 2 3 Done
    Testing ipc | 1024B: 1 2 3 Done
    Testing ipc | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.22 M/s |   5.19 M/s |  +22.80% |
| | Latency |    25.88 us |    17.57 us |  +32.11% (inv) |
| 1024B | Throughput |   1.12 M/s |   0.99 M/s |  -11.15% |
| | Latency |    27.97 us |    16.87 us |  +39.69% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -18.56% |
| | Latency |   130.14 us |   131.26 us |   -0.86% (inv) |

### Transport: ipc
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.52 M/s |   5.14 M/s |  +13.73% |
| | Latency |    24.14 us |    17.58 us |  +27.17% (inv) |
| 1024B | Throughput |   1.02 M/s |   0.98 M/s |   -4.06% |
| | Latency |    24.41 us |    21.26 us |  +12.90% (inv) |
| 262144B | Throughput |   0.02 M/s |   0.02 M/s |   +7.04% |
| | Latency |    83.53 us |   109.80 us |  -31.45% (inv) |


## Summary (Avg Diff %)

- IO_THREADS=1 tcp 64B avg +7.36%
- IO_THREADS=1 tcp 1024B avg -1.64%
- IO_THREADS=1 tcp 262144B avg -18.98%
- IO_THREADS=1 ipc 64B avg +5.47%
- IO_THREADS=1 ipc 1024B avg -3.05%
- IO_THREADS=1 ipc 262144B avg -7.82%
- IO_THREADS=2 tcp 64B avg +5.71%
- IO_THREADS=2 tcp 1024B avg -15.07%
- IO_THREADS=2 tcp 262144B avg -19.27%
- IO_THREADS=2 ipc 64B avg +19.55%
- IO_THREADS=2 ipc 1024B avg +1.78%
- IO_THREADS=2 ipc 262144B avg +24.27%
- IO_THREADS=3 tcp 64B avg +5.41%
- IO_THREADS=3 tcp 1024B avg -16.48%
- IO_THREADS=3 tcp 262144B avg -20.40%
- IO_THREADS=3 ipc 64B avg +15.52%
- IO_THREADS=3 ipc 1024B avg +2.15%
- IO_THREADS=3 ipc 262144B avg +18.08%
- IO_THREADS=4 tcp 64B avg +8.38%
- IO_THREADS=4 tcp 1024B avg -10.52%
- IO_THREADS=4 tcp 262144B avg -17.14%
- IO_THREADS=4 ipc 64B avg +13.28%
- IO_THREADS=4 ipc 1024B avg +3.39%
- IO_THREADS=4 ipc 262144B avg +17.95%
