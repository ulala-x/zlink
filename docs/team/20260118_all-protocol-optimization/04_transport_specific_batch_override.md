# Transport-specific batch size override

## Goal

Allow IPC to use larger batching without impacting TCP.

## Change

- Added transport-specific overrides for ASIO batch sizes (ZMTP engines).
- New environment variables:
  - `ZMQ_ASIO_IN_BATCH_SIZE_TCP`
  - `ZMQ_ASIO_OUT_BATCH_SIZE_TCP`
  - `ZMQ_ASIO_IN_BATCH_SIZE_IPC`
  - `ZMQ_ASIO_OUT_BATCH_SIZE_IPC`
  - `ZMQ_ASIO_IN_BATCH_SIZE_INPROC`
  - `ZMQ_ASIO_OUT_BATCH_SIZE_INPROC`
- If a per-transport override is set, it wins over the global
  `ZMQ_ASIO_IN_BATCH_SIZE` / `ZMQ_ASIO_OUT_BATCH_SIZE` values.
- Transport detection is based on the endpoint scheme:
  - `tcp`, `tls`, `ws`, `wss` -> TCP bucket (for ASIO ZMTP engine usage)
  - `ipc` -> IPC bucket
  - `inproc` -> INPROC bucket

Note: WebSocket engines use a separate ASIO engine class and are not affected
by these overrides yet.

## Targeted verification

### TCP remains unchanged when IPC override is set

Command:

```bash
BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 BENCH_TRANSPORTS=tcp \
  BENCH_MSG_SIZES=1024,262144 ZMQ_ASIO_TCP_MAX_TRANSFER=262144 \
  ZMQ_ASIO_IN_BATCH_SIZE_IPC=65536 ZMQ_ASIO_OUT_BATCH_SIZE_IPC=65536 \
  python3 benchwithzmq/run_comparison.py PUBSUB --runs=3 --build-dir build/bin
```

Result (PUBSUB, tcp):

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 1024B | Throughput | 1.12 M/s | 1.02 M/s | -9.36% |
| 1024B | Latency | 0.79 us | 0.98 us | -24.05% (inv) |
| 262144B | Throughput | 0.03 M/s | 0.02 M/s | -17.70% |
| 262144B | Latency | 37.09 us | 45.06 us | -21.49% (inv) |

### IPC uses 64 KB override

Command:

```bash
BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 BENCH_TRANSPORTS=ipc \
  BENCH_MSG_SIZES=1024,262144 \
  ZMQ_ASIO_IN_BATCH_SIZE_IPC=65536 ZMQ_ASIO_OUT_BATCH_SIZE_IPC=65536 \
  python3 benchwithzmq/run_comparison.py PUBSUB --runs=3 --build-dir build/bin
```

Result (PUBSUB, ipc):

| Size | Metric | libzmq | zlink | Diff |
| --- | --- | --- | --- | --- |
| 1024B | Throughput | 0.98 M/s | 1.35 M/s | +37.18% |
| 1024B | Latency | 1.02 us | 0.74 us | +27.45% (inv) |
| 262144B | Throughput | 0.02 M/s | 0.02 M/s | +13.33% |
| 262144B | Latency | 60.80 us | 53.65 us | +11.76% (inv) |
