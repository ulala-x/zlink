# Phase 1 구현 진행 기록

## 1. 구현 내용 요약

- ZMP v0 프레이밍 구현
  - 고정 8바이트 헤더 + body_len(uint32, big-endian)
  - flags: MORE/CONTROL/IDENTITY
  - CONTROL 타입: HELLO/HEARTBEAT

- ASIO 엔진 분리
  - `asio_zmp_engine_t` 추가
  - HELLO/TYPE 교환 후 ZMP 인코더/디코더로 전환
  - IDENTITY 프레임 필수 규칙을 엔진 레벨에서 강제
  - CONTROL 프레임은 메시지 경계에서만 허용 (디코더에서 검사)

- TLS 강제
  - ZMP 모드에서 transport가 암호화되지 않았으면 fail-fast

- 모드 스위치
  - `ZLINK_PROTOCOL=zmp`일 때 ZMP 엔진 선택

## 2. 변경 파일

- `src/zmp_protocol.hpp`
- `src/zmp_encoder.cpp` / `src/zmp_encoder.hpp`
- `src/zmp_decoder.cpp` / `src/zmp_decoder.hpp`
- `src/asio/asio_zmp_engine.cpp` / `src/asio/asio_zmp_engine.hpp`
- `src/asio/asio_engine.hpp`
- `src/asio/asio_tcp_connecter.cpp`
- `src/asio/asio_tcp_listener.cpp`
- `src/asio/asio_ipc_connecter.cpp`
- `src/asio/asio_ipc_listener.cpp`
- `src/asio/asio_tls_connecter.cpp`
- `src/asio/asio_tls_listener.cpp`
- `CMakeLists.txt`

## 3. 빌드/테스트

- `./build.sh`
  - 빌드 성공
  - 테스트 61/61 통과 (fuzzer 4개 스킵, 기존 동일)

## 4. 벤치마크

- `benchwithzmq/run_benchmarks.sh --pattern PAIR --msg-sizes 1024 --runs 3 --reuse-build`
  - ZMTP 기준 비교 결과만 확보
  - ZMP는 TLS 전용인데 benchwithzmq는 tcp/inproc/ipc만 지원
  - TLS 전송 벤치 지원이 없어 ZMP 성능 수치는 아직 미측정

결론
- ZMP v0 구현은 빌드/테스트 통과
- ZMP 성능 평가는 TLS 벤치 지원 추가 이후 진행 필요
