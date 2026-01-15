# WS/WSS ROUTER 패턴 지원 계획

## 목표
- WebSocket(ws/wss) transport에서 ROUTER/DEALER, ROUTER/ROUTER 패턴을 정상 동작하게 한다.
- ZMTP 3.1 over WebSocket frames 환경에서 routing_id 교환을 지원한다.
- 기존 TCP/IPC/TLS(asio_zmtp_engine_t) 동작과 동일한 handshake 흐름을 확보한다.

## 배경 요약
- 현재 ws/wss는 `asio_ws_engine_t`를 사용하며 ZMTP 3.1은 지원한다.
- `routing_id_msg()`와 `process_routing_id_msg()`가 stub으로 구현되어 있어 ROUTER 패턴이 비활성화됨.
- tcp/ipc/tls는 `asio_zmtp_engine_t`에서 ROUTER 패턴이 정상 동작.
- WebSocket도 ZMTP 프레임을 그대로 실어 전송하므로 routing_id 교환을 추가하면 ROUTER 지원 가능.

## 범위
- 구현 대상: `src/asio/asio_ws_engine.cpp`
- 제한 제거 대상: `tests/test_transport_matrix.cpp`
- 테스트: 기존 테스트 전체 통과 확인

## 구현 상세 계획

### 1) routing_id_msg() 구현 (ws/wss)
**목표:** WS 엔진에서도 ZMTP routing_id 메시지를 생성하도록 구현

- 참고 구현
  - `src/asio/asio_zmtp_engine.cpp`의 `routing_id_msg()` (lines 428-436)
- 작업 내용
  - `asio_ws_engine_t::routing_id_msg()`를 `asio_zmtp_engine_t`와 동일한 규칙으로 구현
  - `_options.routing_id` 길이 검증, `routing_id` 메시지 생성, encoder 설정 흐름 동일 유지
  - WS 전송은 프레임 경계와 무관하게 ZMTP 메시지 포맷을 유지
- 확인 포인트
  - routing_id가 비어있을 때 동작 (zero-length routing_id)
  - encoder 상태 전이와 `_out_batching` 처리 동작 일관성
  - `msg_t` 스택 버퍼(64바이트) 제한 준수 여부 점검

### 2) process_routing_id_msg() 구현 (ws/wss)
**목표:** 수신한 routing_id 메시지를 파싱하고 옵션에 반영

- 참고 구현
  - `src/asio/asio_zmtp_engine.cpp`의 `process_routing_id_msg()` (lines 438-465)
- 작업 내용
  - `asio_ws_engine_t::process_routing_id_msg(msg_t *msg_)`를 `asio_zmtp_engine_t`와 동일한 흐름으로 구현
  - routing_id frame format 검증 (flags, size, header 등)
  - `_options.routing_id` 설정 및 소켓 메타 정보 갱신
- 확인 포인트
  - malformed routing_id 메시지 처리 (에러 코드/로그)
  - routing_id 길이 제한 및 재설정 여부
  - `msg_t` 스택 버퍼(64바이트) 제한 준수 여부 점검

### 3) handshake 완료 후 routing_id 교환 단계 추가
**목표:** ZMTP handshake 직후 routing_id 교환을 수행하도록 상태 전이 추가

- 작업 내용
  - `asio_ws_engine_t::out_event()` 및/또는 handshake 완료 지점에 routing_id 단계 삽입
  - `asio_zmtp_engine_t`의 흐름(ready 메시지 직후 routing_id 전송)을 참고하여 동일한 상태 전이 구현
  - `routing_id_msg()`로 outbound 메시지를 큐에 넣고, inbound에서 `process_routing_id_msg()` 호출
- 고려 사항
  - 기존 ws handshake 완료 처리와 충돌하지 않게 상태 머신 정리
  - socket 타입이 ROUTER 계열인 경우에만 routing_id 교환 수행
  - 기존 DEALER/REQ/REP 등 일반 패턴에는 영향 최소화
  - 상태 머신 단계/전이(ready -> routing_id_out -> routing_id_in -> 정상 데이터 전송) 명확히 문서화

### 4) transport matrix 제한 해제
**목표:** ws/wss에서 ROUTER 패턴 제한 제거

- 작업 내용
  - `tests/test_transport_matrix.cpp`에서 ws/wss ROUTER 제한 조건 제거
  - ws/wss + ROUTER/DEALER, ROUTER/ROUTER 조합을 허용하도록 기대 값 수정

### 5) Draft API 의존성 확인 및 정리
**목표:** ws/wss ROUTER 지원이 Draft API에 의존하지 않도록 확인

- 작업 내용
  - ROUTER/DEALER 관련 옵션/동작이 Draft API로 묶여있는지 점검
  - Draft 전용 심볼 사용 시 대체 경로(정식 API) 여부 확인
  - 필요 시 `VERSION` 및 빌드 플래그 영향 범위 문서화

### 6) 에러 처리 및 리소스 해제 경로 정리
**목표:** routing_id 교환 실패 시 안정적으로 종료/정리

- 작업 내용
  - `process_routing_id_msg()` 실패 시 소켓/엔진 상태 정합성 보장
  - 예외/에러 발생 시 버퍼/메시지 해제 경로 확인
  - 기존 zmtp 엔진과 동일한 에러 코드 및 종료 동작 유지

### 7) 테스트 및 검증
**목표:** 전체 테스트 통과 및 변경 사항 검증

- 기본 테스트
  - `./build.sh` 또는 `cmake -B build -DZMQ_BUILD_TESTS=ON` 후 `ctest --output-on-failure`
- 검증 항목
  - ws/wss에서 ROUTER/DEALER 메시지 송수신 정상 동작
  - ws/wss에서 ROUTER/ROUTER 양방향 routing_id 교환 정상 동작
  - 기존 tcp/ipc/tls 테스트 회귀 없음
  - 에러 경로(잘못된 routing_id 메시지) 테스트 가능 여부 확인

## 성능 고려 (LTO)
- 빌드 옵션 영향이 큰 변경은 아니지만, LTO 케이스에서 링크 에러나 경고가 없는지 점검
- 필요 시 `build-scripts/linux/build.sh x64 ON` + LTO 옵션 조합으로 간단 확인

## 리스크 및 대응

### 상태 머신 충돌
- **리스크:** WS handshake 완료 후 routing_id 단계 삽입 시 기존 out/in 이벤트와 충돌 가능
- **대응:** `asio_zmtp_engine_t`의 state 전이 순서를 그대로 복제하고, ws 엔진 상태 플래그를 동일하게 정리

### routing_id 메시지 포맷 오해
- **리스크:** routing_id frame flags/size 처리 불일치로 handshake 실패 가능
- **대응:** zmtp 엔진의 구현을 그대로 반영하고, 동일한 에러 처리 경로 사용

### 테스트 환경 의존성
- **리스크:** ws/wss 테스트가 환경에 따라 flaky할 수 있음
- **대응:** 로컬에서 반복 실행, 필요 시 CI에서 재확인

## 참고 코드 위치
- `src/asio/asio_zmtp_engine.cpp` (routing_id_msg, process_routing_id_msg)
- `src/asio/asio_ws_engine.cpp` (동일 함수 stub 위치)
- `tests/test_transport_matrix.cpp` (transport 패턴 제한)

## 완료 기준
- ws/wss ROUTER/DEALER, ROUTER/ROUTER 테스트가 통과한다.
- 전체 테스트(`ctest --output-on-failure`)가 성공한다.
- 기존 tcp/ipc/tls 동작에 regression이 없다.
