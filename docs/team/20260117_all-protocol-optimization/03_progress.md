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
