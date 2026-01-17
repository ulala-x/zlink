# Progress Log

## Phase 1: Kickoff

### Goal

- all-protocol 성능 개선 문서화 시작.

### Actions

1. `docs/team/20260117_all-protocol-optimization/` 생성.
2. baseline/analysis 템플릿 추가.
3. main에 병합된 inproc 결과와 native 옵션을 스코프에 반영.

### Status

- baseline 수집 대기.

## Phase 2: Full baseline 1-run 시도 (Timeout)

### Goal

- 전체 프로토콜 1-run baseline 확보.

### Bench

```
BENCH_TRANSPORTS=inproc,tcp,ipc BENCH_MSG_SIZES=64,256,1024,65536,131072,262144 \
  ./benchwithzmq/run_comparison.py ALL --runs 1 --refresh-libzmq
```

### Result

- 전체 실행이 180s 타임아웃으로 중단됨.
- PAIR 진행 중 ipc 구간에서 zlink 실패가 발생했음 (ipc 64B/256B 실패 표시).

### Status

- 패턴별로 분리 실행 필요.
- ipc 실패 원인부터 확인 예정.

## Phase 3: ipc hang 원인 확인 (build/bench bin)

### Findings

- `run_comparison.py` 기본 경로(`build/bench/bin`)의 zlink ipc 실행이 hang 발생.
- 동일 바이너리를 `build/bin`으로 실행하면 정상 동작.

### Working Command

```
BENCH_TRANSPORTS=ipc BENCH_MSG_SIZES=64 \
  ./benchwithzmq/run_comparison.py PAIR --runs 1 --zlink-only --build-dir build/bin
```

### Status

- baseline 실행 시 `--build-dir build/bin` 강제 적용 필요.

## Phase 4: Baseline 1-run (all patterns, build/bin)

### Goal

- 전체 프로토콜 baseline 확보.

### Bench

```
BENCH_TRANSPORTS=inproc,tcp,ipc BENCH_MSG_SIZES=64,256,1024,65536,131072,262144 \
  ./benchwithzmq/run_comparison.py <PATTERN> --runs 1 --refresh-libzmq --build-dir build/bin
```

### Notes

- 상세 결과는 `01_current_baseline.md`에 기록.
- PAIR tcp 65536B latency가 음수로 출력되는 이상치 존재.

### Status

- baseline 수집 완료 (1-run).
- 저하 후보: tcp/large 및 ipc/large 일부 구간, ROUTER_ROUTER 256K.

## Phase 5: tcp large-size syscall 비교 (PUBSUB 262144)

### Goal

- tcp 262144 저하 원인 힌트 확보.

### Bench

```
BENCH_MSG_COUNT=2000 strace -f -c ./build/bin/comp_zlink_pubsub zlink tcp 262144
BENCH_MSG_COUNT=2000 LD_LIBRARY_PATH=benchwithzmq/libzmq/libzmq_dist/lib \\
  strace -f -c ./build/bin/comp_std_zmq_pubsub libzmq tcp 262144
```

### Results (strace -f -c 요약)

- zlink throughput 1041.42, latency 960.23 us
  - futex 672, poll 3003, sendto 15010, recvfrom 16013,
    epoll_wait 16050, read 5415 (1390 errors), write 4020
- libzmq throughput 2166.82, latency 461.51 us
  - epoll_wait 7022, poll 9364, futex 409, sendto 6011,
    recvfrom 6019, read 4015, write 4012

### Status

- zlink는 sendto/recvfrom 호출 수가 2~3배 높음.
- tcp large-size에서 syscall per message 증가 가능성 확인.

## Phase 6: in/out batch size 확장 실험 (8192 -> 65536)

### Goal

- tcp large-size syscall 감소/throughput 개선 여부 확인.

### Change

- `src/options.cpp` 기본값 변경:
  - `in_batch_size`: 8192 -> 65536
  - `out_batch_size`: 8192 -> 65536

### Bench (1-run, large sizes only)

```
BENCH_MSG_COUNT=2000 BENCH_TRANSPORTS=tcp BENCH_MSG_SIZES=131072,262144 \
  ./benchwithzmq/run_comparison.py PUBSUB --runs 1 --refresh-libzmq --build-dir build/bin
BENCH_MSG_COUNT=2000 BENCH_TRANSPORTS=tcp BENCH_MSG_SIZES=131072,262144 \
  ./benchwithzmq/run_comparison.py ROUTER_ROUTER --runs 1 --refresh-libzmq --build-dir build/bin
BENCH_MSG_COUNT=2000 BENCH_TRANSPORTS=tcp BENCH_MSG_SIZES=131072,262144 \
  ./benchwithzmq/run_comparison.py ROUTER_ROUTER_POLL --runs 1 --refresh-libzmq --build-dir build/bin

BENCH_MSG_COUNT=2000 BENCH_TRANSPORTS=ipc BENCH_MSG_SIZES=131072,262144 \
  ./benchwithzmq/run_comparison.py PUBSUB --runs 1 --refresh-libzmq --build-dir build/bin
BENCH_MSG_COUNT=2000 BENCH_TRANSPORTS=ipc BENCH_MSG_SIZES=131072,262144 \
  ./benchwithzmq/run_comparison.py ROUTER_ROUTER --runs 1 --refresh-libzmq --build-dir build/bin
BENCH_MSG_COUNT=2000 BENCH_TRANSPORTS=ipc BENCH_MSG_SIZES=131072,262144 \
  ./benchwithzmq/run_comparison.py ROUTER_ROUTER_POLL --runs 1 --refresh-libzmq --build-dir build/bin
```

### Results (Diff %)

- tcp
  - PUBSUB 131072B: +13.53% throughput, 262144B: -23.66% throughput
  - ROUTER_ROUTER 131072B: +7.37% throughput, 262144B: -20.86% throughput
  - ROUTER_ROUTER_POLL 131072B: -10.67% throughput, 262144B: +0.72% throughput
- ipc
  - PUBSUB 131072B: -12.76% throughput, 262144B: -18.67% throughput
  - ROUTER_ROUTER 131072B: -23.38% throughput, 262144B: +0.53% throughput
  - ROUTER_ROUTER_POLL 131072B: -6.67% throughput, 262144B: -4.49% throughput

### Strace 재확인 (PUBSUB tcp 262144)

```
LD_LIBRARY_PATH=build/lib BENCH_MSG_COUNT=2000 \
  strace -f -c -o /tmp/strace_pubsub_zlink_batch.txt \
  ./build/bin/comp_zlink_pubsub zlink tcp 262144
LD_LIBRARY_PATH=benchwithzmq/libzmq/libzmq_dist/lib BENCH_MSG_COUNT=2000 \
  strace -f -c -o /tmp/strace_pubsub_libzmq_batch.txt \
  ./build/bin/comp_std_zmq_pubsub libzmq tcp 262144
```

- zlink syscall counts (batch 65536): sendto 15010, recvfrom 16013, epoll_wait 16053, futex 957
- libzmq syscall counts: sendto 6011, recvfrom 6019, epoll_wait 7022, futex 436

### Status

- syscall 호출 수가 이전과 유사하여 개선 효과 확인되지 않음.
- tcp 262144 개선 폭이 제한적이고 ipc 일부 구간은 악화.
- 기본값 8192로 되돌림.

## Phase 7: TCP sync write 실험 (ZMQ_ASIO_TCP_SYNC_WRITE=1)

### Goal

- sendto/recvfrom 호출 수 감소 및 tcp large-size 개선 여부 확인.

### Bench (1-run, large sizes)

```
ZMQ_ASIO_TCP_SYNC_WRITE=1 BENCH_MSG_COUNT=2000 BENCH_TRANSPORTS=tcp \
  BENCH_MSG_SIZES=131072,262144 \
  ./benchwithzmq/run_comparison.py PUBSUB --runs 1 --refresh-libzmq --build-dir build/bin
ZMQ_ASIO_TCP_SYNC_WRITE=1 BENCH_MSG_COUNT=2000 BENCH_TRANSPORTS=tcp \
  BENCH_MSG_SIZES=131072,262144 \
  ./benchwithzmq/run_comparison.py ROUTER_ROUTER --runs 1 --refresh-libzmq --build-dir build/bin
ZMQ_ASIO_TCP_SYNC_WRITE=1 BENCH_MSG_COUNT=2000 BENCH_TRANSPORTS=tcp \
  BENCH_MSG_SIZES=131072,262144 \
  ./benchwithzmq/run_comparison.py ROUTER_ROUTER_POLL --runs 1 --refresh-libzmq --build-dir build/bin
```

### 1-run Results (Diff %)

- PUBSUB 262144B: -20.03% throughput
- ROUTER_ROUTER 262144B: -3.74% throughput
- ROUTER_ROUTER_POLL 262144B: -13.20% throughput

### Bench (3-run, tcp 262144 only)

```
BENCH_MSG_COUNT=2000 BENCH_TRANSPORTS=tcp BENCH_MSG_SIZES=262144 \
  ./benchwithzmq/run_comparison.py PUBSUB --runs 3 --refresh-libzmq --build-dir build/bin
ZMQ_ASIO_TCP_SYNC_WRITE=1 BENCH_MSG_COUNT=2000 BENCH_TRANSPORTS=tcp \
  BENCH_MSG_SIZES=262144 \
  ./benchwithzmq/run_comparison.py PUBSUB --runs 3 --refresh-libzmq --build-dir build/bin

BENCH_MSG_COUNT=2000 BENCH_TRANSPORTS=tcp BENCH_MSG_SIZES=262144 \
  ./benchwithzmq/run_comparison.py ROUTER_ROUTER --runs 3 --refresh-libzmq --build-dir build/bin
ZMQ_ASIO_TCP_SYNC_WRITE=1 BENCH_MSG_COUNT=2000 BENCH_TRANSPORTS=tcp \
  BENCH_MSG_SIZES=262144 \
  ./benchwithzmq/run_comparison.py ROUTER_ROUTER --runs 3 --refresh-libzmq --build-dir build/bin

BENCH_MSG_COUNT=2000 BENCH_TRANSPORTS=tcp BENCH_MSG_SIZES=262144 \
  ./benchwithzmq/run_comparison.py ROUTER_ROUTER_POLL --runs 3 --refresh-libzmq --build-dir build/bin
ZMQ_ASIO_TCP_SYNC_WRITE=1 BENCH_MSG_COUNT=2000 BENCH_TRANSPORTS=tcp \
  BENCH_MSG_SIZES=262144 \
  ./benchwithzmq/run_comparison.py ROUTER_ROUTER_POLL --runs 3 --refresh-libzmq --build-dir build/bin
```

### 3-run Results (Diff %, tcp 262144)

- PUBSUB: 기본 +0.75% throughput, sync write -4.68%
- ROUTER_ROUTER: 기본 -10.44%, sync write -9.84% (latency 개선)
- ROUTER_ROUTER_POLL: 기본 -16.05%, sync write -15.77%

### Strace (PUBSUB/ROUTER_ROUTER tcp 262144)

```
ZMQ_ASIO_TCP_SYNC_WRITE=1 LD_LIBRARY_PATH=build/lib BENCH_MSG_COUNT=2000 \
  strace -f -c -o /tmp/strace_pubsub_zlink_sync.txt \
  ./build/bin/comp_zlink_pubsub zlink tcp 262144
ZMQ_ASIO_TCP_SYNC_WRITE=1 LD_LIBRARY_PATH=build/lib BENCH_MSG_COUNT=2000 \
  strace -f -c -o /tmp/strace_router_router_zlink_sync.txt \
  ./build/bin/comp_zlink_router_router zlink tcp 262144
```

- PUBSUB: sendto 15010 -> 11982, recvfrom 16013 -> 12967 감소
- ROUTER_ROUTER: sendto 20012 -> 13984, recvfrom 22019 -> 15975 감소

### Status

- syscall 감소 효과는 확인되나 throughput 개선이 제한적.
- 기본 동작 변경 없이 실험 결과로만 기록.

## Phase 8: TCP socket buffer 실험 (BENCH_SNDBUF/RCVBUF)

### Goal

- 커널 send/recv buffer 확대로 tcp large-size 개선 여부 확인.

### Harness Change

- bench 프로그램에서 `BENCH_SNDBUF`, `BENCH_RCVBUF` 환경변수 지원 추가
  (양쪽 libzmq/zlink에 동일 적용).

### Bench (3-run, tcp 262144)

```
BENCH_SNDBUF=4194304 BENCH_RCVBUF=4194304 BENCH_MSG_COUNT=2000 \
  BENCH_TRANSPORTS=tcp BENCH_MSG_SIZES=262144 \
  ./benchwithzmq/run_comparison.py PUBSUB --runs 3 --refresh-libzmq --build-dir build/bin
BENCH_SNDBUF=4194304 BENCH_RCVBUF=4194304 BENCH_MSG_COUNT=2000 \
  BENCH_TRANSPORTS=tcp BENCH_MSG_SIZES=262144 \
  ./benchwithzmq/run_comparison.py ROUTER_ROUTER --runs 3 --refresh-libzmq --build-dir build/bin
BENCH_SNDBUF=4194304 BENCH_RCVBUF=4194304 BENCH_MSG_COUNT=2000 \
  BENCH_TRANSPORTS=tcp BENCH_MSG_SIZES=262144 \
  ./benchwithzmq/run_comparison.py ROUTER_ROUTER_POLL --runs 3 --refresh-libzmq --build-dir build/bin
```

### Results (Diff %, tcp 262144, 4MB)

- PUBSUB: -11.87% throughput
- ROUTER_ROUTER: -0.46% throughput
- ROUTER_ROUTER_POLL: +3.66% throughput

### Additional Check (1MB)

- PUBSUB: -20.93% throughput
- ROUTER_ROUTER: -21.03% throughput

### Status

- 큰 버퍼는 ROUTER 계열은 개선되지만 PUBSUB이 악화됨.
- 전체 패턴 공통 최적화로는 부적합, 원인 추가 분석 필요.

### Zlink-only Buffer Check (manual, 3x avg)

- zlink에만 4MB 적용, libzmq는 기본값 유지 시 상대 성능 악화
  - PUBSUB: -17.86% throughput
  - ROUTER_ROUTER: -18.50% throughput
  - ROUTER_ROUTER_POLL: -12.57% throughput

## Phase 9: ROUTER 기본 sndbuf/rcvbuf 상향 실험

### Goal

- ROUTER 계열 tcp 262144 성능 개선 (기본값 변경).

### Change

- `src/router.cpp`에서 ROUTER 소켓 기본 `sndbuf/rcvbuf` 상향(2MB/4MB 테스트).

### Results (tcp 262144, 3-run)

- 4MB
  - ROUTER_ROUTER: -2.79% throughput
  - ROUTER_ROUTER_POLL: +5.40% throughput
  - DEALER_ROUTER: -18.67% throughput (regression)
- 2MB
  - ROUTER_ROUTER: -3.22% throughput
  - ROUTER_ROUTER_POLL: +2.30% throughput
  - DEALER_ROUTER: -21.14% throughput (regression)

### Status

- ROUTER 계열 일부 개선되나 DEALER_ROUTER가 크게 악화됨.
- 기본값 변경은 보류하고 원상 복구 예정.

## Phase 10: TCP sync write 기본 활성화 실험 (DEALER/ROUTER 제한)

### Goal

- tcp 262144 구간(특히 DEALER_ROUTER) 개선.

### Change

- `ZMQ_ASIO_TCP_SYNC_WRITE` 기본값을 ON으로 변경 시도.
- `DEALER/ROUTER` 타입만 sync write 사용하도록 조건 추가.

### Results (tcp 262144, 3-run)

- ROUTER_ROUTER: -1.90% throughput
- ROUTER_ROUTER_POLL: +0.81% throughput
- DEALER_ROUTER: -10% ~ -30% 수준으로 변동 (안정적 개선 불가)
- PUBSUB: -7% ~ -18% 변동 (개선 확인 못함)

### Status

- 변동성이 크고 PUBSUB/DEALER_ROUTER 개선이 불확실.
- 기본 동작 변경은 보류하고 원상 복구 예정.

## Phase 11: DEALER_ROUTER syscall 비교 (tcp 262144)

### Goal

- DEALER_ROUTER tcp large-size 저하 원인 추가 확인.

### Bench

```
BENCH_MSG_COUNT=2000 LD_LIBRARY_PATH=build/lib \
  strace -f -c -o /tmp/strace_dealer_router_zlink.txt \
  ./build/bin/comp_zlink_dealer_router zlink tcp 262144
BENCH_MSG_COUNT=2000 LD_LIBRARY_PATH=benchwithzmq/libzmq/libzmq_dist/lib \
  strace -f -c -o /tmp/strace_dealer_router_libzmq.txt \
  ./build/bin/comp_std_zmq_dealer_router libzmq tcp 262144
```

### Results (strace summary)

- zlink: sendto 30009, recvfrom 34013, epoll_wait 34128, futex 1314
- libzmq: sendto 12010, recvfrom 12020, epoll_wait 16023, futex 389

### Status

- DEALER_ROUTER에서도 sendto/recvfrom 호출 수가 2~3배 높음.
- tcp large-size 공통 병목으로 지속 관찰 필요.

## Phase 12: Benchmark 안정화 체크 (msg_count=20000, runs=5)

### Goal

- libzmq cache refresh + 긴 실행으로 변동성 확인.

### Bench

```
BENCH_MSG_COUNT=20000 BENCH_TRANSPORTS=tcp BENCH_MSG_SIZES=262144 \
  ./benchwithzmq/run_comparison.py PUBSUB --runs 5 --refresh-libzmq --build-dir build/bin
BENCH_MSG_COUNT=20000 BENCH_TRANSPORTS=tcp BENCH_MSG_SIZES=262144 \
  ./benchwithzmq/run_comparison.py DEALER_ROUTER --runs 5 --refresh-libzmq --build-dir build/bin
BENCH_MSG_COUNT=20000 BENCH_TRANSPORTS=tcp BENCH_MSG_SIZES=262144 \
  ./benchwithzmq/run_comparison.py ROUTER_ROUTER --runs 5 --refresh-libzmq --build-dir build/bin
BENCH_MSG_COUNT=20000 BENCH_TRANSPORTS=tcp BENCH_MSG_SIZES=262144 \
  ./benchwithzmq/run_comparison.py ROUTER_ROUTER_POLL --runs 5 --refresh-libzmq --build-dir build/bin
```

### Results (Diff %, tcp 262144)

- PUBSUB: -4.89% throughput
- DEALER_ROUTER: -10.75% throughput
- ROUTER_ROUTER: +1.39% throughput
- ROUTER_ROUTER_POLL: +1.62% throughput

### Status

- 장시간/다회 실행 시 ROUTER 계열은 개선 또는 동급 수준.
- DEALER_ROUTER는 여전히 ~-10% 수준 유지.
