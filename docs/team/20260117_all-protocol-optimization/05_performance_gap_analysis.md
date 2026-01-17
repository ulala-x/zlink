# Performance Gap Analysis

## Summary

- 1-run baseline 기준 저하 구간 확인됨 (상세 표는 `01_current_baseline.md`).

## 1-run Gaps (<= -10% throughput)

- PAIR: inproc 256B (-15%), tcp 131072B (-24%), ipc 262144B (-17%).
- PUBSUB: tcp 64B (-10%), tcp 262144B (-29%), ipc 131072B (-25%),
  ipc 262144B (-23%).
- DEALER_DEALER: inproc 256B (-14%), tcp 131072B (-12%),
  tcp 262144B (-14%), ipc 65536B (-25%), ipc 262144B (-13%).
- ROUTER_ROUTER: inproc 262144B (-13%), tcp 131072B (-13%),
  tcp 262144B (-16%), ipc 1024B (-11%).
- ROUTER_ROUTER_POLL: tcp 65536B (-22%), ipc 1024B (-14%),
  ipc 65536B (-12%).

## Notes

- WSL 환경으로 perf/flamegraph 제한.
- 필요 시 strace 중심 비교.
- PAIR tcp 65536B latency가 음수로 출력되는 이상치 확인.
