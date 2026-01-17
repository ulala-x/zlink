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
- in/out batch size 65536 실험에서 tcp/large 개선 폭이 제한적이고
  syscall count 변화가 없어 기본값 8192 유지 예정.
- ZMQ_ASIO_TCP_SYNC_WRITE=1 실험에서 sendto/recvfrom 감소는 확인되나
  tcp 262144 throughput 개선은 제한적(ROUTER_ROUTER -10% 내외 유지).
- BENCH_SNDBUF/RCVBUF 4MB는 ROUTER 계열 개선 효과가 있으나
  PUBSUB throughput이 악화되어 전면 적용은 보류.

## tcp large-size syscall hint

- PUBSUB tcp 262144 (msg_count=2000)에서 zlink는 sendto/recvfrom 호출 수가
  libzmq 대비 2~3배 높음.
- syscall per message 증가가 tcp large-size 저하의 주요 후보.
