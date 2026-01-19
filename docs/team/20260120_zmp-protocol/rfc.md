# RFC: ZMP Protocol v0 (Draft)

**Date:** 2026-01-20  
**Owner:** 팀장님  
**Status:** Draft

---

## 1. Abstract

ZMP v0는 zlink 전용 커스텀 메시지 프로토콜이다. ZMTP와 호환되지 않으며, 고정 길이 헤더와 단순 플래그를 사용해 핫패스를 단순화한다.

---

## 2. Motivation

- ZMTP 핫패스 복잡도 제거
- 헤더/프레이밍 오버헤드 감소
- ZMQ 소켓 패턴 유지

---

## 3. Terminology

- **Frame**: 헤더(8 bytes) + 바디
- **Control Frame**: CONTROL 플래그가 설정된 프레임
- **Identity Frame**: IDENTITY 플래그가 설정된 프레임

---

## 4. Frame Format

### 4.1 Header (8 bytes)

- Byte 0: Magic = 0x5A
- Byte 1: Version = 0x01
- Byte 2: Flags
  - bit0: MORE
  - bit1: CONTROL
  - bit2: IDENTITY
  - bit3: SUBSCRIBE
  - bit4: CANCEL
  - bit5~7: reserved (0)
- Byte 3: Reserved (0)
- Byte 4..7: Body length (uint32, big-endian)

### 4.2 Body

- `body_len` bytes
- Max size: 256MB
- body_len=0인 데이터 프레임은 허용
- SUBSCRIBE/CANCEL에서 body_len=0은 전체 토픽 의미

---

## 5. Flag Rules

- CONTROL과 IDENTITY 동시 사용 금지
- IDENTITY와 MORE 동시 사용 금지
- SUBSCRIBE/CANCEL과 MORE 동시 사용 금지
- SUBSCRIBE와 CANCEL 동시 사용 금지
- 예약 비트(5~7)는 0이어야 함

### 5.1 Flag Combination Table

| CONTROL | IDENTITY | MORE | SUB/CANCEL | Allowed | Notes |
|---|---|---|---|---|---|
| 1 | 1 | * | * | no | CONTROL+IDENTITY 금지 |
| * | 1 | 1 | * | no | IDENTITY+MORE 금지 |
| * | * | 1 | 1 | no | SUB/CANCEL+MORE 금지 |
| * | * | * | both | no | SUBSCRIBE+CANCEL 동시 설정 금지 |
| 0 | 0 | 0/1 | 0 | yes | 일반 데이터 프레임 |
| 0 | 0 | 0 | 1 | yes | 구독 프레임 |
| 1 | 0 | 0 | 0 | yes | CONTROL 프레임 |

---

## 6. Handshake

### 6.1 HELLO

- 연결 직후 양측 1회 교환
- Body format:
  - Byte 0: Control type = 0x01
  - Byte 1: Socket type (1 byte)
  - Byte 2: Identity length (uint8)
  - Byte 3..: Identity bytes

### 6.2 Timeout

- HELLO timeout: 3s
- 타임아웃 시 연결 종료

### 6.3 Control Types

| Type | Name | Body |
|---|---|---|
| 0x01 | HELLO | type + identity |
| 0x02 | HEARTBEAT | empty |
| 0x03 | HEARTBEAT_ACK | empty |

### 6.4 Heartbeat Behavior

- heartbeat_interval > 0 이면 주기적으로 HEARTBEAT 송신
- 일정 시간(heartbeat_timeout) 동안 응답 없으면 연결 종료
- HEARTBEAT_ACK는 v1 확장으로 예약

---

## 7. ROUTER Identity

- IDENTITY 프레임은 ROUTER 수신 경로 전용
- 메시지의 첫 프레임에서만 허용 (HELLO 이후)
- 단독 프레임으로만 사용

---

## 8. SUBSCRIBE/CANCEL

- 데이터 프레임 플래그로 처리
- 바디는 구독 토픽
- XPUB/XSUB은 기존 ZMQ 구독 규칙 유지

---

## 9. Error Handling

| Code | Name | Action |
|---|---|---|
| 0x01 | INVALID_MAGIC | Disconnect |
| 0x02 | VERSION_MISMATCH | Disconnect |
| 0x03 | FLAGS_INVALID | Disconnect |
| 0x04 | BODY_TOO_LARGE | Disconnect |
| 0x05 | SOCKET_TYPE_MISMATCH | Disconnect |
| 0x06 | HELLO_TIMEOUT | Disconnect |

### 9.1 Unknown Values

- 알 수 없는 control type: Disconnect
- 예약 비트가 0이 아님: Disconnect

---

## 10. Security

- ZMP는 보안 메커니즘을 정의하지 않음
- 보안은 전송 계층 정책에 따름 (네트워크 환경에서는 TLS 권장)

---

## 11. Compatibility

- ZMTP와 완전 비호환
- zlink ↔ zlink 전용

### 11.1 Socket Type Compatibility Matrix

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

### 11.2 Versioning

- Version byte는 0x01 고정
- 버전 협상 없음
- 다른 버전 수신 시 VERSION_MISMATCH로 종료

---

## 12. IANA Considerations

None.

---

## 13. Message Sequences

### 13.1 PAIR ↔ PAIR

```
Client -> Server: HELLO (PAIR)
Server -> Client: HELLO (PAIR)
Client -> Server: DATA
Server -> Client: DATA
```

### 13.2 DEALER -> ROUTER

```
DEALER -> ROUTER: HELLO (DEALER)
ROUTER -> DEALER: HELLO (ROUTER)
DEALER -> ROUTER: DATA
```

### 13.3 ROUTER -> ROUTER

```
ROUTER -> ROUTER: HELLO (ROUTER)
ROUTER -> ROUTER: HELLO (ROUTER)
ROUTER -> ROUTER: IDENTITY
ROUTER -> ROUTER: DATA
```

### 13.4 PUB -> SUB

```
SUB -> PUB: HELLO (SUB)
PUB -> SUB: HELLO (PUB)
SUB -> PUB: SUBSCRIBE (topic)
PUB -> SUB: DATA (topic payload)
```

---

## 14. References

- `docs/team/20260120_zmp-protocol/plan.md`
- `docs/team/20260120_zmp-protocol/design.md`
- `src/zmp_protocol.hpp`
