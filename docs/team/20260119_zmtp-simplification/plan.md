# ZMTP 대체 커스텀 프로토콜 - TLS 전용 최적화 계획

**Date:** 2026-01-19  
**Author:** Codex (Planning Agent)  
**Scope:** ZMTP 완전 비호환 커스텀 프로토콜 (TLS 전용, CURVE 제거)  
**Primary Goal:** 최대 성능 + 기존 ZMQ 소켓 패턴 유지

---

## 1. 목표 (Goals)

1. **TLS 전용 환경에서 핫패스 최소화**
2. **ZMTP 의존성 제거 또는 최소화**
3. **ZMQ 소켓 패턴(PAIR/DEALER/ROUTER/PUB/SUB/XPUB/XSUB) 유지**
4. **메시지 프레이밍 규칙의 단순화**
5. **API/동작 호환성 유지 범위 명확화**
6. **연결 직후 소켓 타입 검증과 Identity 전달 방식 명확화**

---

## 2. 배경 요약 (Background)

- ZMTP 표준 준수는 필수가 아님.
- 보안은 TLS만 사용, CURVE는 제거됨.
- 성능 최적화가 최우선 목표.
- ZMTP 제거 시에도 **간단한 메시지 프레임 규칙**은 필요.

---

## 3. 방향 (Direction)

**비호환 커스텀 프로토콜을 기본 전제로 진행**
- 고정 헤더 + length + flags만 사용하는 단순 프레임
- greeting/버전 협상 제거
- zlink ↔ zlink 전용

**이유**
- 핫패스 단순화 극대화
- ZMTP 유지 비용 제거

---

## 4. 실행 계획 (Phased Plan)

### Phase 0. 사전 분석 (1~2일)
- 현재 ZMTP 관련 핫패스 코드 경로 정리
- TLS 전용 환경에서 남아있는 핸드셰이크/메커니즘 처리 확인
- **프로토콜 오버헤드 비중 분석 (프레이밍/핫패스 비용)**
- 기본 성능 벤치마크 확보 (현재 기준선)
- **Socket type 검증/Identity 전달/Heartbeat 요구사항 정리**
- **64-bit 정렬 및 헤더 오버헤드(소형 메시지) 영향 측정**

**산출물**
- 핫패스 플로우 다이어그램
- 기준 성능 수치 (latency/throughput)
- 최소 핸드셰이크/컨트롤 프레임 규칙 합의안


### Phase 1. 커스텀 프로토콜 PoC (5~8일)

**목표**
- 고정 헤더 + length + flags 기반 프레이밍 도입
- greeting/버전 협상 제거

**작업 항목**
- 새로운 메시지 프레임 규칙 정의
- encoder/decoder 전면 수정
- 기존 ZMTP 경로와 명확히 분리
- **연결 직후 소켓 타입 교환/검증 프레임 1회 필수**
- **IDENTITY 플래그 방식 고정 (v0)**
- **Heartbeat 지원 방식 결정 (CONTROL 프레임 vs TCP Keep-alive)**
- 성능 벤치마크 재측정

**검증**
- zlink ↔ zlink 통신만 성공하면 OK
- 기존 ZMTP와 호환성 포기 명시


### Phase 2. 통합 및 기준 확정 (2~3일)
- 성능 개선 목표 충족 여부 확인
- 문서화 및 내부 가이드 업데이트

---

## 5. 메시지 프레임 규칙 (필수 정의 항목)

**고정 헤더 (v0, 8 bytes)**
- Byte 0: Magic (0x5A)
- Byte 1: Version (0x01)
- Byte 2: Flags
  - bit0: MORE (multipart)
  - bit1: CONTROL (control frame)
  - bit2: IDENTITY (routing-id frame)
  - bit3~7: reserved (0)
- Byte 3: Reserved (0)
- Byte 4~7: Body length (uint32, big-endian)

**바디**
- `body_len` 바이트 raw payload
- 최대 크기 상한: 256MB (초과 시 연결 종료)

**정렬/오버헤드**
- 헤더는 8 bytes로 64-bit 정렬을 만족
- v0는 8 bytes 고정, 축소 헤더는 v1 이후 검토
- 대형 메시지 확장 필요 시 length를 64-bit로 확장하는 옵션을 검토
- 모든 multi-byte 필드는 big-endian 고정

**멀티파트 규칙**
- MORE=1이면 동일 메시지의 다음 프레임이 이어짐
- MORE=0이면 메시지 종료

**ROUTER identity 처리 (결정)**
- **IDENTITY=1인 프레임을 routing-id로 간주**
- ROUTER 수신 경로에서만 허용, 기타 소켓은 오류 처리
- v0에서는 “첫 프레임=ID” 규칙을 사용하지 않음
- IDENTITY 프레임은 **HELLO/TYPE 이후, 데이터 프레임 직전에만 허용**
- IDENTITY 프레임은 MORE=1과 동시 사용 불가 (단독 프레임)
- HELLO Identity는 **연결 식별용**, IDENTITY 프레임은 **ROUTER 라우팅용**으로 구분
- ROUTER는 IDENTITY 프레임 필수 (누락 시 오류)

**CONTROL 프레임**
- **연결 직후 HELLO/TYPE 프레임 1회 필수 (socket type 교환/검증)**
- **HEARTBEAT 프레임 사용 (CONTROL=1)**
- CONTROL 프레임 바디 포맷은 고정 필드로 정의
- CONTROL 프레임은 **메시지 경계에서만 허용**

**CONTROL 타입 enum**
- 0x01 = HELLO
- 0x02 = HEARTBEAT
- 0x03 = HEARTBEAT_ACK (v1 후보)

**HELLO/TYPE 바디 포맷 (v0 고정)**
- Byte 0: Control type (0x01 = HELLO)
- Byte 1: Socket type (enum, 1 byte, 기존 ZMQ 값 재사용)
- Byte 2: Identity length (uint8, 0~255)
- Byte 3..: Identity bytes (optional)
- 최소 길이: 3 bytes (identity length=0)

**HELLO 교환 규칙 (v0)**
- 연결 직후 양측 HELLO 1회 교환
- HELLO 미수신 또는 socket type 불일치 시 연결 종료
- HELLO 수신 타임아웃: 3s

**HEARTBEAT 정책 (v0)**
- 기본 활성, 고정 간격 송신 (예: 5s)
- 연속 미수신 3회 시 연결 종료
- HEARTBEAT 바디는 0 bytes (타임스탬프 없음)

**오류 처리 (Fail-fast)**
- Magic/Version/Reserved 불일치: 연결 종료
- body_len 상한 초과: 연결 종료
- CONTROL 프레임 규칙 위반: 연결 종료
- multipart 도중 오류: 연결 종료
- 재동기화 시도 없음

**모드 스위치**
- 빌드 옵션 또는 런타임 옵션으로 모드 선택
- 예: `ZLINK_PROTOCOL=zmtp|zmp`
- 포트 분리 권장 (혼선 방지)
- Magic/Version 불일치 또는 HELLO 누락 시 즉시 연결 종료
- Reserved 비트/바이트가 0이 아니면 연결 종료
- 모드 우선순위: 런타임 옵션 > 빌드 옵션
- 모드 불일치 시 에러 로그 후 연결 종료

---

## 6. 수정 대상 파일 (후보)

**핵심 경로**
- `src/asio/asio_zmtp_engine.cpp`
- `src/asio/asio_zmtp_engine.hpp`
- `src/asio/asio_engine.cpp`

**핸드셰이크/메커니즘 관련**
- `src/mechanism.cpp`
- `src/mechanism.hpp`
- `src/mechanism_base.cpp`
- `src/mechanism_base.hpp`
- `src/null_mechanism.cpp`
- `src/null_mechanism.hpp`
- `src/session_base.cpp`
- `src/session_base.hpp`

**라우팅/ROUTER 관련**
- `src/router.cpp`

**옵션/메타데이터**
- `src/options.hpp`

**테스트**
- `tests/test_*.cpp` (추가/수정 범위는 Phase 1에서 확정)
- `unittests/unittest_*.cpp` (필요 시 내부 상태 머신 검증)

---

## 7. 도구 호환성 (명시 필요)

- **디버깅/분석 도구**
  - Wireshark ZMTP dissector, zmtp 프록시/로깅 도구는 커스텀 프로토콜 모드에서 비호환 가능
- **연동 라이브러리**
  - 외부 ZMTP 구현체(타 언어 바인딩/브로커)와의 상호 운용성은 커스텀 프로토콜 모드에서 포기
- **내부 도구**
  - 기존 테스트/벤치 스크립트가 ZMTP 전제인지 확인 필요

---

## 8. 리스크 및 대응

| 리스크 | 영향 | 대응 |
|--------|------|------|
| 커스텀 프로토콜 변경 범위 과대 | 일정/안정성 | PoC 분리 + 단계적 검증 |
| ZMTP 호환성 파기 | 기존 연동 불가 | 모드 분리 + 포트 분리로 리스크 완화 |
| 성능 개선 미미 | 목표 미달 | 핫패스 측정 후 병목 집중 |
| CONTROL 프레임 도입으로 핫패스 복잡화 | 성능/구현 난이도 | 최소 1회 교환으로 제한, 나머지는 데이터 프레임만 사용 |

---

## 9. 테스트/검증 계획

- 기능 테스트: 기존 소켓 패턴별 통신 시나리오
- 소켓 타입 검증: 잘못된 타입 연결 시 즉시 종료 확인
- Identity 플래그: ROUTER에서만 허용/순서 검증
- Heartbeat: 지원/미지원 경로 각각 정상 동작 확인
- 성능 테스트: TCP/TLS throughput, latency
- 회귀 테스트: 기존 Unity 테스트 + 주요 시나리오
- 성능 목표: 1K 이하 기준 중앙값 +5% 이상 개선
- Phase 0 프로토콜 오버헤드 비중 분석 결과에 따라 목표 재조정

---

## 10. 산출물

- 설계 문서: 커스텀 프레이밍 규칙(v0)
- 변경 로그: 제거된 ZMTP 요소 목록
- 성능 비교 리포트 (baseline vs custom protocol)

---

## 11. TLS 강제 조건

- TLS 비활성화 연결은 거부
- 테스트는 TLS on 기준으로만 측정

---

## 12. 구현 계획 (코드 변경 매핑)

**핵심 흐름**
- 새 프로토콜 전용 encoder/decoder 도입
- 기존 ZMTP 경로와 완전 분리 (런타임 옵션으로 선택)
- zmtp_engine과 zmp_engine 분리 여부 결정

**변경 지점**
- `src/asio/asio_zmtp_engine.cpp`
  - 기존 ZMTP handshake/encode/decode 경로 분기
  - `ZLINK_PROTOCOL=zmp`일 때 새 프레임 파이프 사용
- `src/asio/asio_zmtp_engine.hpp`
  - zmp encoder/decoder 인스턴스 추가
  - 모드 플래그 추가
- `src/asio/asio_engine.cpp`
  - 엔진 초기화 시 모드 선택
  - TLS 강제 옵션 체크

**새 파일(예상)**
- `src/zmp_encoder.cpp/.hpp`
- `src/zmp_decoder.cpp/.hpp`
  - v0 프레임 헤더/바디 처리
  - MORE 플래그 기반 multipart 처리
- `src/zmp_engine.cpp/.hpp` (분리 결정 시)
  - HELLO/TYPE/HEARTBEAT 처리 포함

**테스트**
- `tests/test_*.cpp`
  - zmp 모드 기본 동작 검증
  - ROUTER identity 프레임 유지 확인
  - socket type 검증/CONTROL 프레임 검증
