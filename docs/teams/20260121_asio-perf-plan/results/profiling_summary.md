# Profiling summary (strace -c -f)

WSL2 perf unavailable; using strace summaries.
valgrind not installed; callgrind profiling skipped.

## pair_tcp_64
- 1. futex: 53.60% (1483 calls, errors 493)
- 2. poll: 13.41% (3595 calls)
- 3. epoll_wait: 9.25% (5805 calls)
- 4. getpid: 7.98% (12810 calls)
- 5. read: 4.95% (9024 calls, errors 3407)

## pubsub_tcp_65536
- 1. futex: 57.25% (3011 calls, errors 714)
- 2. poll: 17.20% (20999 calls)
- 3. sendto: 6.46% (42005 calls)
- 4. getpid: 5.59% (64020 calls)
- 5. recvfrom: 4.44% (43009 calls, errors 1005)

## router_router_ipc_65536
- 1. futex: 57.15% (2912 calls, errors 638)
- 2. poll: 17.14% (20992 calls)
- 3. getpid: 5.76% (64018 calls)
- 4. sendto: 5.52% (44005 calls)
- 5. recvfrom: 5.27% (46013 calls, errors 2007)
