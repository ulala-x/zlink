# Issue and Background

## Goal

- 모든 프로토콜/패턴/사이즈에서 libzmq 대비 throughput 90%+ 달성.
- latency 및 안정성 회귀 없이 진행.

## Scope

- Patterns: PAIR, PUBSUB, DEALER_DEALER, DEALER_ROUTER, ROUTER_ROUTER,
  ROUTER_ROUTER_POLL
- Transports: inproc, tcp, ipc, ws/wss (가용 시)
- Message sizes: 64, 256, 1024, 65536, 131072, 262144

## Environment

- WSL 환경, perf 설치 불가 (sudo 필요).
- 비교 벤치: `benchwithzmq/run_comparison.py`

## Context

- inproc 최적화 결과는 main에 병합 완료.
- native tuning 옵션 추가 완료:
  - `ENABLE_NATIVE_OPTIMIZATIONS` (`-march=native`)
  - `ENABLE_NATIVE_TUNE` (`-mtune=native`)

## Success Criteria

- 모든 패턴/사이즈/프로토콜에서 90%+ 유지.
- 테스트 및 기능 회귀 없음.
