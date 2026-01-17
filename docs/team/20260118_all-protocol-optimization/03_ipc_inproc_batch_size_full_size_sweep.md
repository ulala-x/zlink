# IPC/INPROC batch size full-size sweep

## Setup

- Build: `cmake --build build`
- Unpinned: `BENCH_NO_TASKSET=1`
- IO threads: `BENCH_IO_THREADS=2`
- Message sizes: `BENCH_MSG_SIZES=64,256,1024,65536,131072,262144`
- Batch override: `ZMQ_ASIO_IN_BATCH_SIZE=65536`, `ZMQ_ASIO_OUT_BATCH_SIZE=65536`

## IPC

### PUBSUB baseline (no batch override)

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 64B | Throughput | 4.99 M/s | 5.47 M/s | +9.58% |
| 64B | Latency | 0.20 us | 0.18 us | +10.00% (inv) |
| 256B | Throughput | 3.06 M/s | 2.60 M/s | -15.11% |
| 256B | Latency | 0.33 us | 0.38 us | -15.15% (inv) |
| 1024B | Throughput | 0.97 M/s | 1.09 M/s | +12.26% |
| 1024B | Latency | 1.03 us | 0.92 us | +10.68% (inv) |
| 65536B | Throughput | 0.02 M/s | 0.06 M/s | +194.92% |
| 65536B | Latency | 46.40 us | 15.73 us | +66.10% (inv) |
| 131072B | Throughput | 0.02 M/s | 0.04 M/s | +111.25% |
| 131072B | Latency | 52.75 us | 24.97 us | +52.66% (inv) |
| 262144B | Throughput | 0.02 M/s | 0.02 M/s | +24.72% |
| 262144B | Latency | 63.96 us | 51.28 us | +19.82% (inv) |

### PUBSUB 64 KB batch override

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 64B | Throughput | 4.99 M/s | 5.91 M/s | +18.47% |
| 64B | Latency | 0.20 us | 0.17 us | +15.00% (inv) |
| 256B | Throughput | 3.06 M/s | 3.65 M/s | +19.28% |
| 256B | Latency | 0.33 us | 0.27 us | +18.18% (inv) |
| 1024B | Throughput | 0.97 M/s | 1.49 M/s | +54.06% |
| 1024B | Latency | 1.03 us | 0.67 us | +34.95% (inv) |
| 65536B | Throughput | 0.02 M/s | 0.06 M/s | +186.27% |
| 65536B | Latency | 46.40 us | 15.02 us | +67.63% (inv) |
| 131072B | Throughput | 0.02 M/s | 0.04 M/s | +86.04% |
| 131072B | Latency | 52.75 us | 28.35 us | +46.26% (inv) |
| 262144B | Throughput | 0.02 M/s | 0.02 M/s | +18.08% |
| 262144B | Latency | 63.96 us | 54.16 us | +15.32% (inv) |

### ROUTER_ROUTER baseline (no batch override)

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 64B | Throughput | 4.35 M/s | 5.51 M/s | +26.53% |
| 64B | Latency | 29.00 us | 24.67 us | +14.93% (inv) |
| 256B | Throughput | 2.78 M/s | 2.46 M/s | -11.67% |
| 256B | Latency | 27.06 us | 20.73 us | +23.39% (inv) |
| 1024B | Throughput | 0.93 M/s | 1.09 M/s | +17.37% |
| 1024B | Latency | 28.29 us | 20.94 us | +25.98% (inv) |
| 65536B | Throughput | 0.02 M/s | 0.07 M/s | +218.68% |
| 65536B | Latency | 43.70 us | 34.98 us | +19.95% (inv) |
| 131072B | Throughput | 0.02 M/s | 0.04 M/s | +100.32% |
| 131072B | Latency | 47.84 us | 48.29 us | -0.94% (inv) |
| 262144B | Throughput | 0.02 M/s | 0.02 M/s | +31.30% |
| 262144B | Latency | 64.86 us | 82.14 us | -26.64% (inv) |

### ROUTER_ROUTER 64 KB batch override

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 64B | Throughput | 4.35 M/s | 5.41 M/s | +24.35% |
| 64B | Latency | 29.00 us | 21.38 us | +26.28% (inv) |
| 256B | Throughput | 2.78 M/s | 3.44 M/s | +23.60% |
| 256B | Latency | 27.06 us | 19.56 us | +27.72% (inv) |
| 1024B | Throughput | 0.93 M/s | 1.53 M/s | +63.58% |
| 1024B | Latency | 28.29 us | 19.51 us | +31.04% (inv) |
| 65536B | Throughput | 0.02 M/s | 0.07 M/s | +209.05% |
| 65536B | Latency | 43.70 us | 41.21 us | +5.70% (inv) |
| 131072B | Throughput | 0.02 M/s | 0.03 M/s | +49.74% |
| 131072B | Latency | 47.84 us | 56.37 us | -17.83% (inv) |
| 262144B | Throughput | 0.02 M/s | 0.02 M/s | +14.33% |
| 262144B | Latency | 64.86 us | 83.49 us | -28.72% (inv) |

### ROUTER_ROUTER_POLL baseline (no batch override)

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 64B | Throughput | 4.13 M/s | 5.71 M/s | +38.44% |
| 64B | Latency | 26.57 us | 21.77 us | +18.07% (inv) |
| 256B | Throughput | 2.62 M/s | 2.44 M/s | -6.83% |
| 256B | Latency | 29.72 us | 20.31 us | +31.66% (inv) |
| 1024B | Throughput | 0.97 M/s | 1.01 M/s | +4.28% |
| 1024B | Latency | 28.16 us | 21.69 us | +22.98% (inv) |
| 65536B | Throughput | 0.02 M/s | 0.06 M/s | +241.00% |
| 65536B | Latency | 42.19 us | 46.13 us | -9.34% (inv) |
| 131072B | Throughput | 0.02 M/s | 0.04 M/s | +94.85% |
| 131072B | Latency | 46.04 us | 49.96 us | -8.51% (inv) |
| 262144B | Throughput | 0.01 M/s | 0.02 M/s | +57.88% |
| 262144B | Latency | 66.61 us | 77.67 us | -16.60% (inv) |

### ROUTER_ROUTER_POLL 64 KB batch override

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 64B | Throughput | 4.13 M/s | 5.28 M/s | +27.94% |
| 64B | Latency | 26.57 us | 21.07 us | +20.70% (inv) |
| 256B | Throughput | 2.62 M/s | 3.43 M/s | +30.77% |
| 256B | Latency | 29.72 us | 20.59 us | +30.72% (inv) |
| 1024B | Throughput | 0.97 M/s | 1.52 M/s | +55.98% |
| 1024B | Latency | 28.16 us | 21.35 us | +24.18% (inv) |
| 65536B | Throughput | 0.02 M/s | 0.06 M/s | +216.11% |
| 65536B | Latency | 42.19 us | 40.79 us | +3.32% (inv) |
| 131072B | Throughput | 0.02 M/s | 0.03 M/s | +55.53% |
| 131072B | Latency | 46.04 us | 53.25 us | -15.66% (inv) |
| 262144B | Throughput | 0.01 M/s | 0.02 M/s | +31.51% |
| 262144B | Latency | 66.61 us | 90.76 us | -36.26% (inv) |

## INPROC

### PUBSUB baseline (no batch override)

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 64B | Throughput | 5.80 M/s | 5.43 M/s | -6.41% |
| 64B | Latency | 0.17 us | 0.18 us | -5.88% (inv) |
| 256B | Throughput | 4.76 M/s | 4.72 M/s | -0.94% |
| 256B | Latency | 0.21 us | 0.21 us | +0.00% (inv) |
| 1024B | Throughput | 2.38 M/s | 2.18 M/s | -8.61% |
| 1024B | Latency | 0.42 us | 0.46 us | -9.52% (inv) |
| 65536B | Throughput | 0.16 M/s | 0.16 M/s | +1.56% |
| 65536B | Latency | 6.22 us | 6.12 us | +1.61% (inv) |
| 131072B | Throughput | 0.10 M/s | 0.09 M/s | -4.70% |
| 131072B | Latency | 10.36 us | 10.87 us | -4.92% (inv) |
| 262144B | Throughput | 0.04 M/s | 0.05 M/s | +5.11% |
| 262144B | Latency | 23.05 us | 21.93 us | +4.86% (inv) |

### PUBSUB 64 KB batch override

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 64B | Throughput | 5.80 M/s | 5.59 M/s | -3.51% |
| 64B | Latency | 0.17 us | 0.18 us | -5.88% (inv) |
| 256B | Throughput | 4.76 M/s | 4.69 M/s | -1.45% |
| 256B | Latency | 0.21 us | 0.21 us | +0.00% (inv) |
| 1024B | Throughput | 2.38 M/s | 2.22 M/s | -6.83% |
| 1024B | Latency | 0.42 us | 0.45 us | -7.14% (inv) |
| 65536B | Throughput | 0.16 M/s | 0.17 M/s | +7.70% |
| 65536B | Latency | 6.22 us | 5.77 us | +7.23% (inv) |
| 131072B | Throughput | 0.10 M/s | 0.09 M/s | -4.36% |
| 131072B | Latency | 10.36 us | 10.83 us | -4.54% (inv) |
| 262144B | Throughput | 0.04 M/s | 0.04 M/s | +3.56% |
| 262144B | Latency | 23.05 us | 22.25 us | +3.47% (inv) |

### ROUTER_ROUTER baseline (no batch override)

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 64B | Throughput | 4.04 M/s | 4.63 M/s | +14.68% |
| 64B | Latency | 0.16 us | 0.17 us | -6.25% (inv) |
| 256B | Throughput | 3.28 M/s | 3.34 M/s | +1.64% |
| 256B | Latency | 0.16 us | 0.16 us | +0.00% (inv) |
| 1024B | Throughput | 1.84 M/s | 1.81 M/s | -1.72% |
| 1024B | Latency | 0.19 us | 0.19 us | +0.00% (inv) |
| 65536B | Throughput | 0.18 M/s | 0.16 M/s | -12.44% |
| 65536B | Latency | 2.02 us | 2.09 us | -3.47% (inv) |
| 131072B | Throughput | 0.10 M/s | 0.09 M/s | -16.70% |
| 131072B | Latency | 3.74 us | 3.80 us | -1.60% (inv) |
| 262144B | Throughput | 0.05 M/s | 0.04 M/s | -16.44% |
| 262144B | Latency | 7.10 us | 7.02 us | +1.13% (inv) |

### ROUTER_ROUTER 64 KB batch override

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 64B | Throughput | 4.04 M/s | 4.74 M/s | +17.48% |
| 64B | Latency | 0.16 us | 0.17 us | -6.25% (inv) |
| 256B | Throughput | 3.28 M/s | 3.31 M/s | +0.79% |
| 256B | Latency | 0.16 us | 0.16 us | +0.00% (inv) |
| 1024B | Throughput | 1.84 M/s | 1.85 M/s | +0.65% |
| 1024B | Latency | 0.19 us | 0.18 us | +5.26% (inv) |
| 65536B | Throughput | 0.18 M/s | 0.17 M/s | -7.90% |
| 65536B | Latency | 2.02 us | 2.04 us | -0.99% (inv) |
| 131072B | Throughput | 0.10 M/s | 0.08 M/s | -23.14% |
| 131072B | Latency | 3.74 us | 3.81 us | -1.87% (inv) |
| 262144B | Throughput | 0.05 M/s | 0.05 M/s | -11.37% |
| 262144B | Latency | 7.10 us | 6.98 us | +1.69% (inv) |

### ROUTER_ROUTER_POLL baseline (no batch override)

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 64B | Throughput | 4.09 M/s | 3.70 M/s | -9.54% |
| 64B | Latency | 0.53 us | 0.46 us | +13.21% (inv) |
| 256B | Throughput | 3.31 M/s | 3.25 M/s | -1.89% |
| 256B | Latency | 0.53 us | 0.45 us | +15.09% (inv) |
| 1024B | Throughput | 1.88 M/s | 1.74 M/s | -7.20% |
| 1024B | Latency | 0.56 us | 0.49 us | +12.50% (inv) |
| 65536B | Throughput | 0.17 M/s | 0.21 M/s | +24.44% |
| 65536B | Latency | 2.47 us | 2.36 us | +4.45% (inv) |
| 131072B | Throughput | 0.10 M/s | 0.07 M/s | -27.08% |
| 131072B | Latency | 4.17 us | 4.14 us | +0.72% (inv) |
| 262144B | Throughput | 0.05 M/s | 0.05 M/s | -15.09% |
| 262144B | Latency | 7.62 us | 7.44 us | +2.36% (inv) |

### ROUTER_ROUTER_POLL 64 KB batch override

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 64B | Throughput | 4.09 M/s | 3.92 M/s | -4.19% |
| 64B | Latency | 0.53 us | 0.46 us | +13.21% (inv) |
| 256B | Throughput | 3.31 M/s | 3.35 M/s | +1.11% |
| 256B | Latency | 0.53 us | 0.46 us | +13.21% (inv) |
| 1024B | Throughput | 1.88 M/s | 1.84 M/s | -2.22% |
| 1024B | Latency | 0.56 us | 0.49 us | +12.50% (inv) |
| 65536B | Throughput | 0.17 M/s | 0.18 M/s | +8.46% |
| 65536B | Latency | 2.47 us | 2.38 us | +3.64% (inv) |
| 131072B | Throughput | 0.10 M/s | 0.08 M/s | -24.74% |
| 131072B | Latency | 4.17 us | 4.11 us | +1.44% (inv) |
| 262144B | Throughput | 0.05 M/s | 0.05 M/s | -13.42% |
| 262144B | Latency | 7.62 us | 7.46 us | +2.10% (inv) |

## Observations

- IPC: zlink already leads on large sizes; 64 KB batching boosts small/mid throughput but sometimes hurts large latency.
- INPROC: batch override has limited impact; results remain mixed for small sizes.

## Commands

```bash
BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 BENCH_TRANSPORTS=ipc \
  BENCH_MSG_SIZES=64,256,1024,65536,131072,262144 \
  python3 benchwithzmq/run_comparison.py PUBSUB --runs=3 --refresh-libzmq --build-dir build/bin

BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 BENCH_TRANSPORTS=ipc \
  BENCH_MSG_SIZES=64,256,1024,65536,131072,262144 \
  ZMQ_ASIO_IN_BATCH_SIZE=65536 ZMQ_ASIO_OUT_BATCH_SIZE=65536 \
  python3 benchwithzmq/run_comparison.py PUBSUB --runs=3 --build-dir build/bin

BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 BENCH_TRANSPORTS=inproc \
  BENCH_MSG_SIZES=64,256,1024,65536,131072,262144 \
  python3 benchwithzmq/run_comparison.py PUBSUB --runs=3 --refresh-libzmq --build-dir build/bin

BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 BENCH_TRANSPORTS=inproc \
  BENCH_MSG_SIZES=64,256,1024,65536,131072,262144 \
  ZMQ_ASIO_IN_BATCH_SIZE=65536 ZMQ_ASIO_OUT_BATCH_SIZE=65536 \
  python3 benchwithzmq/run_comparison.py PUBSUB --runs=3 --build-dir build/bin
```
