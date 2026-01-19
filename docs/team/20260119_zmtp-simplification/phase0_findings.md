# Phase 0 조사 기록

## 목적
- 기존 ZMTP 경로와 커스텀 프로토콜(ZMP) 설계가 충돌하는 지점을 사전에 파악
- HELLO/IDENTITY/ROUTER 규칙을 기존 코드 경로에 매핑
- TLS 강제 체크 위치와 실패 동작(로그/에러 코드) 후보 정리
- 타임아웃 정책(HELLO/HEARTBEAT) 적용 위치 후보 파악

---

## 1. 핫패스/핸드셰이크 경로 요약

**Transport handshake**
- `src/asio/asio_engine.cpp`
  - `start_transport_handshake()`에서 TLS/WS 등 transport 레벨 handshake 수행
  - `handshake_ivl`로 타임아웃 관리

**ZMTP handshake**
- `src/asio/asio_zmtp_engine.cpp`
  - `handshake()` → greeting 수신/버전 분기
  - `handshake_v1_0/_v2_0/_v3_x`에서 encoder/decoder 선택
  - ZMTP greeting/메커니즘 경로가 고정되어 있음

**메시지 프레임/라우팅 ID**
- `asio_zmtp_engine_t::routing_id_msg()`
- `asio_zmtp_engine_t::process_routing_id_msg()`
  - `msg_t::routing_id` 플래그가 ROUTER 흐름에 전달됨

---

## 2. HELLO/IDENTITY/ROUTER 규칙 매핑

**현재 ROUTER 동작(기존 ZMTP)**
- `src/router.cpp`
  - 첫 프레임을 routing-id로 처리하는 흐름이 기본
  - `_routing_id_sent`/`_more_out` 상태로 메시지 파이프라인 유지

**현행 엔진의 routing-id 주입 경로**
- `src/asio/asio_zmtp_engine.cpp`
  - `routing_id_msg()` / `process_routing_id_msg()`
  - `options.recv_routing_id`를 통해 라우팅 ID 프레임 전달

**ZMP v0 규칙과 충돌 지점**
- ZMP는 IDENTITY 플래그 기반 라우팅 ID를 명시적으로 요구
- ROUTER 쪽은 “첫 프레임이 routing-id”라는 전제가 깊게 박혀 있음
- 해결 방향(Phase 1 설계 반영 대상)
  - ROUTER 경로에서 IDENTITY 프레임을 첫 프레임으로 강제 주입
  - 또는 ROUTER 처리 로직이 IDENTITY 플래그를 해석하도록 분기 추가

---

## 3. TLS 강제 체크 위치 후보

**현재 TLS 경로**
- `src/asio/asio_tls_listener.cpp`
  - `asio_tls_listener_t::create_engine()`에서 TLS transport 생성
- `src/asio/asio_tcp_listener.cpp`
  - TCP는 일반 transport

**ZMP 모드 TLS 강제 후보**
- `asio_engine_t::plug()` 이전
  - transport가 ssl인지 확인 가능
  - TLS가 아니면 `error(connection_error)` 또는 이벤트 로그

**실패 동작 후보**
- `socket()->event_handshake_failed_protocol(...)`
- `error(protocol_error)` 또는 `error(connection_error)`

---

## 4. HEARTBEAT/타임아웃 정책 매핑

**기존 heartbeat 옵션/타이머**
- `src/options.hpp`
  - `heartbeat_interval`, `heartbeat_ttl`, `heartbeat_timeout`
- `src/asio/asio_engine.cpp`
  - `heartbeat_ivl_timer_id`, `heartbeat_timeout_timer_id`, `heartbeat_ttl_timer_id`
- `src/asio/asio_zmtp_engine.cpp`
  - `produce_ping_message()` / `process_heartbeat_message()`

**ZMP v0 적용 후보**
- CONTROL 프레임 heartbeat를 기존 heartbeat 타이머와 연동
- `heartbeat_interval`을 그대로 사용하거나 ZMP 전용 옵션 추가 검토

**HELLO 타임아웃 후보**
- `handshake_ivl` 기반 타임아웃 재사용 가능
- 필요 시 ZMP 전용 HELLO 타이머 추가

---

## 5. 프로토콜 오버헤드 비중 측정 포인트

- ZMTP greeting/handshake 비용
- encoder/decoder 프레임 파싱 비용
- routing-id 프레임 처리 비용
- heartbeat/ping/pong 처리 비용

측정 대상은 1K 이하 메시지 기준으로 우선 추정

---

## 6. Phase 1 리스크 사전 메모

- ROUTER routing-id 규칙 변경 시 파이프라인 충돌 가능
- ZMP 모드/TLS 강제 순서가 세션 초기화 흐름과 충돌 가능
- HELLO 3s/HEARTBEAT 5s/3회 정책은 환경별 튜닝 필요

