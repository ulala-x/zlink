# TCP Max Transfer Size (async_write)

## Setup

- BENCH_NO_TASKSET=1
- BENCH_IO_THREADS=2
- ZMQ_ASIO_TCP_MAX_TRANSFER=262144
- build dir: build/bin

## Commands

```
BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 BENCH_MSG_COUNT=2000 \
  ZMQ_ASIO_TCP_MAX_TRANSFER=262144 LD_LIBRARY_PATH=build/lib \
  strace -f -c -o /tmp/strace_pubsub_zlink_unpinned_262144_max262144.txt \
  ./build/bin/comp_zlink_pubsub zlink tcp 262144

BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 BENCH_MSG_COUNT=2000 \
  ZMQ_ASIO_TCP_STATS=1 ZMQ_ASIO_TCP_MAX_TRANSFER=262144 \
  LD_LIBRARY_PATH=build/lib ./build/bin/comp_zlink_pubsub zlink tcp 262144

BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 BENCH_TRANSPORTS=tcp \
  BENCH_MSG_SIZES=1024,262144 ZMQ_ASIO_TCP_MAX_TRANSFER=262144 \
  ./benchwithzmq/run_comparison.py PUBSUB --runs=3 --refresh-libzmq --build-dir build/bin

BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 BENCH_TRANSPORTS=tcp \
  BENCH_MSG_SIZES=1024,262144 ZMQ_ASIO_TCP_MAX_TRANSFER=262144 \
  ./benchwithzmq/run_comparison.py ROUTER_ROUTER --runs=3 --refresh-libzmq --build-dir build/bin
```

## Results (strace -f -c)

- zlink PUBSUB tcp 262144:
  - throughput 1997.95, latency 500.51 us
  - sendto 6010, recvfrom 7015, epoll_wait 7051, poll 3002, futex 1547

## Results (ZMQ_ASIO_TCP_STATS)

- PUBSUB tcp 262144:
  - throughput 22634.78, latency 44.18 us
  - async_read calls=6009 bytes=786,459,194 errors=2
  - async_write calls=6008 bytes=786,459,194 errors=0
  - read_some/write_some calls=0

## Results (run_comparison, tcp only)

- PUBSUB tcp
  - 1024B: -9.94% throughput
  - 262144B: -14.21% throughput
- ROUTER_ROUTER tcp
  - 1024B: -15.97% throughput
  - 262144B: -11.02% throughput

## Notes

- sendto/recvfrom 호출 수가 기존 15k 수준에서 6~7k로 감소.
- tcp 262144 성능 저하는 -20%대에서 -11%~-14%로 완화됨.
- 1024B 구간은 개선폭이 제한적이며 여전히 음수.
