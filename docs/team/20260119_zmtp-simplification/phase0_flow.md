# Phase 0 핫패스/매핑 다이어그램

## 1. 핫패스 플로우 (ASIO 엔진 기준)

```
[transport open]
    |
    v
[transport handshake?]
    |
    +-- yes --> start_transport_handshake() -> on_transport_handshake()
    |                                   |
    |                                   v
    |                             plug_internal()
    |
    +-- no ----------------------------> plug_internal()
                                         |
                                         v
                              zmtp_engine: handshake()
                                         |
                                         v
                          encoder/decoder 선택 + next_msg 설정
                                         |
                                         v
                               pull_msg_from_session()
                                         |
                                         v
                             encode -> async_write
```

참고 경로
- `src/asio/asio_engine.cpp`: `start_transport_handshake()`, `plug_internal()`
- `src/asio/asio_zmtp_engine.cpp`: `handshake()`, `handshake_v*()`

---

## 2. HELLO/IDENTITY/ROUTER 매핑

```
[ZMP 연결 시작]
    |
    v
[HELLO/TYPE 1회 필수]
    |
    v
[ROUTER?]
    |
    +-- yes --> [IDENTITY 프레임 필수]
    |              |
    |              v
    |         [DATA 프레임들]
    |
    +-- no -----------------> [DATA 프레임들]
```

매핑 포인트
- ROUTER는 기존에 “첫 프레임=ID” 가정이 강함
- ZMP v0에서는 **IDENTITY 플래그 프레임**을 명시적으로 요구
- 엔진 레벨에서 ROUTER용 IDENTITY 프레임을 강제 주입하거나
  ROUTER 파서가 IDENTITY 플래그를 인지하도록 분기가 필요

참고 경로
- `src/asio/asio_zmtp_engine.cpp`: `routing_id_msg()`, `process_routing_id_msg()`
- `src/router.cpp`: `_routing_id_sent`, `_more_out`, `_prefetched_id`

---

## 3. TLS 강제 체크 후보 위치

```
[engine plug()] -> transport open
    |
    v
[requires_handshake()]
    |
    v
[on_transport_handshake]
    |
    v
[plug_internal]
```

후보
- `plug()` 이전/직후에 transport 종류 검사
- TLS가 아니면 fail-fast

---

## 4. 타임아웃 매핑

```
HELLO timeout -> handshake_ivl 재사용 또는 전용 타이머
HEARTBEAT timeout -> heartbeat_ivl/heartbeat_timeout 재사용
```

참고 경로
- `src/asio/asio_engine.cpp`: handshake/heartbeat timers
- `src/asio/asio_zmtp_engine.cpp`: ping/pong 처리
