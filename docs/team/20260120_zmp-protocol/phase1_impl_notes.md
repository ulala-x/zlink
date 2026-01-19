# Phase 1 Implementation Notes (ZMP v0)

**Date:** 2026-01-20  
**Owner:** 팀장님  
**Status:** Draft

---

## Encoder/Decoder

- 고정 8바이트 헤더 처리
- flags/length 검증
- body_len=0 처리

---

## Handshake/HELLO

- HELLO 프레임 1회 교환
- 소켓 타입 불일치 시 즉시 종료
- HELLO 타임아웃: 3s

---

## ROUTER Identity

- IDENTITY 프레임은 ROUTER 수신 경로 전용
- 메시지 첫 프레임으로 제한
- IDENTITY+MORE 금지

---

## PUB/SUB/XPUB/XSUB

- SUBSCRIBE/CANCEL 플래그 처리
- 바디는 토픽 바이트열
- XPUB/XSUB 경로에 전달

---

## Tests

- encoder/decoder 단위 테스트
- HELLO 정상/에러 케이스 테스트
- IDENTITY 규칙 위반 테스트

---

## Reference Sources

- `src/asio/asio_zmp_engine.cpp`
- `src/asio/asio_engine.cpp`
- `src/zmp_encoder.cpp`
- `src/zmp_decoder.cpp`
- `src/zmp_protocol.hpp`
- `tests/test_*.cpp`
- `unittests/unittest_*.cpp`
