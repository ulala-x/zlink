# TCP Max Transfer: IO Threads 3/4 (1024, 262144)

## Setup

- BENCH_NO_TASKSET=1
- BENCH_IO_THREADS=3 or 4
- BENCH_TRANSPORTS=tcp
- BENCH_MSG_SIZES=1024,262144
- ZMQ_ASIO_TCP_MAX_TRANSFER=262144
- runs=3, refresh libzmq baseline
- build dir: build/bin

## IO_THREADS=3

## PATTERN: PAIR
  > Benchmarking libzmq for PAIR...
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
  > Benchmarking zlink for PAIR...
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 1024B | Throughput |   1.14 M/s |   0.99 M/s |  -13.29% |
| | Latency |    48.07 us |    33.72 us |  +29.85% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |   -9.73% |
| | Latency |   262.85 us |   176.13 us |  +32.99% (inv) |

## PATTERN: PUBSUB
  > Benchmarking libzmq for PUBSUB...
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
  > Benchmarking zlink for PUBSUB...
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 1024B | Throughput |   1.14 M/s |   0.97 M/s |  -15.03% |
| | Latency |     0.88 us |     1.03 us |  -17.05% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -13.60% |
| | Latency |    38.46 us |    44.51 us |  -15.73% (inv) |

## PATTERN: DEALER_DEALER
  > Benchmarking libzmq for DEALER_DEALER...
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
  > Benchmarking zlink for DEALER_DEALER...
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 1024B | Throughput |   1.16 M/s |   0.99 M/s |  -14.90% |
| | Latency |    48.32 us |    35.86 us |  +25.79% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |   -6.80% |
| | Latency |   233.21 us |   145.95 us |  +37.42% (inv) |

## PATTERN: DEALER_ROUTER
  > Benchmarking libzmq for DEALER_ROUTER...
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
  > Benchmarking zlink for DEALER_ROUTER...
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 1024B | Throughput |   1.20 M/s |   0.99 M/s |  -17.52% |
| | Latency |    99.60 us |    45.63 us |  +54.19% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -11.65% |
| | Latency |   254.55 us |   152.54 us |  +40.07% (inv) |

## PATTERN: ROUTER_ROUTER
  > Benchmarking libzmq for ROUTER_ROUTER...
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
  > Benchmarking zlink for ROUTER_ROUTER...
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 1024B | Throughput |   1.14 M/s |   0.93 M/s |  -18.03% |
| | Latency |    23.79 us |    17.18 us |  +27.78% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |   -5.78% |
| | Latency |   100.80 us |   133.86 us |  -32.80% (inv) |

## PATTERN: ROUTER_ROUTER_POLL
  > Benchmarking libzmq for ROUTER_ROUTER_POLL...
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
  > Benchmarking zlink for ROUTER_ROUTER_POLL...
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 1024B | Throughput |   1.14 M/s |   0.96 M/s |  -15.53% |
| | Latency |    24.06 us |    17.70 us |  +26.43% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |   -7.35% |
| | Latency |   115.04 us |   176.22 us |  -53.18% (inv) |

## IO_THREADS=4

## PATTERN: PAIR
  > Benchmarking libzmq for PAIR...
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
  > Benchmarking zlink for PAIR...
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 1024B | Throughput |   1.15 M/s |   0.99 M/s |  -13.53% |
| | Latency |    46.55 us |    33.24 us |  +28.59% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -10.69% |
| | Latency |   235.73 us |   172.36 us |  +26.88% (inv) |

## PATTERN: PUBSUB
  > Benchmarking libzmq for PUBSUB...
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
  > Benchmarking zlink for PUBSUB...
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 1024B | Throughput |   1.16 M/s |   0.97 M/s |  -16.42% |
| | Latency |     0.86 us |     1.03 us |  -19.77% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |   -9.92% |
| | Latency |    38.82 us |    43.09 us |  -11.00% (inv) |

## PATTERN: DEALER_DEALER
  > Benchmarking libzmq for DEALER_DEALER...
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
  > Benchmarking zlink for DEALER_DEALER...
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 1024B | Throughput |   1.14 M/s |   0.98 M/s |  -14.26% |
| | Latency |    49.11 us |    29.36 us |  +40.22% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |  -13.22% |
| | Latency |   147.10 us |   188.17 us |  -27.92% (inv) |

## PATTERN: DEALER_ROUTER
  > Benchmarking libzmq for DEALER_ROUTER...
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
  > Benchmarking zlink for DEALER_ROUTER...
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 1024B | Throughput |   1.19 M/s |   0.99 M/s |  -17.17% |
| | Latency |    47.86 us |    45.64 us |   +4.64% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |   -9.30% |
| | Latency |   142.22 us |   216.13 us |  -51.97% (inv) |

## PATTERN: ROUTER_ROUTER
  > Benchmarking libzmq for ROUTER_ROUTER...
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
  > Benchmarking zlink for ROUTER_ROUTER...
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 1024B | Throughput |   1.16 M/s |   0.94 M/s |  -18.55% |
| | Latency |    23.34 us |    16.62 us |  +28.79% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |   -8.29% |
| | Latency |   117.17 us |   146.62 us |  -25.13% (inv) |

## PATTERN: ROUTER_ROUTER_POLL
  > Benchmarking libzmq for ROUTER_ROUTER_POLL...
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done
  > Benchmarking zlink for ROUTER_ROUTER_POLL...
    Testing tcp | 1024B: 1 2 3 Done
    Testing tcp | 262144B: 1 2 3 Done

### Transport: tcp
| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 1024B | Throughput |   1.15 M/s |   0.96 M/s |  -16.38% |
| | Latency |    25.39 us |    16.60 us |  +34.62% (inv) |
| 262144B | Throughput |   0.03 M/s |   0.02 M/s |   -7.58% |
| | Latency |   120.73 us |   150.94 us |  -25.02% (inv) |
