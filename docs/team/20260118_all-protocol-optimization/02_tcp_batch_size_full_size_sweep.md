# TCP batch size full-size sweep

## Setup

- Build: `cmake --build build`
- Unpinned: `BENCH_NO_TASKSET=1`
- IO threads: `BENCH_IO_THREADS=2`
- Transports: `BENCH_TRANSPORTS=tcp`
- Message sizes: `BENCH_MSG_SIZES=64,256,1024,65536,131072,262144`
- Max transfer: `ZMQ_ASIO_TCP_MAX_TRANSFER=262144`

## PUBSUB

### Baseline (no batch override)

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 64B | Throughput | 5.07 M/s | 5.65 M/s | +11.62% |
| 64B | Latency | 0.20 us | 0.18 us | +10.00% (inv) |
| 256B | Throughput | 3.09 M/s | 2.56 M/s | -17.23% |
| 256B | Latency | 0.32 us | 0.39 us | -21.88% (inv) |
| 1024B | Throughput | 1.23 M/s | 1.02 M/s | -17.39% |
| 1024B | Latency | 0.81 us | 0.98 us | -20.99% (inv) |
| 65536B | Throughput | 0.08 M/s | 0.06 M/s | -21.35% |
| 65536B | Latency | 12.17 us | 15.48 us | -27.20% (inv) |
| 131072B | Throughput | 0.05 M/s | 0.04 M/s | -19.26% |
| 131072B | Latency | 19.80 us | 24.53 us | -23.89% (inv) |
| 262144B | Throughput | 0.03 M/s | 0.03 M/s | -9.53% |
| 262144B | Latency | 36.03 us | 39.82 us | -10.52% (inv) |

### 16 KB (`ZMQ_ASIO_IN_BATCH_SIZE=16384`, `ZMQ_ASIO_OUT_BATCH_SIZE=16384`)

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 64B | Throughput | 5.07 M/s | 6.35 M/s | +25.44% |
| 64B | Latency | 0.20 us | 0.16 us | +20.00% (inv) |
| 256B | Throughput | 3.09 M/s | 2.88 M/s | -6.63% |
| 256B | Latency | 0.32 us | 0.35 us | -9.37% (inv) |
| 1024B | Throughput | 1.23 M/s | 1.22 M/s | -0.54% |
| 1024B | Latency | 0.81 us | 0.75 us | +7.41% (inv) |
| 65536B | Throughput | 0.08 M/s | 0.07 M/s | -20.14% |
| 65536B | Latency | 12.17 us | 15.24 us | -25.23% (inv) |
| 131072B | Throughput | 0.05 M/s | 0.04 M/s | -20.40% |
| 131072B | Latency | 19.80 us | 24.88 us | -25.66% (inv) |
| 262144B | Throughput | 0.03 M/s | 0.02 M/s | -15.35% |
| 262144B | Latency | 36.03 us | 42.56 us | -18.12% (inv) |

### 32 KB (`ZMQ_ASIO_IN_BATCH_SIZE=32768`, `ZMQ_ASIO_OUT_BATCH_SIZE=32768`)

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 64B | Throughput | 5.07 M/s | 6.23 M/s | +22.96% |
| 64B | Latency | 0.20 us | 0.16 us | +20.00% (inv) |
| 256B | Throughput | 3.09 M/s | 3.25 M/s | +5.19% |
| 256B | Latency | 0.32 us | 0.31 us | +3.13% (inv) |
| 1024B | Throughput | 1.23 M/s | 1.37 M/s | +11.49% |
| 1024B | Latency | 0.81 us | 0.73 us | +9.88% (inv) |
| 65536B | Throughput | 0.08 M/s | 0.06 M/s | -27.59% |
| 65536B | Latency | 12.17 us | 16.81 us | -38.13% (inv) |
| 131072B | Throughput | 0.05 M/s | 0.04 M/s | -24.60% |
| 131072B | Latency | 19.80 us | 26.26 us | -32.63% (inv) |
| 262144B | Throughput | 0.03 M/s | 0.02 M/s | -23.68% |
| 262144B | Latency | 36.03 us | 47.20 us | -31.00% (inv) |

### 64 KB (`ZMQ_ASIO_IN_BATCH_SIZE=65536`, `ZMQ_ASIO_OUT_BATCH_SIZE=65536`)

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 64B | Throughput | 5.07 M/s | 6.03 M/s | +19.02% |
| 64B | Latency | 0.20 us | 0.17 us | +15.00% (inv) |
| 256B | Throughput | 3.09 M/s | 3.85 M/s | +24.67% |
| 256B | Latency | 0.32 us | 0.26 us | +18.75% (inv) |
| 1024B | Throughput | 1.23 M/s | 1.54 M/s | +25.30% |
| 1024B | Latency | 0.81 us | 0.65 us | +19.75% (inv) |
| 65536B | Throughput | 0.08 M/s | 0.06 M/s | -23.80% |
| 65536B | Latency | 12.17 us | 15.98 us | -31.31% (inv) |
| 131072B | Throughput | 0.05 M/s | 0.04 M/s | -28.10% |
| 131072B | Latency | 19.80 us | 27.54 us | -39.09% (inv) |
| 262144B | Throughput | 0.03 M/s | 0.02 M/s | -23.69% |
| 262144B | Latency | 36.03 us | 47.21 us | -31.03% (inv) |

## ROUTER_ROUTER

### Baseline (no batch override)

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 64B | Throughput | 4.85 M/s | 5.27 M/s | +8.61% |
| 64B | Latency | 23.02 us | 16.95 us | +26.37% (inv) |
| 256B | Throughput | 2.84 M/s | 2.36 M/s | -17.00% |
| 256B | Latency | 25.10 us | 16.71 us | +33.43% (inv) |
| 1024B | Throughput | 1.22 M/s | 1.02 M/s | -16.35% |
| 1024B | Latency | 22.13 us | 17.18 us | +22.37% (inv) |
| 65536B | Throughput | 0.08 M/s | 0.07 M/s | -12.00% |
| 65536B | Latency | 43.58 us | 30.57 us | +29.85% (inv) |
| 131072B | Throughput | 0.05 M/s | 0.04 M/s | -17.25% |
| 131072B | Latency | 58.65 us | 42.11 us | +28.20% (inv) |
| 262144B | Throughput | 0.03 M/s | 0.02 M/s | -12.13% |
| 262144B | Latency | 98.34 us | 140.86 us | -43.24% (inv) |

### 64 KB (`ZMQ_ASIO_IN_BATCH_SIZE=65536`, `ZMQ_ASIO_OUT_BATCH_SIZE=65536`)

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 64B | Throughput | 4.85 M/s | 5.44 M/s | +12.04% |
| 64B | Latency | 23.02 us | 16.74 us | +27.28% (inv) |
| 256B | Throughput | 2.84 M/s | 3.28 M/s | +15.52% |
| 256B | Latency | 25.10 us | 16.79 us | +33.11% (inv) |
| 1024B | Throughput | 1.22 M/s | 1.48 M/s | +20.90% |
| 1024B | Latency | 22.13 us | 16.71 us | +24.49% (inv) |
| 65536B | Throughput | 0.08 M/s | 0.07 M/s | -16.55% |
| 65536B | Latency | 43.58 us | 35.18 us | +19.27% (inv) |
| 131072B | Throughput | 0.05 M/s | 0.04 M/s | -28.60% |
| 131072B | Latency | 58.65 us | 56.03 us | +4.47% (inv) |
| 262144B | Throughput | 0.03 M/s | 0.02 M/s | -22.81% |
| 262144B | Latency | 98.34 us | 150.67 us | -53.21% (inv) |

## ROUTER_ROUTER_POLL

### Baseline (no batch override)

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 64B | Throughput | 4.81 M/s | 5.28 M/s | +9.58% |
| 64B | Latency | 24.23 us | 16.20 us | +33.14% (inv) |
| 256B | Throughput | 2.76 M/s | 2.43 M/s | -11.98% |
| 256B | Latency | 24.49 us | 16.82 us | +31.32% (inv) |
| 1024B | Throughput | 1.22 M/s | 1.01 M/s | -17.15% |
| 1024B | Latency | 24.38 us | 17.64 us | +27.65% (inv) |
| 65536B | Throughput | 0.08 M/s | 0.07 M/s | -14.67% |
| 65536B | Latency | 36.61 us | 33.95 us | +7.27% (inv) |
| 131072B | Throughput | 0.05 M/s | 0.04 M/s | -12.92% |
| 131072B | Latency | 57.86 us | 44.59 us | +22.93% (inv) |
| 262144B | Throughput | 0.03 M/s | 0.02 M/s | -10.19% |
| 262144B | Latency | 104.52 us | 136.14 us | -30.25% (inv) |

### 64 KB (`ZMQ_ASIO_IN_BATCH_SIZE=65536`, `ZMQ_ASIO_OUT_BATCH_SIZE=65536`)

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 64B | Throughput | 4.81 M/s | 5.42 M/s | +12.55% |
| 64B | Latency | 24.23 us | 16.93 us | +30.13% (inv) |
| 256B | Throughput | 2.76 M/s | 3.30 M/s | +19.51% |
| 256B | Latency | 24.49 us | 16.70 us | +31.81% (inv) |
| 1024B | Throughput | 1.22 M/s | 1.51 M/s | +24.11% |
| 1024B | Latency | 24.38 us | 16.75 us | +31.30% (inv) |
| 65536B | Throughput | 0.08 M/s | 0.07 M/s | -19.01% |
| 65536B | Latency | 36.61 us | 34.74 us | +5.11% (inv) |
| 131072B | Throughput | 0.05 M/s | 0.04 M/s | -26.24% |
| 131072B | Latency | 57.86 us | 59.10 us | -2.14% (inv) |
| 262144B | Throughput | 0.03 M/s | 0.02 M/s | -18.97% |
| 262144B | Latency | 104.52 us | 164.87 us | -57.74% (inv) |

## Observations

- Static batching improves 64B/256B/1024B across all patterns but regresses 64KB+ throughput.
- The inflection point appears between 1024B and 65536B for these tcp runs.
- This suggests static batching needs to be size-aware or configured per workload.

## Commands

```bash
BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 BENCH_TRANSPORTS=tcp \
  BENCH_MSG_SIZES=64,256,1024,65536,131072,262144 ZMQ_ASIO_TCP_MAX_TRANSFER=262144 \
  python3 benchwithzmq/run_comparison.py PUBSUB --runs=3 --refresh-libzmq --build-dir build/bin

BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 BENCH_TRANSPORTS=tcp \
  BENCH_MSG_SIZES=64,256,1024,65536,131072,262144 ZMQ_ASIO_TCP_MAX_TRANSFER=262144 \
  ZMQ_ASIO_IN_BATCH_SIZE=65536 ZMQ_ASIO_OUT_BATCH_SIZE=65536 \
  python3 benchwithzmq/run_comparison.py PUBSUB --runs=3 --build-dir build/bin

BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 BENCH_TRANSPORTS=tcp \
  BENCH_MSG_SIZES=64,256,1024,65536,131072,262144 ZMQ_ASIO_TCP_MAX_TRANSFER=262144 \
  python3 benchwithzmq/run_comparison.py ROUTER_ROUTER --runs=3 --refresh-libzmq --build-dir build/bin

BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 BENCH_TRANSPORTS=tcp \
  BENCH_MSG_SIZES=64,256,1024,65536,131072,262144 ZMQ_ASIO_TCP_MAX_TRANSFER=262144 \
  ZMQ_ASIO_IN_BATCH_SIZE=65536 ZMQ_ASIO_OUT_BATCH_SIZE=65536 \
  python3 benchwithzmq/run_comparison.py ROUTER_ROUTER --runs=3 --build-dir build/bin

BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 BENCH_TRANSPORTS=tcp \
  BENCH_MSG_SIZES=64,256,1024,65536,131072,262144 ZMQ_ASIO_TCP_MAX_TRANSFER=262144 \
  python3 benchwithzmq/run_comparison.py ROUTER_ROUTER_POLL --runs=3 --refresh-libzmq --build-dir build/bin

BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 BENCH_TRANSPORTS=tcp \
  BENCH_MSG_SIZES=64,256,1024,65536,131072,262144 ZMQ_ASIO_TCP_MAX_TRANSFER=262144 \
  ZMQ_ASIO_IN_BATCH_SIZE=65536 ZMQ_ASIO_OUT_BATCH_SIZE=65536 \
  python3 benchwithzmq/run_comparison.py ROUTER_ROUTER_POLL --runs=3 --build-dir build/bin
```
