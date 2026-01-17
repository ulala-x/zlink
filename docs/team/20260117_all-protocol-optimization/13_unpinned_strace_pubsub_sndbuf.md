# Unpinned strace: PUBSUB tcp 262144 (SNDBUF/RCVBUF 4MB)

## Setup

- BENCH_NO_TASKSET=1
- BENCH_IO_THREADS=2
- BENCH_MSG_COUNT=2000
- BENCH_SNDBUF=4194304, BENCH_RCVBUF=4194304
- build dir: build/bin
- zlink: LD_LIBRARY_PATH=build/lib
- libzmq: LD_LIBRARY_PATH=benchwithzmq/libzmq/libzmq_dist/lib

## Commands

```
BENCH_MSG_COUNT=2000 BENCH_IO_THREADS=2 BENCH_SNDBUF=4194304 \
  BENCH_RCVBUF=4194304 LD_LIBRARY_PATH=build/lib \
  strace -f -c -o /tmp/strace_pubsub_zlink_unpinned_262144_4mb.txt \
  ./build/bin/comp_zlink_pubsub zlink tcp 262144

BENCH_MSG_COUNT=2000 BENCH_IO_THREADS=2 BENCH_SNDBUF=4194304 \
  BENCH_RCVBUF=4194304 LD_LIBRARY_PATH=benchwithzmq/libzmq/libzmq_dist/lib \
  strace -f -c -o /tmp/strace_pubsub_libzmq_unpinned_262144_4mb.txt \
  ./build/bin/comp_std_zmq_pubsub libzmq tcp 262144
```

## Results (throughput/latency)

- zlink: throughput 1093.95, latency 914.12 us
- libzmq: throughput 2851.47, latency 350.70 us

## Results (strace -f -c summary)

- zlink: sendto 15010, recvfrom 16013, epoll_wait 16051, poll 3003, futex 670
- libzmq: sendto 6298, recvfrom 7182, epoll_wait 14485, poll 8135, futex 597

## Notes

- SNDBUF/RCVBUF 4MB로 설정해도 sendto/recvfrom 호출 수는 기존과 유사.
- syscall 차이는 버퍼 크기보다 async_write 내부 분할 영향일 가능성이 큼.
