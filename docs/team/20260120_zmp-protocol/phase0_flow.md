# Phase 0 Flow Diagram Notes (ZMP v0)

**Date:** 2026-01-20  
**Owner:** 팀장님  
**Status:** Draft

---

## Session/Engine Flow

1) session attach
2) engine plug
3) HELLO 교환
4) engine_ready
5) normal IO loop

---

## Message Flow (Send Path)

socket -> pipe -> session -> engine -> encoder -> transport

---

## Message Flow (Recv Path)

transport -> decoder -> engine -> session -> pipe -> socket

---

## Control Frames

- HELLO: 연결 직후 1회 교환
- HEARTBEAT: 옵션 활성 시 주기 송신
- 규칙 위반 시 즉시 연결 종료

---

## Reference Sources

- `src/asio/asio_zmp_engine.cpp`
- `src/asio/asio_engine.cpp`
- `src/session_base.cpp`
- `src/pipe.cpp`
