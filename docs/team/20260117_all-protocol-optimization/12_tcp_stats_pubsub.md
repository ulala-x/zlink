# TCP Stats: PUBSUB tcp (ZMQ_ASIO_TCP_STATS)

## Setup

- BENCH_NO_TASKSET=1
- BENCH_IO_THREADS=2
- zlink only (ZMQ_ASIO_TCP_STATS=1)
- build dir: build/bin

## Commands

```
BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 ZMQ_ASIO_TCP_STATS=1 \
  LD_LIBRARY_PATH=build/lib ./build/bin/comp_zlink_pubsub zlink tcp 1024

BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 BENCH_MSG_COUNT=2000 \
  ZMQ_ASIO_TCP_STATS=1 LD_LIBRARY_PATH=build/lib \
  ./build/bin/comp_zlink_pubsub zlink tcp 262144

BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 BENCH_MSG_COUNT=2000 \
  ZMQ_ASIO_TCP_STATS=1 ZMQ_ASIO_TCP_SYNC_WRITE=1 LD_LIBRARY_PATH=build/lib \
  ./build/bin/comp_zlink_pubsub zlink tcp 262144
```

## Results (default, 1024B)

- throughput 1,006,668.73, latency 0.99 us
- async_read calls=26227 bytes=207,633,194 errors=2
- async_write calls=26228 bytes=207,633,194 errors=0
- read_some/write_some calls=0

## Results (default, 262144B)

- throughput 21,182.47, latency 47.21 us
- async_read calls=15007 bytes=786,459,194 errors=2
- async_write calls=6008 bytes=786,459,194 errors=0
- read_some/write_some calls=0

## Results (ZMQ_ASIO_TCP_SYNC_WRITE=1, 262144B)

- throughput 20,553.93, latency 48.65 us
- async_read calls=11932 bytes=786,459,194 errors=2
- async_write calls=3974 bytes=519,842,064 errors=0
- write_some calls=2035 bytes=266,617,130 eagain=0 errors=0

## Notes

- 262144B에서 async_write 호출 수는 메시지당 약 3회 수준.
- sendto/recvfrom syscall 수는 여전히 더 많아, async_write 내부 분할에 의한
  syscall 증가 가능성이 큼.
