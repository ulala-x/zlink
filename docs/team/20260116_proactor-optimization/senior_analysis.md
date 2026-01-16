# 시니어 개발자의 Proactor 최적화 우선순위 분석

**날짜:** 2026-01-16
**작성자:** 시니어 백엔드 개발자
**컨텍스트:** "Reactor(libzmq)의 옷을 입은 Proactor(ASIO)" 상태 개선

---

## 문제 정의

현재 코드는 "데이터를 준비하고 → 알림을 받고 → 쓴다"는 **Reactor 식 흐름**을 따르고 있는데,
이를 "준비되면 → 커널에 바로 밀어넣는다"는 **Proactor 식**으로 바꿔야 합니다.

---

## 🚨 Priority 1: Critical (Latency/Throughput 직결)

**가장 시급한 구조적 병목입니다. 여기만 고쳐도 성능의 80%는 돌아옵니다.**

### 1. Speculative Write (즉시 전송) 도입

**현재 문제:**
- `process_output()`에서 데이터를 인코딩한 후, 무조건 `start_async_write()`를 호출하여 io_context 큐에 넣습니다.
- 소켓이 비어있어도 컨텍스트 스위칭이 발생합니다.

**최적화 대상:**
- `src/asio/asio_engine.cpp`: process_output 함수
- `src/asio/i_asio_transport.hpp`: write_some (동기 쓰기) 인터페이스 추가

**변경 방향:**
- 큐에 넣기 전 `socket.write_some()`을 먼저 호출
- 실패(EWOULDBLOCK)시에만 비동기로 전환

**예상 효과:**
- Latency 복구 (최우선)
- 즉시 전송 가능 시 컨텍스트 스위칭 오버헤드 제거

---

### 2. Zero-Copy Write (Scatter-Gather I/O)

**현재 문제:**
- ZMQ `msg_t`의 데이터 블록을 asio_engine 내부의 선형 버퍼(`_write_buffer`)로 **memcpy** 한 뒤 전송합니다.

**최적화 대상:**
- `src/asio/asio_engine.cpp`: 인코더와 버퍼 관리 로직
- `src/asio/tcp_transport.cpp`: boost::asio::buffer 생성 로직

**변경 방향:**
- `msg_t`의 프레임 포인터들을 복사 없이 `std::vector<boost::asio::const_buffer>`로 모아서(scatter) 바로 async_write에 전달

**예상 효과:**
- Throughput 증대
- CPU 사용량 감소 (memcpy 제거)
- 메모리 대역폭 절약

---

## 🛠️ Priority 2: Architectural (Proactor 구조 정렬)

**ASIO의 비동기 특성을 제대로 활용하여 CPU 효율을 높이는 작업입니다.**

### 3. Read Coalescing (읽기 루프 융합)

**현재 문제:**
- `on_read_complete` 핸들러가 호출될 때마다 딱 정해진 크기만 읽고 리턴합니다.
- 예: 헤더 읽고 리턴 → 다시 큐잉 → 바디 읽기

**최적화 대상:**
- `src/asio/asio_engine.cpp`: on_read_complete 및 process_input

**변경 방향:**
- 핸들러 내에서 **while 루프**를 돌며 소켓 버퍼에 데이터가 있는 한 계속 디코딩하여 메시지를 처리
- 핸들러 호출 오버헤드 감소

**예상 효과:**
- 핸들러 호출 횟수 감소
- CPU 캐시 효율 향상

---

### 4. Timer Integration (타이머 구조 단일화)

**현재 문제:**
- `io_thread_t`가 관리하는 레거시 타이머 휠과, ASIO의 `steady_timer`가 혼재되어 있습니다.
- `asio_poller`가 `run_for()`로 타임아웃을 걸며 깨어나는 방식은 비효율적입니다.

**최적화 대상:**
- `src/asio/asio_poller.cpp`: loop() 함수의 타임아웃 처리

**변경 방향:**
- 모든 타임아웃을 io_context 내의 `asio::steady_timer` 이벤트로 등록
- 폴링 루프가 불필요하게 깨어나지 않도록 변경

**예상 효과:**
- Wakeup 최소화
- CPU 사용량 감소 (불필요한 폴링 제거)

---

## ⚙️ Priority 3: Micro-optimizations (메모리/할당)

**C++ 런타임 오버헤드를 줄이는 작업입니다.**

### 5. Custom Handler Allocation (메모리 재사용)

**현재 문제:**
- `async_read`/`async_write`를 호출할 때마다 ASIO는 내부적으로 핸들러(람다 등)를 저장하기 위해 new/malloc을 할 수 있습니다.

**최적화 대상:**
- `src/asio/asio_engine.cpp`: 모든 비동기 핸들러

**변경 방향:**
- `src/asio/handler_allocator.hpp` (이미 파일은 존재함)를 활용
- 연결마다 할당된 고정 메모리 청크(Memory Pool)를 재사용하도록 핸들러에 할당기를 바인딩

**예상 효과:**
- 힙 할당 제거
- 메모리 파편화 감소

---

### 6. Strand 오버헤드 점검

**현재 문제:**
- io_context가 하나인데도 불필요하게 strand를 사용하여 락 비용을 지불하고 있을 가능성이 있습니다.

**최적화 대상:**
- `src/asio/asio_engine.cpp`, `src/asio/asio_poller.cpp`

**변경 방향:**
- ZMQ_IOTHREAD_POLLER_USE_ASIO 환경에서 단일 스레드(1 io_thread)가 보장된다면 strand 제거 검토

**예상 효과:**
- 락 오버헤드 제거
- 성능 향상

---

## 📋 요약 리스트

| 순위 | 항목 | 대상 파일 | 목표 |
|------|------|----------|------|
| **1** | **Speculative Write** | `asio_engine.cpp` | **Latency 복구 (최우선)** |
| **2** | **Zero-Copy Write** | `asio_engine.cpp`, `tcp_transport.cpp` | **Throughput 증대, CPU 감소** |
| **3** | **Read Coalescing** | `asio_engine.cpp` | **핸들러 호출 비용 절감** |
| 4 | Timer Integration | `asio_poller.cpp` | Wakeup 최소화 |
| 5 | Custom Allocator | `asio_engine.cpp` | 힙 할당 제거 |
| 6 | Strand 오버헤드 점검 | `asio_engine.cpp`, `asio_poller.cpp` | 락 비용 제거 |

---

## 실행 전략

1. **Phase 1**: Priority 1 (Speculative Write, Zero-Copy Write) 구현
   - 예상 성능 향상: 50-80%
   - 기간: 3-5일

2. **Phase 2**: Priority 2 (Read Coalescing, Timer Integration) 구현
   - 예상 성능 향상: 추가 10-20%
   - 기간: 3-5일

3. **Phase 3**: Priority 3 (Micro-optimizations) 검토 및 구현
   - 예상 성능 향상: 추가 5-10%
   - 기간: 2-3일

**총 예상 기간**: 8-13일
**예상 총 성능 향상**: 65-110% (libzmq 동등 이상)

---

## 참고 사항

- **Speculative Write**는 이미 `20260114_ASIO-성능개선` 프로젝트에서 부분적으로 구현되었을 수 있음
- 기존 구현 확인 후 개선 방향 결정 필요
- 각 단계마다 벤치마크로 검증 필수
