# ZMP 프로토콜 설계 문서 (v0)

**Date:** 2026-01-20  
**Owner:** 팀장님  
**Author:** Codex (Planning Agent)  
**Scope:** zlink 전용 커스텀 프로토콜, ZMTP 완전 비호환

---

## 1. 목표

- 핫패스 단순화 (고정 헤더 + 최소 분기)
- ZMTP 의존성 최소화
- 기존 ZMQ 소켓 패턴 유지
- 전송 계층과 독립적으로 동작 (보안은 전송 계층 정책)

---

## 2. 프레임 구조

### 2.1 고정 헤더 (8 bytes)

- Byte 0: Magic (0x5A)
- Byte 1: Version (0x01)
- Byte 2: Flags
  - bit0: MORE
  - bit1: CONTROL
  - bit2: IDENTITY
  - bit3: SUBSCRIBE
  - bit4: CANCEL
  - bit5~7: reserved (0)
- Byte 3: Reserved (0)
- Byte 4..7: Body length (uint32, big-endian)

### 2.2 바디

- body_len 바이트 raw payload
- 최대 허용 크기: 256MB

---

## 3. 플래그 조합 규칙

- CONTROL과 IDENTITY 동시 사용 금지
- IDENTITY와 MORE 동시 사용 금지
- SUBSCRIBE/CANCEL과 MORE 동시 사용 금지
- 예약 비트(5~7)는 항상 0

---

## 4. 핸드셰이크

### 4.1 HELLO 프레임 (CONTROL)

- 연결 직후 양측 1회 교환
- 바디 포맷
  - Byte 0: Control type (0x01)
  - Byte 1: Socket type (1 byte, 기존 ZMQ 값 재사용)
  - Byte 2: Identity length (uint8)
  - Byte 3..: Identity bytes

### 4.2 타임아웃

- HELLO 수신 대기: 3s
- 타임아웃 시 즉시 연결 종료

---

## 5. ROUTER Identity 처리

- IDENTITY=1 프레임을 routing-id로 간주
- ROUTER 수신 경로 전용
- 메시지의 첫 프레임에서만 허용 (HELLO 이후)
- 단독 프레임으로만 사용

---

## 6. SUBSCRIBE/CANCEL 처리

- SUBSCRIBE/CANCEL은 데이터 프레임 플래그
- 바디는 구독 토픽 바이트열
- XPUB/XSUB은 기존 구독 규칙 그대로 사용

---

## 7. 오류 처리

- Magic/Version/Reserved 불일치: 연결 종료
- body_len 상한 초과: 연결 종료
- CONTROL 규칙 위반: 연결 종료
- 재동기화 없음

---

## 8. 소켓 타입 호환 규칙 (요약)

- PAIR ↔ PAIR
- DEALER ↔ ROUTER
- DEALER ↔ DEALER
- ROUTER ↔ ROUTER
- PUB ↔ SUB
- XPUB ↔ XSUB

---

## 9. 런타임 모드

- `ZLINK_PROTOCOL=zmp|zmtp`
- 보안은 전송 계층 정책에 따름

---

## 10. 상태 머신 (요약)

```
INIT -> HELLO_SENT -> HELLO_RECV -> READY -> DATA
   \-> ERROR (protocol violation / timeout)
```

- INIT: 연결 직후
- HELLO_SENT: HELLO 전송 완료
- HELLO_RECV: HELLO 수신 완료
- READY: 핸드셰이크 완료, 메시지 처리 가능
- DATA: 일반 메시지 송수신
- ERROR: 즉시 연결 종료

---

## 11. 시퀀스 예시

### 11.1 PAIR ↔ PAIR (단일 프레임)

```
Client -> Server: HELLO (PAIR)
Server -> Client: HELLO (PAIR)
Client -> Server: DATA
Server -> Client: DATA
```

### 11.2 DEALER -> ROUTER

```
DEALER -> ROUTER: HELLO (DEALER)
ROUTER -> DEALER: HELLO (ROUTER)
DEALER -> ROUTER: DATA
```

### 11.3 ROUTER -> ROUTER (routing-id 포함)

```
ROUTER -> ROUTER: HELLO (ROUTER)
ROUTER -> ROUTER: HELLO (ROUTER)
ROUTER -> ROUTER: IDENTITY
ROUTER -> ROUTER: DATA
```

### 11.4 PUB -> SUB (구독)

```
SUB -> PUB: HELLO (SUB)
PUB -> SUB: HELLO (PUB)
SUB -> PUB: SUBSCRIBE (topic)
PUB -> SUB: DATA (topic payload)
```

---

## 12. 타임아웃/에러 동작

- HELLO 타임아웃: 3s
- INVALID_MAGIC/FLAGS_INVALID: 즉시 연결 종료
- BODY_TOO_LARGE: 즉시 연결 종료

---

## 13. 참고 소스 위치

- `src/asio/asio_zmp_engine.cpp`
- `src/asio/asio_zmp_engine.hpp`
- `src/zmp_encoder.cpp`
- `src/zmp_decoder.cpp`
- `src/zmp_protocol.hpp`
- `src/session_base.cpp`
- `src/pipe.cpp`
