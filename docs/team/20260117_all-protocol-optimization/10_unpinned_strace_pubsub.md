# Unpinned strace: PUBSUB tcp 1024/262144

## Setup

- BENCH_IO_THREADS=2
- BENCH_MSG_COUNT=2000
- no taskset (direct run)
- build dir: build/bin
- zlink: LD_LIBRARY_PATH=build/lib
- libzmq: LD_LIBRARY_PATH=benchwithzmq/libzmq/libzmq_dist/lib

## Commands

```
BENCH_MSG_COUNT=2000 BENCH_IO_THREADS=2 LD_LIBRARY_PATH=build/lib \
  strace -f -c -o /tmp/strace_pubsub_zlink_unpinned_1024.txt \
  ./build/bin/comp_zlink_pubsub zlink tcp 1024
BENCH_MSG_COUNT=2000 BENCH_IO_THREADS=2 \
  LD_LIBRARY_PATH=benchwithzmq/libzmq/libzmq_dist/lib \
  strace -f -c -o /tmp/strace_pubsub_libzmq_unpinned_1024.txt \
  ./build/bin/comp_std_zmq_pubsub libzmq tcp 1024

BENCH_MSG_COUNT=2000 BENCH_IO_THREADS=2 LD_LIBRARY_PATH=build/lib \
  strace -f -c -o /tmp/strace_pubsub_zlink_unpinned_262144.txt \
  ./build/bin/comp_zlink_pubsub zlink tcp 262144
BENCH_MSG_COUNT=2000 BENCH_IO_THREADS=2 \
  LD_LIBRARY_PATH=benchwithzmq/libzmq/libzmq_dist/lib \
  strace -f -c -o /tmp/strace_pubsub_libzmq_unpinned_262144.txt \
  ./build/bin/comp_std_zmq_pubsub libzmq tcp 262144
```

## Results (throughput/latency)

- 1024B
  - zlink: throughput 18602.58, latency 53.76 us
  - libzmq: throughput 34070.74, latency 29.35 us
- 262144B
  - zlink: throughput 1056.00, latency 946.97 us
  - libzmq: throughput 3337.66, latency 299.61 us

## Results (strace -f -c summary)

- 1024B (zlink)
  - sendto 1263, recvfrom 2266, epoll_wait 2296, poll 1249, futex 1210
- 1024B (libzmq)
  - sendto 1264, recvfrom 1270, epoll_wait 3539, poll 5242, futex 51
- 262144B (zlink)
  - sendto 15010, recvfrom 16013, epoll_wait 16050, poll 3003, futex 635
- 262144B (libzmq)
  - sendto 6010, recvfrom 6018, epoll_wait 13036, poll 7923, futex 806

## Notes

- unpinned 환경에서도 zlink의 sendto/recvfrom 호출 수는 libzmq 대비 2~3배 수준.
- 1024B에서 zlink의 futex 호출이 크게 높음.
