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
- zlink만 4MB 적용 시 상대 throughput도 악화(12~18% 수준).
- ROUTER 기본 sndbuf/rcvbuf 상향(2MB/4MB)은 ROUTER_ROUTER 개선을 보여도
  DEALER_ROUTER tcp 262144에서 큰 회귀가 발생해 기본값 변경 보류.
- TCP sync write 기본 활성화(타입 제한 포함)도 변동성이 커서
  개선 여부가 불확실하므로 기본값 변경은 보류.

## tcp large-size syscall hint

- PUBSUB tcp 262144 (msg_count=2000)에서 zlink는 sendto/recvfrom 호출 수가
  libzmq 대비 2~3배 높음.
- syscall per message 증가가 tcp large-size 저하의 주요 후보.
- DEALER_ROUTER tcp 262144도 sendto/recvfrom이 2~3배 높음
  (30009/34013 vs 12010/12020).
- msg_count=20000, runs=5 재측정 시 tcp 262144에서
  ROUTER/ROUTER_POLL은 동급 또는 소폭 우위, DEALER_ROUTER는 ~-10%.
- BENCH_IO_THREADS=2 적용 시 tcp 262144에서 모든 패턴이
  zlink 우위로 전환(+20~+32% throughput).

## IO_THREADS=2 Gaps (runs=3)

- PAIR: ipc 1024B (-10.59%), ipc 131072B (-33.90%).
- PUBSUB: ipc 131072B (-12.49%).
- DEALER_DEALER: ipc 131072B (-21.56%).
- DEALER_ROUTER: ipc 65536B (-15.55%), ipc 131072B (-19.62%).
- ROUTER_ROUTER: ipc 131072B (-17.38%).
- ROUTER_ROUTER_POLL: ipc 131072B (-12.28%).

## IO_THREADS=2 Rerun Gaps (runs=3)

- PAIR: ipc 131072B (-18.35%).
- PUBSUB: ipc 131072B (-26.95%).
- DEALER_DEALER: ipc 256B (-10.09%), ipc 131072B (-19.49%).
- DEALER_ROUTER: ipc 131072B (-18.02%).
- ROUTER_ROUTER: ipc 131072B (-19.47%).

## IO thread scaling note

- zlink은 IO thread 1개에서 syscall/락 오버헤드가 병목이 되는 경향이 있어
  IO thread=2에서 개선 폭이 크게 나타난 것으로 추정됨.
