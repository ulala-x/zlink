# Phase 0 Findings (ZMP v0)

**Date:** 2026-01-20  
**Owner:** 팀장님  
**Status:** Draft

---

## Summary

- ZMP는 고정 8바이트 헤더와 최소 플래그로 메시지 프레이밍을 단순화한다.
- 핸드셰이크는 HELLO 1회 교환으로 제한한다.
- routing-id는 ROUTER 수신 경로에서만 허용하며 메시지 첫 프레임으로 고정한다.

---

## Baseline Performance

수집 계획
- 벤치 도구: `benchwithzmq/run_benchmarks.sh`
- 환경변수: `ZLINK_PROTOCOL=zmtp` (기준선), `ZLINK_PROTOCOL=zmp` (비교)
- 기준 출력: throughput, latency

실행 예시
- `ZLINK_PROTOCOL=zmtp benchwithzmq/run_benchmarks.sh --runs 3 --reuse-build`
- `ZLINK_PROTOCOL=zmp benchwithzmq/run_benchmarks.sh --runs 3 --reuse-build --zlink-only`

측정 결과
- 기준선 로그: `/tmp/bench_baseline_zmtp.txt`
- 실행은 ROUTER_ROUTER_POLL 구간에서 타임아웃으로 중단됨
- 재실행 시 패턴 분할 또는 `--runs 1` 권장
- ROUTER_ROUTER_POLL 단독 실행: `/tmp/bench_baseline_zmtp_rrpoll.txt`
- ROUTER_ROUTER_POLL tcp/65536B 표준 libzmq throughput 값이 음수로 기록됨 (재측정 필요)

---

## Hot Path Notes

송신 경로
- socket -> pipe -> session -> engine -> encoder -> transport

수신 경로
- transport -> decoder -> engine -> session -> pipe -> socket

---

## HELLO/IDENTITY/ROUTER Mapping

- HELLO는 연결 직후 1회 교환
- HELLO 바디에 소켓 타입/identity 포함
- routing-id는 ROUTER 수신 경로에서만 허용
- routing-id는 메시지 첫 프레임으로 고정

---

## TLS Enforcement Behavior

- ZMP 자체는 보안 정책을 정의하지 않음
- 보안은 전송 계층 정책에 따름 (네트워크 환경에서는 TLS 권장)

---

## Risks

- routing-id 처리 규칙 위반 시 즉시 연결 종료
- 버전 협상 부재로 버전 업그레이드 시 단절 위험

---

## Reference Sources

- `src/asio/asio_zmp_engine.cpp`
- `src/asio/asio_engine.cpp`
- `src/session_base.cpp`
- `src/pipe.cpp`
- `src/zmp_protocol.hpp`
- `benchwithzmq/run_benchmarks.sh`
