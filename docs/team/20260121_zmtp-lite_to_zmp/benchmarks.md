# Benchmarks Plan and Results

**Date:** 2026-01-21  
**Owner:** 팀장님

---

## 1. Commands

Baseline (ZMTP):
- `ZLINK_PROTOCOL=zmtp ./benchwithzmq/run_benchmarks.sh --runs 10 --reuse-build`

Stage A (ZMTP Lite):
- `ZLINK_PROTOCOL=zmtp ./benchwithzmq/run_benchmarks.sh --runs 10 --reuse-build`

Stage B (ZMP):
- `ZLINK_PROTOCOL=zmp ./benchwithzmq/run_benchmarks.sh --runs 10 --reuse-build`

Focused runs:
- `BENCH_TRANSPORTS=tcp,inproc,ipc BENCH_MSG_SIZES=64,256,1024`

---

## 2. Metrics

- Throughput (Mmsg/s for small, MB/s for large)
- Latency (us)
- Diff (%) vs libzmq baseline

---

## 3. Notes

- IPC is CPU-bound and often reveals protocol overhead.
- Negative or zero metrics indicate measurement anomalies and should be re-run.

---

## 4. Results

- Fill after each stage is complete.
