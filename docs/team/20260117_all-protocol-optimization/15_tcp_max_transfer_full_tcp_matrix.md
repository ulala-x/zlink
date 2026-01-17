# TCP Max Transfer Size: Full TCP Matrix

## Setup

- BENCH_NO_TASKSET=1
- BENCH_IO_THREADS=2
- BENCH_TRANSPORTS=tcp
- BENCH_MSG_SIZES=64,256,1024,65536,131072,262144
- ZMQ_ASIO_TCP_MAX_TRANSFER=262144
- runs=3, refresh libzmq baseline
- build dir: build/bin

## Output

## PATTERN: PAIR
  > Benchmarking libzmq for PAIR...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
  > Benchmarking zlink for PAIR...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.66 M/s |   6.18 M/s |   +9.12% |
| | Latency |    48.43 us |    31.34 us |  +35.29% (inv) |
| 256B | Throughput |   3.24 M/s |   2.61 M/s |  -19.55% |
| | Latency |    48.86 us |    29.10 us |  +40.44% (inv) |
| 1024B | Throughput |   1.21 M/s |   1.06 M/s |  -12.44% |
| | Latency |    46.83 us |    29.01 us |  +38.05% (inv) |
| 65536B | Throughput |   0.08 M/s |   0.06 M/s |  -18.89% |
| | Latency |   102.13 us |    55.52 us |  +45.64% (inv) |
| 131072B | Throughput |   0.05 M/s |   0.04 M/s |  -18.38% |
| | Latency |   162.76 us |   128.97 us |  +20.76% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -13.14% |
| | Latency |   158.99 us |   146.23 us |   +8.03% (inv) |

## PATTERN: PUBSUB
  > Benchmarking libzmq for PUBSUB...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
  > Benchmarking zlink for PUBSUB...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.16 M/s |   5.65 M/s |   +9.51% |
| | Latency |     0.19 us |     0.18 us |   +5.26% (inv) |
| 256B | Throughput |   3.18 M/s |   2.60 M/s |  -18.12% |
| | Latency |     0.31 us |     0.38 us |  -22.58% (inv) |
| 1024B | Throughput |   1.27 M/s |   1.05 M/s |  -17.44% |
| | Latency |     0.79 us |     0.95 us |  -20.25% (inv) |
| 65536B | Throughput |   0.08 M/s |   0.07 M/s |  -19.11% |
| | Latency |    12.17 us |    14.48 us |  -18.98% (inv) |
| 131072B | Throughput |   0.05 M/s |   0.04 M/s |  -16.64% |
| | Latency |    20.17 us |    24.20 us |  -19.98% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -14.15% |
| | Latency |    36.30 us |    42.28 us |  -16.47% (inv) |

## PATTERN: DEALER_DEALER
  > Benchmarking libzmq for DEALER_DEALER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
  > Benchmarking zlink for DEALER_DEALER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.66 M/s |   5.94 M/s |   +4.97% |
| | Latency |    48.84 us |    31.44 us |  +35.63% (inv) |
| 256B | Throughput |   3.31 M/s |   2.62 M/s |  -20.79% |
| | Latency |    59.02 us |    30.02 us |  +49.14% (inv) |
| 1024B | Throughput |   1.32 M/s |   1.05 M/s |  -20.38% |
| | Latency |    46.70 us |    30.01 us |  +35.74% (inv) |
| 65536B | Throughput |   0.08 M/s |   0.07 M/s |  -19.45% |
| | Latency |    77.39 us |    48.84 us |  +36.89% (inv) |
| 131072B | Throughput |   0.05 M/s |   0.04 M/s |  -20.90% |
| | Latency |   132.36 us |    62.51 us |  +52.77% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -13.20% |
| | Latency |   223.48 us |   125.82 us |  +43.70% (inv) |

## PATTERN: DEALER_ROUTER
  > Benchmarking libzmq for DEALER_ROUTER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
  > Benchmarking zlink for DEALER_ROUTER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.47 M/s |   5.40 M/s |   -1.29% |
| | Latency |    74.60 us |    43.81 us |  +41.27% (inv) |
| 256B | Throughput |   3.15 M/s |   2.52 M/s |  -20.15% |
| | Latency |    70.29 us |    42.11 us |  +40.09% (inv) |
| 1024B | Throughput |   1.26 M/s |   1.05 M/s |  -16.23% |
| | Latency |    68.32 us |    43.63 us |  +36.14% (inv) |
| 65536B | Throughput |   0.08 M/s |   0.07 M/s |  -20.65% |
| | Latency |   122.88 us |    95.52 us |  +22.27% (inv) |
| 131072B | Throughput |   0.05 M/s |   0.04 M/s |  -16.92% |
| | Latency |   116.52 us |   138.44 us |  -18.81% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -14.96% |
| | Latency |   258.44 us |   174.53 us |  +32.47% (inv) |

## PATTERN: ROUTER_ROUTER
  > Benchmarking libzmq for ROUTER_ROUTER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
  > Benchmarking zlink for ROUTER_ROUTER...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.07 M/s |   5.23 M/s |   +3.14% |
| | Latency |    23.51 us |    16.90 us |  +28.12% (inv) |
| 256B | Throughput |   2.84 M/s |   2.39 M/s |  -15.77% |
| | Latency |    27.00 us |    17.32 us |  +35.85% (inv) |
| 1024B | Throughput |   1.21 M/s |   1.02 M/s |  -16.37% |
| | Latency |    22.87 us |    17.16 us |  +24.97% (inv) |
| 65536B | Throughput |   0.08 M/s |   0.07 M/s |  -13.66% |
| | Latency |    37.49 us |    30.79 us |  +17.87% (inv) |
| 131072B | Throughput |   0.05 M/s |   0.04 M/s |  -18.08% |
| | Latency |    48.10 us |    49.28 us |   -2.45% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -10.90% |
| | Latency |   119.46 us |   105.83 us |  +11.41% (inv) |

## PATTERN: ROUTER_ROUTER_POLL
  > Benchmarking libzmq for ROUTER_ROUTER_POLL...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
  > Benchmarking zlink for ROUTER_ROUTER_POLL...
    Testing tcp | 64B: 1 2 3 Done
    Testing tcp | 256B: 1 2 3 Done
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 65536B: 1 2 3 Done
    Testing tcp | 131072B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.75 M/s |   5.26 M/s |  +10.56% |
| | Latency |    24.25 us |    16.59 us |  +31.59% (inv) |
| 256B | Throughput |   2.85 M/s |   2.39 M/s |  -16.19% |
| | Latency |    29.11 us |    18.12 us |  +37.75% (inv) |
| 1024B | Throughput |   1.22 M/s |   1.02 M/s |  -16.38% |
| | Latency |    24.20 us |    16.75 us |  +30.79% (inv) |
| 65536B | Throughput |   0.08 M/s |   0.07 M/s |  -15.79% |
| | Latency |    34.78 us |    30.61 us |  +11.99% (inv) |
| 131072B | Throughput |   0.05 M/s |   0.04 M/s |  -13.29% |
| | Latency |    47.40 us |    50.70 us |   -6.96% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -11.07% |
| | Latency |   120.34 us |   143.39 us |  -19.15% (inv) |
