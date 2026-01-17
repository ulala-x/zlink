# TCP batch size override tests

## Goal

Reduce the tcp 1024B gap without regressing 262144B when `ZMQ_ASIO_TCP_MAX_TRANSFER=262144` is enabled.

## Setup

- Build: `cmake --build build`
- Unpinned: `BENCH_NO_TASKSET=1`
- IO threads: `BENCH_IO_THREADS=2`
- Transports: `BENCH_TRANSPORTS=tcp`
- Message sizes: `BENCH_MSG_SIZES=1024,262144`
- Max transfer: `ZMQ_ASIO_TCP_MAX_TRANSFER=262144`

## Baseline (no batch override)

### PUBSUB

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 1024B | Throughput | 1.12 M/s | 1.03 M/s | -8.66% |
| 1024B | Latency | 0.79 us | 0.97 us | -22.78% (inv) |
| 262144B | Throughput | 0.03 M/s | 0.02 M/s | -11.98% |
| 262144B | Latency | 37.09 us | 42.13 us | -13.59% (inv) |

### ROUTER_ROUTER

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 1024B | Throughput | 1.20 M/s | 1.02 M/s | -14.91% |
| 1024B | Latency | 25.40 us | 16.78 us | +33.94% (inv) |
| 262144B | Throughput | 0.03 M/s | 0.02 M/s | -11.70% |
| 262144B | Latency | 128.45 us | 167.50 us | -30.40% (inv) |

### ROUTER_ROUTER_POLL

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 1024B | Throughput | 1.19 M/s | 0.99 M/s | -16.68% |
| 1024B | Latency | 25.54 us | 17.29 us | +32.30% (inv) |
| 262144B | Throughput | 0.03 M/s | 0.02 M/s | -12.05% |
| 262144B | Latency | 113.14 us | 173.35 us | -53.22% (inv) |

## Batch override sweep (PUBSUB)

### 16 KB (`ZMQ_ASIO_IN_BATCH_SIZE=16384`, `ZMQ_ASIO_OUT_BATCH_SIZE=16384`)

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 1024B | Throughput | 1.12 M/s | 1.25 M/s | +11.52% |
| 1024B | Latency | 0.79 us | 0.80 us | -1.27% (inv) |
| 262144B | Throughput | 0.03 M/s | 0.02 M/s | -14.22% |
| 262144B | Latency | 37.09 us | 43.24 us | -16.58% (inv) |

### 32 KB (`ZMQ_ASIO_IN_BATCH_SIZE=32768`, `ZMQ_ASIO_OUT_BATCH_SIZE=32768`)

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 1024B | Throughput | 1.12 M/s | 1.32 M/s | +17.60% |
| 1024B | Latency | 0.79 us | 0.76 us | +3.80% (inv) |
| 262144B | Throughput | 0.03 M/s | 0.02 M/s | -18.36% |
| 262144B | Latency | 37.09 us | 45.43 us | -22.49% (inv) |

### 64 KB (`ZMQ_ASIO_IN_BATCH_SIZE=65536`, `ZMQ_ASIO_OUT_BATCH_SIZE=65536`)

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 1024B | Throughput | 1.12 M/s | 1.48 M/s | +31.72% |
| 1024B | Latency | 0.79 us | 0.67 us | +15.19% (inv) |
| 262144B | Throughput | 0.03 M/s | 0.02 M/s | -24.59% |
| 262144B | Latency | 37.09 us | 49.18 us | -32.60% (inv) |

### 64 KB out only (`ZMQ_ASIO_OUT_BATCH_SIZE=65536`)

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 1024B | Throughput | 1.12 M/s | 1.08 M/s | -3.67% |
| 1024B | Latency | 0.79 us | 0.92 us | -16.46% (inv) |
| 262144B | Throughput | 0.03 M/s | 0.02 M/s | -19.13% |
| 262144B | Latency | 37.09 us | 45.86 us | -23.65% (inv) |

### 64 KB in only (`ZMQ_ASIO_IN_BATCH_SIZE=65536`)

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 1024B | Throughput | 1.12 M/s | 1.03 M/s | -8.46% |
| 1024B | Latency | 0.79 us | 0.97 us | -22.78% (inv) |
| 262144B | Throughput | 0.03 M/s | 0.02 M/s | -11.33% |
| 262144B | Latency | 37.09 us | 41.83 us | -12.78% (inv) |

## Batch override on routers (64 KB in+out)

### ROUTER_ROUTER

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 1024B | Throughput | 1.20 M/s | 1.47 M/s | +22.52% |
| 1024B | Latency | 25.40 us | 17.45 us | +31.30% (inv) |
| 262144B | Throughput | 0.03 M/s | 0.02 M/s | -21.01% |
| 262144B | Latency | 128.45 us | 130.45 us | -1.56% (inv) |

### ROUTER_ROUTER_POLL

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 1024B | Throughput | 1.19 M/s | 1.46 M/s | +22.14% |
| 1024B | Latency | 25.54 us | 16.28 us | +36.26% (inv) |
| 262144B | Throughput | 0.03 M/s | 0.02 M/s | -23.82% |
| 262144B | Latency | 113.14 us | 182.21 us | -61.05% (inv) |

## Observations

- Larger batch sizes strongly improve 1024B throughput for all patterns.
- 262144B throughput regresses as batch size grows; 16 KB is the least harmful, 64 KB is the worst.
- Single-sided overrides (in-only or out-only) do not improve 1024B, so the gain appears to come from combined batching.

## Commands

```bash
BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 BENCH_TRANSPORTS=tcp BENCH_MSG_SIZES=1024,262144 \
  ZMQ_ASIO_TCP_MAX_TRANSFER=262144 \
  python3 benchwithzmq/run_comparison.py PUBSUB --runs=3 --refresh-libzmq --build-dir build/bin

BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 BENCH_TRANSPORTS=tcp BENCH_MSG_SIZES=1024,262144 \
  ZMQ_ASIO_TCP_MAX_TRANSFER=262144 ZMQ_ASIO_IN_BATCH_SIZE=65536 ZMQ_ASIO_OUT_BATCH_SIZE=65536 \
  python3 benchwithzmq/run_comparison.py PUBSUB --runs=3 --build-dir build/bin

BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 BENCH_TRANSPORTS=tcp BENCH_MSG_SIZES=1024,262144 \
  ZMQ_ASIO_TCP_MAX_TRANSFER=262144 ZMQ_ASIO_IN_BATCH_SIZE=65536 ZMQ_ASIO_OUT_BATCH_SIZE=65536 \
  python3 benchwithzmq/run_comparison.py ROUTER_ROUTER --runs=3 --build-dir build/bin

BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 BENCH_TRANSPORTS=tcp BENCH_MSG_SIZES=1024,262144 \
  ZMQ_ASIO_TCP_MAX_TRANSFER=262144 ZMQ_ASIO_IN_BATCH_SIZE=65536 ZMQ_ASIO_OUT_BATCH_SIZE=65536 \
  python3 benchwithzmq/run_comparison.py ROUTER_ROUTER_POLL --runs=3 --build-dir build/bin
```
