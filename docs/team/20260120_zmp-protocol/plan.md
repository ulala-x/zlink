# ZMTP 대체 커스텀 프로토콜 계획 (ZMP v0)

**Date:** 2026-01-20  
**Owner:** 팀장님  
**Author:** Codex (Planning Agent)  
**Scope:** ZMTP 완전 비호환 커스텀 프로토콜, zlink 전용  
**Primary Goal:** 최대 성능 + 기존 ZMQ 소켓 패턴 유지

---

## 1. 목표 (Goals)

1) ZMTP 의존성 제거 또는 최소화
2) ZMQ 소켓 패턴(PAIR/DEALER/ROUTER/PUB/SUB/XPUB/XSUB) 유지
3) 메시지 프레이밍 규칙 단순화
4) API/동작 호환성 유지 범위 명확화
5) 연결 직후 소켓 타입 검증과 Identity 전달 방식 명확화

---

## 2. 배경 요약 (Background)

- ZMTP 표준 준수는 필수가 아님.
- 보안은 전송 계층 정책으로 관리 (TLS 권장), CURVE 제거.
- 성능 최적화가 최우선 목표.
- ZMTP 제거 시에도 간단한 메시지 프레임 규칙은 필요.
- zlink ↔ zlink 전용으로 진행.

---

## 3. 설계 원칙 (Principles)

- 핫패스 단순화(분기 최소, 복사 최소)
- 프레임 헤더 고정 길이
- 메시지 경계 명확, 재동기화 없음(오류 시 즉시 종료)
- ZMTP와 완전 분리(런타임 모드 선택)

---

## 4. 프로토콜 스펙 (v0)

### 4.1 헤더(8 bytes, 고정)

- Byte 0: Magic (0x5A)
- Byte 1: Version (0x01)
- Byte 2: Flags
  - bit0: MORE (multipart)
  - bit1: CONTROL (control frame)
  - bit2: IDENTITY (routing-id frame)
  - bit3: SUBSCRIBE
  - bit4: CANCEL
  - bit5~7: reserved (0)
- Byte 3: Reserved (0)
- Byte 4..7: Body length (uint32, big-endian)

### 4.2 바디

- body_len 바이트 payload
- 최대 크기 상한: 256MB (초과 시 연결 종료)

### 4.3 멀티파트 규칙

- MORE=1이면 동일 메시지의 다음 프레임
- MORE=0이면 메시지 종료

### 4.4 CONTROL 프레임

- CONTROL=1이면 바디는 control 타입을 포함
- 연결 직후 HELLO 1회 교환 필수
- HELLO 미수신 또는 타입 불일치 시 연결 종료

**CONTROL 타입**
- 0x01 = HELLO
- 0x02 = HEARTBEAT
- 0x03 = HEARTBEAT_ACK (v1 후보)

**HELLO 바디 포맷**
- Byte 0: Control type (0x01)
- Byte 1: Socket type (1 byte, 기존 ZMQ 값 재사용)
- Byte 2: Identity length (uint8)
- Byte 3..: Identity bytes

### 4.5 ROUTER identity 처리

- IDENTITY=1 프레임을 routing-id로 간주
- ROUTER 수신 경로에서만 허용, 기타 소켓은 오류 처리
- 메시지의 첫 프레임에서만 허용 (HELLO 이후)
- IDENTITY 프레임은 MORE=1과 동시 사용 불가
  - 이유: routing-id는 단독 프레임으로 고정해 라우팅 경계를 단순화

### 4.6 SUBSCRIBE/CANCEL 처리

- SUBSCRIBE/CANCEL은 데이터 프레임 플래그로 처리 (CONTROL 아님)
- SUBSCRIBE=1 또는 CANCEL=1이면 바디는 구독 토픽 바이트열
- XPUB/XSUB은 기존 ZMQ 구독 규칙을 그대로 사용

### 4.7 플래그 조합 규칙

- CONTROL과 IDENTITY 동시 사용 금지
- IDENTITY와 MORE 동시 사용 금지
- SUBSCRIBE/CANCEL은 MORE와 동시 사용 금지
- 예약 비트(5~7)는 0이어야 하며, 0이 아니면 연결 종료

### 4.8 오류 처리

- Magic/Version/Reserved 불일치: 연결 종료
- body_len 상한 초과: 연결 종료
- CONTROL 규칙 위반: 연결 종료
- 재동기화 시도 없음

---

## 5. 런타임 모드

- 환경변수: `ZLINK_PROTOCOL=zmtp|zmp`
- 모드 우선순위: 런타임 > 빌드 옵션
- 모드 불일치 시 즉시 종료

---

## 6. 구현 범위 (Phase 계획)

### Phase 0. 조사/정리 (1~2일)

- 핫패스 플로우 정리
- HELLO/IDENTITY/ROUTER 매핑 정리
- 기본 성능 기준선 확보
- 전송 계층 보안 정책 범위 확정 (TLS 권장)

산출물
- `docs/team/20260120_zmp-protocol/phase0_findings.md`
- `docs/team/20260120_zmp-protocol/phase0_flow.md`

### Phase 1. PoC (5~8일)

- encoder/decoder 분리 구현
- zmp 엔진 핸드셰이크 구현(HELLO)
- PUB/SUB/XPUB/XSUB용 SUBSCRIBE/CANCEL 플래그 처리
- ROUTER routing-id 처리
- 벤치마크 재측정

산출물
- `docs/team/20260120_zmp-protocol/phase1_impl_notes.md`

### Phase 2. 통합/검증 (2~3일)

- 동작 호환성 확인
- 문서화 및 내부 가이드 업데이트

---

## 7. 수정 대상 파일 (후보)

핵심
- `src/asio/asio_zmp_engine.cpp`
- `src/asio/asio_zmp_engine.hpp`
- `src/asio/asio_engine.cpp`
- `src/zmp_encoder.cpp`
- `src/zmp_decoder.cpp`
- `src/zmp_protocol.hpp`

세션/파이프
- `src/session_base.cpp`
- `src/pipe.cpp`
- `src/pipe.hpp`

테스트
- `tests/test_*.cpp`
- `unittests/unittest_*.cpp`

---

## 8. 리스크 및 대응

- ZMTP와 완전 비호환으로 인해 외부 상호운용 불가
- routing-id/identity 처리 미스매치 위험
- 핸드셰이크 타임아웃/에러 처리 불일치 위험
- 프로토콜 버전 업그레이드 시 단절 위험

대응
- HELLO 실패 시 즉시 종료
- 명확한 로그/에러 코드 유지
- 벤치와 단위 테스트로 회귀 확인
- ZMTP로 롤백 가능한 런타임 플래그 유지

---

## 9. 다음 단계 (Immediate Next)

- Phase 0 조사 시작
- 벤치 기준선 재수집(동일 옵션)
- HELLO/IDENTITY 규칙 확정

---

## 9.1 소켓 타입 호환 매트릭스 (요약)

| Client | Server | Valid | Notes |
|---|---|---|---|
| PAIR | PAIR | yes | 동등 패턴 |
| DEALER | ROUTER | yes | 기본 패턴 |
| DEALER | DEALER | yes | 동등 패턴 |
| ROUTER | ROUTER | yes | 양방향 라우팅 |
| PUB | SUB | yes | 구독 필수 |
| XPUB | XSUB | yes | 내부 확장 |
| DEALER | PUB | no | 타입 불일치 |
| SUB | ROUTER | no | 타입 불일치 |

---

## 9.2 HELLO 타임아웃

- HELLO 수신 대기: 3s (초과 시 연결 종료)

---

## 9.3 오류 코드 (내부)

- INVALID_MAGIC
- VERSION_MISMATCH
- FLAGS_INVALID
- BODY_TOO_LARGE
- SOCKET_TYPE_MISMATCH
- HELLO_TIMEOUT

---

## 10. 참고 소스 위치 (Reference Sources)

핵심 엔진/프로토콜
- `src/asio/asio_zmp_engine.cpp`
- `src/asio/asio_zmp_engine.hpp`
- `src/asio/asio_engine.cpp`
- `src/zmp_encoder.cpp`
- `src/zmp_decoder.cpp`
- `src/zmp_protocol.hpp`

세션/파이프 경로
- `src/session_base.cpp`
- `src/pipe.cpp`
- `src/pipe.hpp`

벤치/비교
- `benchwithzmq/run_benchmarks.sh`
- `benchwithzmq/run_comparison.py`
