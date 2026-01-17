# Current Baseline

## Benchmark Commands

```
./build.sh

# 전체 프로토콜 baseline (3-run)
BENCH_TRANSPORTS=inproc,tcp,ipc \
  BENCH_MSG_SIZES=64,256,1024,65536,131072,262144 \
  ./benchwithzmq/run_comparison.py ALL --runs 3 --refresh-libzmq

# ws/wss는 환경 가용 시 별도 실행
# BENCH_TRANSPORTS=ws,wss ...
```

## Baseline Results

- TODO: fill after first sweep.
