# zlink STREAM 소켓 상세 설계 (2026-01-22, rev2)

## 0. 범위와 목표
- 목적: TCP/TLS/WebSocket 위에 4-byte length prefix 기반 STREAM 소켓 제공
- 애플리케이션 메시지 형식: 2-프레임 `[routing_id][payload]`
- 구현 기반: ASIO(Transport/Proactor 엔진) + Beast(WebSocket)
- 비범위: ZMP 핸드셰이크/메타데이터 교환(사용하지 않음)

---

## 1. 프로토콜 명세 (zlink STREAM Protocol Specification)

### 1.1 용어
- Payload: 애플리케이션 데이터 바이트
- Routing ID: 연결을 식별하는 바이너리 ID(서버 내부 생성 또는 클라이언트 지정)
- Wire message: 네트워크로 전송되는 length-prefix 메시지

### 1.2 와이어 포맷 (4-byte length + payload)
모든 전송 매체(TCP/TLS/WS/WSS)에서 동일한 바이트 스트림 포맷을 사용한다.

```
0                   1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-------------------------------+-------------------------------+
|            Length (u32, BE)   |    Payload bytes ...         |
+-------------------------------+-------------------------------+
```

- Length: Big Endian uint32, payload 길이
- Payload: Length 만큼의 raw bytes
- Maximum: Length는 ZMQ_MAXMSGSIZE 초과 시 오류 처리

참고: 연결/해제 이벤트는 1-byte payload (0x01/0x00)로 구분한다 (섹션 1.4 참조).

### 1.3 WebSocket 전송 규칙 (성능 최적화)
WebSocket은 메시지 경계가 보존되므로 아래 규칙을 사용한다.

- 1 WS 메시지 안에 여러 개의 `Length+Payload`를 배치할 수 있다.
- 수신 측은 WS 메시지 바이트를 내부 버퍼에 누적하고,
  length prefix 기준으로 가능한 만큼 **반복 파싱**한다.
- 길이 4바이트보다 작은 잔여 데이터가 남으면 다음 WS 메시지에서 이어서 처리한다.

목표는 WS에서도 TCP처럼 배칭/파싱 루프를 돌려 syscall/콜백 횟수를 줄이는 것이다.

### 1.4 메시지 구조 (ZMQ 메시지 레벨)
애플리케이션은 항상 2-프레임 메시지를 사용한다.

```
Frame 0: [routing_id]  (MORE=1)
Frame 1: [payload]     (MORE=0)
```

- routing_id는 **4바이트 uint32** (Big Endian, Network Byte Order)
  - 기존 ZMQ의 5바이트(0x00 + uint32) 방식 대신 순수 4바이트 사용
  - 애플리케이션에서 직접 uint32_t로 캐스팅 가능
  - 자동 발급: 1부터 시작하는 순차 증가 정수
- payload는 길이 제한 내의 바이너리

### 1.5 연결/해제 이벤트 (명시적 구분)
STREAM 소켓은 연결 이벤트를 **1바이트 이벤트 코드**로 명시적 구분한다.

이벤트 코드:
```
0x01 = CONNECT (연결 수립)
0x00 = DISCONNECT (연결 종료)
```

메시지 형식:
```
Connect:    [routing_id (4B)][0x01]     - 1바이트 payload
Disconnect: [routing_id (4B)][0x00]     - 1바이트 payload
Data:       [routing_id (4B)][payload]  - N바이트 payload
```

주의:
- 1바이트 payload가 0x00 또는 0x01이면 이벤트로 해석됨
- 실제 데이터로 단일 바이트 0x00/0x01 전송 시 패딩 필요 (예: [0x00, 0x00])

연결 흐름:
1) 연결 수립: 새 연결 수립 시 소켓이 [routing_id][0x01]을 전달
2) 연결 종료(원격): 원격 종료 시 [routing_id][0x00] 수신
3) 연결 종료(로컬): 앱이 [routing_id][0x00]를 보내면 해당 연결 종료

### 1.6 Routing ID 설계 근거
- 4바이트 uint32 (BE)로 통일
- 메모리 정렬 용이, 저장/비교 단순

### 1.7 오류/예외 처리
- Length prefix가 ZMQ_MAXMSGSIZE 초과: **연결 즉시 종료**
- 길이/바디가 불완전할 경우: 데이터 누적 후 재시도
- TLS/WS 핸드셰이크 실패: 연결 실패 이벤트 발생

---

## 2. 소켓 API 명세

### 2.1 생성/바인드/연결
```cpp
void *ctx = zmq_ctx_new();

// server (bind)
void *s = zmq_socket(ctx, ZMQ_STREAM);
zmq_bind(s, "tcp://*:9000");
// zmq_bind(s, "tls://*:9443");
// zmq_bind(s, "ws://*:8080/stream");
// zmq_bind(s, "wss://*:8443/stream");

// client (connect)
void *c = zmq_socket(ctx, ZMQ_STREAM);
zmq_connect(c, "tcp://127.0.0.1:9000");
// zmq_connect(c, "tls://127.0.0.1:9443");
// zmq_connect(c, "ws://127.0.0.1:8080/stream");
// zmq_connect(c, "wss://127.0.0.1:8443/stream");
```

- ZMQ_STREAM는 새 소켓 타입 (예: #define ZMQ_STREAM 11)
- 내부적으로 options.raw_socket = true로 ZMP 핸드셰이크 비활성화

### 2.2 송신/수신
전송:
```cpp
// 1) routing_id 전송 (MORE)
zmq_send(s, routing_id, 4, ZMQ_SNDMORE);

// 2) payload 전송 (1바이트 0x00/0x01은 이벤트로 해석됨)
zmq_send(s, payload, payload_len, 0);
```

수신:
```cpp
uint32_t routing_id_be;
int rid_size = zmq_recv(s, &routing_id_be, 4, 0);
int more = 0;
size_t more_size = sizeof(more);
zmq_getsockopt(s, ZMQ_RCVMORE, &more, &more_size); // must be 1

unsigned char payload[4096];
int payload_size = zmq_recv(s, payload, sizeof(payload), 0);
```

### 2.3 소켓 옵션
STREAM 소켓은 ROUTER와 유사한 라우팅 옵션 및 전송 옵션을 지원한다.

라우팅/ID:
- ZMQ_ROUTING_ID: 로컬 소켓 라우팅 ID 지정
- ZMQ_CONNECT_ROUTING_ID: connect 시 사용할 라우팅 ID 지정

전송/버퍼:
- ZMQ_LINGER, ZMQ_SNDTIMEO, ZMQ_RCVTIMEO
- ZMQ_SNDHWM, ZMQ_RCVHWM
- ZMQ_MAXMSGSIZE
- ZMQ_RECONNECT_IVL, ZMQ_RECONNECT_IVL_MAX, ZMQ_IMMEDIATE

TCP:
- ZMQ_TCP_KEEPALIVE, ZMQ_TCP_KEEPALIVE_CNT,
  ZMQ_TCP_KEEPALIVE_IDLE, ZMQ_TCP_KEEPALIVE_INTVL
- ZMQ_TCP_MAXRT, ZMQ_TOS, ZMQ_BINDTODEVICE

TLS:
- ZMQ_TLS_CERT, ZMQ_TLS_KEY, ZMQ_TLS_CA
- ZMQ_TLS_VERIFY, ZMQ_TLS_REQUIRE_CLIENT_CERT
- ZMQ_TLS_HOSTNAME, ZMQ_TLS_TRUST_SYSTEM, ZMQ_TLS_PASSWORD

WebSocket:
- 엔드포인트 URL의 path 사용 (예: ws://host:port/path)
- 별도 옵션 없음 (Beast 핸드셰이크 사용)

---

## 3. 클라이언트 구현 가이드

### 3.1 공통 규칙
- 송신: len(u32, BE) + payload
- 수신: 4바이트 length 읽고, 그 길이만큼 payload 읽기
- payload 길이 1이고 값이 0x01/0x00이면 이벤트
- 단일 바이트 데이터 0x01/0x00은 패딩 필요

---

## 4. 서버 구현 가이드 (zlink STREAM)

### 4.1 기본 흐름
1) STREAM 소켓 생성 및 bind
2) [routing_id][payload] 수신
3) payload 길이/값 검사: 길이 1이고 0x01=connect, 0x00=disconnect, 그 외=data
4) 응답 시 동일 routing_id로 2-프레임 송신

### 4.2 이벤트 감지 (모니터는 선택적)
- connect/disconnect는 payload 이벤트로 처리
- TLS/WS 핸드셰이크 실패나 연결 실패는 모니터 이벤트로 확인 가능

---

## 5. 아키텍처 (핵심 경로)

### 5.1 클래스 구성
```
socket_base_t
  └─ routing_socket_base_t
       ├─ router_t (existing)
       └─ stream_t (new)

session_base_t
  └─ uses i_engine
       ├─ asio_engine_t (existing)
       │    └─ asio_raw_engine_t (new, TCP/TLS/WS/WSS)
       └─ asio_zmp_engine_t (existing)

i_asio_transport
  ├─ tcp_transport (existing)
  ├─ tls_transport (existing)
  ├─ ws_transport (existing)
  └─ wss_transport (existing)
```

### 5.2 수신 시퀀스
```
Remote Client
  Length(4) + Payload
    ↓
Transport (tcp/tls/ws)
    ↓
asio_raw_engine_t
  - length prefix decode
  - msg_t(payload)
    ↓
stream_t::xrecv()
  - routing_id + payload 2-프레임 반환
    ↓
Application
```

### 5.3 송신 시퀀스
```
Application
  send [routing_id][payload]
    ↓
stream_t::xsend()
  - routing_id로 pipe 선택
  - payload 전달
    ↓
asio_raw_engine_t
  - length prefix encode
  - TCP/TLS: stream write
  - WS/WSS: 배칭 후 단일 WS 메시지 write
```

### 5.4 연결/해제 이벤트 흐름
```
listener accept
  ↓
pipe 생성 + routing_id 할당
  ↓
stream_t -> [routing_id][0x01] deliver
  ↓
Application (connect 이벤트 처리)

disconnect 발생
  ↓
pipe 종료
  ↓
stream_t -> [routing_id][0x00] deliver
  ↓
Application (disconnect 이벤트 처리)
```

---

## 6. 구현 체크리스트

### 6.1 헤더/상수 정의
- [ ] include/zmq.h: #define ZMQ_STREAM 11 추가
- [ ] src/core/options.hpp: raw_socket 옵션 추가

### 6.2 소켓 구현
- [ ] src/sockets/stream.hpp/.cpp
  - routing_socket_base_t 상속
  - routing_id: 4바이트 uint32 (BE)
  - connect/disconnect 이벤트 주입 (1바이트 0x01/0x00)

### 6.3 Raw 프로토콜
- [ ] src/protocol/raw_encoder.hpp/.cpp
- [ ] src/protocol/raw_decoder.hpp/.cpp
  - 4-byte length prefix encode/decode
  - ZMQ_MAXMSGSIZE 초과 시 EMSGSIZE

### 6.4 Raw 엔진
- [ ] src/engine/asio/asio_raw_engine.hpp/.cpp
  - ZMP 핸드셰이크 제거
  - raw_encoder/raw_decoder 사용
  - WS 배칭 지원 (여러 메시지를 한 WS 메시지로)

### 6.5 트랜스포트 연동
- [ ] src/transports/tcp/asio_tcp_connecter.cpp
- [ ] src/transports/tcp/asio_tcp_listener.cpp
- [ ] src/transports/tls/asio_tls_connecter.cpp
- [ ] src/transports/tls/asio_tls_listener.cpp
- [ ] src/transports/ws/asio_ws_connecter.cpp
- [ ] src/transports/ws/asio_ws_listener.cpp

raw_socket이면 asio_raw_engine_t 사용

### 6.6 빌드 시스템
- [ ] CMakeLists.txt: 신규 소스 등록
  - src/sockets/stream.cpp
  - src/protocol/raw_encoder.cpp
  - src/protocol/raw_decoder.cpp
  - src/engine/asio/asio_raw_engine.cpp

### 6.7 테스트
- [ ] tests/test_stream_socket.cpp
  - Basic Echo (TCP)
  - Connect/Disconnect Event (1바이트 0x01/0x00)
  - Multiple Clients
  - Max Msg Size Breach
  - TLS STREAM (조건부)
  - WebSocket STREAM (조건부)
  - WS 배칭 파싱 (여러 메시지를 1 WS 메시지에 담아 수신)

---

## 7. 구현 핵심 포인트

### 7.1 raw_decoder/encoder
- decoder: 4바이트 BE length → payload 읽기
- encoder: 4바이트 BE length + payload 생성

### 7.2 WS 배칭 규칙
- 송신: out_batch_size 만큼 여러 raw 메시지를 하나의 버퍼로 합쳐
  단일 WS 메시지로 전송
- 수신: WS 메시지 바이트를 누적하고 length prefix 기준으로
  가능한 만큼 반복 파싱

### 7.3 connect/disconnect 이벤트 주입
- stream_t::xattach_pipe()에서 [routing_id][0x01]
- stream_t::xpipe_terminated()에서 [routing_id][0x00]

---

## 8. 제약/주의사항
- 단일 바이트 0x00/0x01 데이터는 이벤트와 충돌 → 패딩 필요
- WS는 메시지 단위 프로토콜이므로 **배칭이 핵심 성능 요인**
- ZMP 핸드셰이크 미사용 (raw_socket)


---

## 9. 성능 전략 (WS 90% 목표)

### 9.1 성능 목표
- 목표: Pure Beast WS 성능 대비 90% 이상
- 기준: 동일 메시지 크기/반복 횟수 벤치에서 WS/TCP 비율 비교

### 9.2 병목 가설
- WS는 메시지 경계가 보존되어 콜백/파싱 횟수가 증가함
- zlink 파이프/세션 경로 오버헤드가 WS에서 그대로 노출됨
- 해결은 **배칭 + 콜백 감소 + 복사 최소화**

### 9.3 WS 배칭 송신 (핵심)
- 여러 `Length+Payload`를 하나의 WS 메시지로 결합
- out_batch_size에 맞춰 배치 후 단일 async_write
- 목표: 1 syscall / 1 callback로 다수 메시지 전송

### 9.4 WS 배칭 수신 (핵심)
- WS 메시지 바이트를 누적 버퍼에 쌓고 반복 파싱
- length prefix 기준으로 가능한 만큼 파싱하여 push
- 잔여 데이터는 다음 WS 메시지와 결합해 처리

### 9.5 복사 최소화
- raw_decoder가 가능한 경우 zero-copy 경로 사용
- 송신은 gather write 또는 contiguous buffer 사용
- WS에서도 가능한 한 한 번의 버퍼 구성으로 전송

### 9.6 콜백/스케줄링 최소화
- 한 번의 read 콜백에서 가능한 만큼 메시지 파싱/전달
- write는 배치 단위로 묶어 호출 횟수 축소
- 파이프/세션에서 불필요한 wake-up 줄이기

### 9.7 검증 방법
- 동일 조건 벤치에서 Pure Beast WS vs zlink WS 비교
- 메시지 크기(64B/256B/1KB), 반복 횟수(10k~100k)로 측정
- 목표 미달 시 배칭 크기/버퍼 전략 튜닝


---

## 10. 테스트 추가 계획

### 10.1 유닛 테스트 (protocol)
- raw_decoder/encoder 정상 케이스
- length prefix 부족/초과/오류 케이스
- ZMQ_MAXMSGSIZE 초과 → EMSGSIZE 반환

### 10.2 기능 테스트 (socket)
- Basic Echo (TCP)
- Connect/Disconnect 이벤트 (1바이트 0x01/0x00)
- Multiple Clients 동시 연결
- Max Msg Size Breach 처리
- TLS STREAM (빌드 플래그 조건부)
- WS/WSS STREAM (빌드 플래그 조건부)
- WS 배칭 파싱 (1 WS 메시지에 다수 length+payload 포함)

### 10.3 성능/회귀 테스트
- Pure Beast WS vs zlink WS 상대 성능 비교
- 메시지 크기 64B/256B/1KB, 반복 10k~100k
- 목표: WS가 Pure Beast 대비 90% 이상 유지

