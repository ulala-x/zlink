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

## Phase 13: IO thread tuning (BENCH_IO_THREADS)

### Goal

- tcp 262144에서 zlink 상대 성능 개선 가능성 확인.

### Harness Change

- `BENCH_IO_THREADS` 환경변수로 `ZMQ_IO_THREADS` 설정 지원 추가.

### Bench (msg_count=20000, runs=5, tcp 262144)

```
BENCH_IO_THREADS=2 BENCH_MSG_COUNT=20000 BENCH_TRANSPORTS=tcp \
  BENCH_MSG_SIZES=262144 \
  ./benchwithzmq/run_comparison.py PUBSUB --runs 5 --refresh-libzmq --build-dir build/bin
BENCH_IO_THREADS=2 BENCH_MSG_COUNT=20000 BENCH_TRANSPORTS=tcp \
  BENCH_MSG_SIZES=262144 \
  ./benchwithzmq/run_comparison.py DEALER_ROUTER --runs 5 --refresh-libzmq --build-dir build/bin
BENCH_IO_THREADS=2 BENCH_MSG_COUNT=20000 BENCH_TRANSPORTS=tcp \
  BENCH_MSG_SIZES=262144 \
  ./benchwithzmq/run_comparison.py ROUTER_ROUTER --runs 5 --refresh-libzmq --build-dir build/bin
BENCH_IO_THREADS=2 BENCH_MSG_COUNT=20000 BENCH_TRANSPORTS=tcp \
  BENCH_MSG_SIZES=262144 \
  ./benchwithzmq/run_comparison.py ROUTER_ROUTER_POLL --runs 5 --refresh-libzmq --build-dir build/bin
```

### Results (Diff %, tcp 262144)

- PUBSUB: +21.02% throughput
- DEALER_ROUTER: +29.73% throughput
- ROUTER_ROUTER: +23.86% throughput
- ROUTER_ROUTER_POLL: +32.32% throughput

### 4-thread Check (DEALER_ROUTER)

- BENCH_IO_THREADS=4: +22.09% throughput

### Status

- IO thread 증가 시 zlink가 tcp 262144에서 크게 개선됨.
- default 변경 여부는 추가 검토 필요.

## Phase 14: IO thread=2 전체 매트릭스 재측정

### Goal

- 모든 프로토콜/사이즈에서 IO thread=2 기준 성능 확인.

### Bench

```
BENCH_IO_THREADS=2 BENCH_TRANSPORTS=inproc,tcp,ipc \
  BENCH_MSG_SIZES=64,256,1024,65536,131072,262144 \
  ./benchwithzmq/run_comparison.py <PATTERN> --runs 3 --refresh-libzmq --build-dir build/bin
```

### Results

- 상세 테이블: `docs/team/20260117_all-protocol-optimization/04_io_threads_baseline.md`
- tcp 구간은 대부분 패턴/사이즈에서 zlink 우위.
- ipc 131072B 등 일부 구간은 여전히 음수 (요약은 gap 분석 참고).

### Status

- IO thread 설정이 성능 영향이 큼.
- 적용 범위/기본값 변경 필요성은 추가 검토 필요.

## Phase 15: libzmq IO thread=2 비교 재확인

### Goal

- libzmq에도 IO thread=2가 적용된 상태에서 비교 확인.

### Bench

```
BENCH_IO_THREADS=2 BENCH_MSG_COUNT=20000 BENCH_TRANSPORTS=tcp \
  BENCH_MSG_SIZES=262144 \
  ./benchwithzmq/run_comparison.py DEALER_ROUTER --runs 3 --refresh-libzmq --build-dir build/bin
```

### Result

- DEALER_ROUTER tcp 262144: +13.38% throughput

## Phase 16: IO thread=2 전체 매트릭스 재실행

### Goal

- 전체 패턴/프로토콜/사이즈를 동일 조건으로 재확인.

### Bench

```
BENCH_IO_THREADS=2 BENCH_TRANSPORTS=inproc,tcp,ipc \
  BENCH_MSG_SIZES=64,256,1024,65536,131072,262144 \
  ./benchwithzmq/run_comparison.py ALL --runs 3 --refresh-libzmq --build-dir build/bin
```

### Results

- 상세 테이블: `docs/team/20260117_all-protocol-optimization/07_io_threads2_full_rerun.md`

### Status

- tcp 구간은 전반적으로 개선 유지.
- 잔여 음수 구간은 ipc 131072B 중심으로 집중됨.

## Phase 18: Unpinned IO thread sweep (1~4)

### Goal

- CPU 고정 해제 상태에서 IO thread 스윕을 통해 변동성/경향 확인.

### Harness Change

- `BENCH_NO_TASKSET=1`일 때 taskset 핀 해제.

### Bench

```
BENCH_NO_TASKSET=1 BENCH_IO_THREADS=<1|2|3|4> \
  BENCH_TRANSPORTS=tcp,ipc BENCH_MSG_SIZES=64,1024,262144 \
  ./benchwithzmq/run_comparison.py <PATTERN> --runs 3 --refresh-libzmq --build-dir build/bin
```

### Results

- 상세 테이블: `docs/team/20260117_all-protocol-optimization/08_unpinned_iothreads_sweep.md`
- 요약:
  - tcp 64B는 +5~+8% 수준으로 양호
  - tcp 1024/262144는 모든 IO thread에서 음수 경향
  - ipc는 IO thread>=2에서 중/대형 구간이 개선되는 경향

## Phase 17: IO thread=2 개선 원인 가설 정리

### Observation

- IO thread=2에서 zlink 향상이 libzmq보다 크게 나타남.

### Hypotheses

- zlink(ASIO 경로)의 syscall 비용/락 비용이 높아 IO thread 1개가 병목이 됨.
- IO thread 분산으로 poll/epoll 및 send/recv 처리 큐가 분리되어
  head-of-line blocking이 완화됨.
- libzmq는 기본 IO thread 1개에서도 상대적으로 효율이 높아
  추가 thread에 대한 이득이 작음.
- `run_comparison.py`는 `taskset -c 1`로 고정되어 있어
  개선이 단순 멀티코어 효과가 아니라 스케줄링/큐 분산 효과일 가능성.

## Phase 19: Unpinned IO thread=2 전체 매트릭스

### Goal

- CPU 핀 해제 상태에서 IO thread=2 full matrix 확인.

### Bench

```
BENCH_NO_TASKSET=1 BENCH_IO_THREADS=2 BENCH_TRANSPORTS=inproc,tcp,ipc \
  BENCH_MSG_SIZES=64,256,1024,65536,131072,262144 \
  ./benchwithzmq/run_comparison.py ALL --runs 3 --refresh-libzmq --build-dir build/bin
```

### Results

- 상세 테이블: `docs/team/20260117_all-protocol-optimization/09_unpinned_io_threads2_full_matrix.md`
- tcp 256B 이상 구간에서 대부분 패턴이 음수.
- ipc는 중/대형 구간이 대체로 우위, inproc은 대형 일부 음수.

### Note

- 초기 실행에서 `--refresh` 오타로 libzmq cache가 사용되어 결과가 왜곡됨.
  (정상 실행은 `--refresh-libzmq`로 재실행)

## Phase 20: Unpinned strace (PUBSUB tcp 1024/262144)

### Goal

- unpinned tcp 중/대형 구간 syscall 분포 확인.

### Bench

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

### Results (summary)

- 1024B: zlink sendto/recvfrom 1263/2266, libzmq 1264/1270
- 262144B: zlink sendto/recvfrom 15010/16013, libzmq 6010/6018
- 상세 표는 `docs/team/20260117_all-protocol-optimization/10_unpinned_strace_pubsub.md`
