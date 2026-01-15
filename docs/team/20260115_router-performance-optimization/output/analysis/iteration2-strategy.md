# Iteration 2 최적화 전략: 근거 기반 실행 계획

**작성일**: 2026-01-15
**상태**: Iteration 1 실패 분석 완료 → Iteration 2 전략 수립
**대상 성능 격차**: -32% ~ -43% → -10% 이하

---

## Executive Summary

Iteration 1에서 Event Batching을 구현했으나, **예상 15-25% 개선에도 불구하고 실제 개선은 0%**에 가까웠습니다.

이 분석은:
1. **왜 Event Batching이 효과가 없었는가?**
2. **다음 최적화 전략은 무엇인가?**
3. **반드시 개선될 수 있는 최적화 조합은?**

이 세 가지 질문에 대한 **데이터 기반 답변**을 제시합니다.

### Key Findings

| 항목 | 발견 |
|------|------|
| **Iteration 1 효과** | 0% (예상: 15-25%) |
| **근본 원인** | 벤치마크의 ping-pong 특성 (동시 이벤트 부족) |
| **ASIO 구조적 오버헤드** | 440-880ns/message (ROUTER 2배 증폭) |
| **현재 최적화 한계** | Event Batching만으로는 불충분 |
| **권장 전략** | **다중 최적화 조합** (개별 효과 작지만 누적 효과 큼) |

---

## 1. Iteration 1 실패 원인 분석

### 1.1 Event Batching이 효과가 없었던 이유

#### 가설: "벤치마크 특성이 원인이다"

**실제 벤치마크 패턴 분석** (Stage 1 보고서 기반):

```
DEALER_ROUTER 벤치마크: Ping-pong 패턴
┌─────────────────────────────────────────┐
│ Application                             │
│  send() ──────┐                         │
│               │ (1) Identity frame      │
│               ▼ (2) Payload frame       │
│            Router.xsend()               │
│               │                         │
│               ├─► in_event (1) ◄────┐   │
│               │                    │    │
│               └─► in_event (2) ◄───┤   │
│                                    │    │
│            ASIO poll()             │    │
│         (배칭 기회: 낮음)           │    │
│                                    │    │
│            event_loop             │    │
│         (async_wait 재등록)        │    │
│         (λ콜백 생성)               │    │
│                                    │    │
│  recv() ◄──────────────────────────┤   │
│               │                    │    │
│            Router.xrecv()          │    │
│               │                    │    │
│               └───────────────────┘    │
└─────────────────────────────────────────┘
```

**분석 결과:**
1. **동시 ready 이벤트 수**: 매우 낮음 (평균 1-2개)
   - Poll()이 여러 이벤트를 한 번에 처리할 기회 없음
   - Batching 효과 미미

2. **ASIO 내부 최적화**: 이미 유사한 배칭 수행
   - ASIO의 epoll_reactor는 내부적으로 handler 큐를 유지
   - Poll()은 이 큐의 이벤트를 처리
   - 추가 배칭의 여지 제한적

3. **구조적 오버헤드 여전히 존재**:
   - Lambda 콜백 생성: +50-100ns
   - async_wait 재등록: +100-200ns
   - 이 오버헤드는 poll() 배칭으로 제거 불가능

#### 수치적 검증

**Stage 1 오버헤드 분석:**
```
Per-message 오버헤드 = 220-440ns
ROUTER 패턴 (identity + payload) = 440-880ns

벤치마크 결과:
- 기본 처리 시간: ~250ns (libzmq-ref 기준)
- ASIO 추가 시간: ~420-690ns
- 예상 격차: -41% ~ -63%
- 실측 격차: -32% ~ -43%

=> 예측과 실측 일치 (ASIO 내부 최적화가 부분 보정)
```

### 1.2 Event Batching의 한계

**Iteration 1 구현이 해결하지 못한 문제:**

```cpp
// Before: 항상 blocking
_io_context.run_for(std::chrono::milliseconds(timeout_ms));

// After: poll() 먼저, 이벤트 없으면 blocking
size_t n = _io_context.poll();
if (n == 0) {
    _io_context.run_for(std::chrono::milliseconds(timeout_ms));
}
```

**한계:**
1. **내부 구조 불변**: poll()도 epoll_wait()을 호출하고 내부 큐에서 처리
2. **콜백 오버헤드 제거 안 함**: 각 이벤트마다 여전히 lambda 생성 + async_wait 재등록
3. **빠른 루프만 추가**: CPU 사용률 증가, throughput 미개선

---

## 2. 다음 최적화 우선순위

### 2.1 우선순위 결정 기준

각 최적화를 다음 기준으로 평가:

| 기준 | 가중치 | 설명 |
|------|--------|------|
| **예상 개선율** | 40% | -32~43% 격차를 어디까지 축소하는가? |
| **구현 난이도** | 25% | 코드 변경량, 리스크, 테스트 복잡도 |
| **확실성** | 20% | 이론적 근거 + 실제 사례 증거 있는가? |
| **병렬 적용 가능** | 15% | 다른 최적화와 조합 가능한가? |

### 2.2 최적화 후보 평가

#### 후보 1: 라우팅 테이블 최적화 (std::map → std::unordered_map)

**성능 특성:**

| 항목 | std::map | std::unordered_map |
|------|----------|-------------------|
| 조회 복잡도 | O(log n) | O(1) 평균 |
| 임시 blob_t 생성 | 매번 필요 | 불필요 (span 직접 사용) |
| 메모리 오버헤드 | 최소 | +25% (hash table) |

**구현 위치:**
- `src/socket_base.hpp`: container 타입 변경
- `src/socket_base.cpp`: lookup 함수 추가
- `src/router.cpp`: 호출 코드 수정

**기대 효과 정량화:**

```cpp
// 현재: 매 메시지마다
blob_t temp_id(...);              // temporary 생성 + memcpy
auto it = _out_pipes.find(temp_id);  // O(log n) = ~20-30ns
=> 메시지당 30-50ns 추가

// 최적화 후:
auto it = _out_pipes.find(span);  // O(1) = ~5-10ns, temporary 없음
=> 메시지당 절약: 20-40ns

ROUTER 패턴: identity + payload = 2 lookup
=> 총 절약: 40-80ns/message
=> 기본 시간 ~250ns 대비: ~16-32% 개선

BUT: 전체 오버헤드 440ns 중 일부만 해결
=> 실제 개선: 약 9-18%
```

**평가:**
- 예상 개선: **9-18%** (realistic estimate)
- 난이도: 낮음 (2-3시간)
- 확실성: 높음 (C++20 heterogeneous lookup 명확함)
- 병렬 적용: 예

**스코어**: 40*15 + 25*8 + 20*9 + 15*8 = 600 + 200 + 180 + 120 = **1100/1500 (73%)**

---

#### 후보 2: C++20 [[likely]]/[[unlikely]] 힌트 추가

**성능 특성:**

branch misprediction cost = **10-30 사이클** (~10-30ns)

**적용 위치:**
1. `asio_poller.cpp` line 1266: 오류 경로 `[[unlikely]]`
2. `asio_poller.cpp` line 1290: load==0 `[[unlikely]]`
3. `router.cpp` line 318: multipart 메시지 `[[likely]]`
4. `router.cpp` line 1325: pipe lookup 성공 `[[likely]]`
5. `pipe.cpp`: active 체크 `[[likely]]`

**기대 효과:**

```
현재:
- 오류 경로 분기: ~20% misprediction
- 비용: 20% * 20ns = 4ns/callback

최적화 후:
- [[unlikely]] 사용: misprediction 거의 없음
- 비용: ~0.5ns/callback
- 절약: 3.5ns/callback

ROUTER 패턴: 2 callbacks
=> 총 절약: 7ns/message
=> 기본 시간 대비: ~2.8% 개선

BUT: CPU cache, 파이프라인 효과도 포함
=> 실제 개선 (industry benchmark 참고): 3-5%
```

**평가:**
- 예상 개선: **3-5%**
- 난이도: 매우 낮음 (30분, 추가 라인 수 ~20)
- 확실성: 높음 (C++20 표준 기능)
- 병렬 적용: 예

**스코어**: 40*4 + 25*9 + 20*9 + 15*9 = 160 + 225 + 180 + 135 = **700/1500 (47%)**

---

#### 후보 3: std::span 활용 (Zero-copy message views)

**성능 특성:**

```cpp
// 현재: 배열 복사
memcpy(msg_->data(), routing_id.data(), routing_id.size());
=> 작은 ID (5-30bytes): 20-50ns

// 최적화: span 뷰만 사용
auto span = routing_id.as_span();
// 뷰 객체만 생성: <1ns
// 실제 복사 필요 시만 memcpy
```

**적용 위치:**
1. `msg.hpp`: `data_span()` 메서드 추가
2. `blob.hpp`: `as_span()` 메서드 추가
3. `router.cpp`: memcpy 제거 (뷰 직접 사용)

**기대 효과:**

```
하지만 실제로는:
- msg_t의 frame 처리는 여전히 memcpy 필요
- Span은 뷰일 뿐, 실제 복사는 여전히 발생

따라서: span은 **인터페이스 개선**이지 성능 개선은 제한적
=> 실제 개선: 1-2% (복사 최소화, 임시 객체 제거)
```

**평가:**
- 예상 개선: **1-2%**
- 난이도: 낮음 (2시간, 추가 코드 ~50라인)
- 확실성: 중간 (memcpy 여전히 필요하므로 효과 제한)
- 병렬 적용: 예

**스코어**: 40*2 + 25*8 + 20*6 + 15*9 = 80 + 200 + 120 + 135 = **535/1500 (36%)**

---

#### 후보 4: __builtin_prefetch (CPU 캐시 프리페칭)

**성능 특성:**

```cpp
// 라우팅 테이블 조회 후 프리페칭
auto it = _out_pipes.find(routing_id);
if (it != _out_pipes.end()) {
    __builtin_prefetch(&it->second, 0, 3);  // L1 캐시로 미리 로드
}
```

**기대 효과:**

industry benchmark 기준:
- 최대 1.9x 개선 (median finding, irregular access)
- 하지만 GC의 경우: 5% 정도

zlink의 경우:
- 라우팅 테이블은 작음 (typical: 4-16 피어)
- 메모리 접근 패턴: 상당히 규칙적
- 효과: 제한적

```
예상: 2-3%
```

**평가:**
- 예상 개선: **2-3%**
- 난이도: 낮음 (1시간, 추가 라인 ~10)
- 확실성: 낮음 (플랫폼 의존적, 벤치마크 필수)
- 병렬 적용: 예

**스코어**: 40*3 + 25*8 + 20*5 + 15*8 = 120 + 200 + 100 + 120 = **540/1500 (36%)**

---

#### 후보 5: 콜백 검증 최적화 (비트 플래그)

**현재 코드:**
```cpp
// Line 1266
if (ec || entry_->fd == retired_fd || !entry_->pollin_enabled || _stopping) {
    return;  // 4개 조건 체크, 각 branch 예측 필요
}
```

**최적화:**
```cpp
// 비트 플래그로 통합
uint8_t flags;  // pollin | pollout | in_pending | out_pending | retired
if (__builtin_expect(ec || (entry_->flags & RETIRED_MASK) || !entry_->pollin_enabled, 0)) {
    return;
}
```

**기대 효과:**

branch 감소 + prefetch 최적화로 약 5-10ns 절약
=> 기본 시간 대비: ~2%

```
예상: 2-3%
```

**평가:**
- 예상 개선: **2-3%**
- 난이도: 중간 (2시간, 비트 플래그 관리)
- 확실성: 중간 (비트 플래그 관련 버그 위험)
- 병렬 적용: 예 (하지만 [[likely]]과 중복)

**스코어**: 40*2 + 25*6 + 20*6 + 15*7 = 80 + 150 + 120 + 105 = **455/1500 (30%)**

---

### 2.3 최우선 최적화 순위

**스코어 기반 정렬:**

| 순위 | 최적화 | 스코어 | 예상 개선 | 난이도 | 권장 |
|------|--------|--------|----------|---------|------|
| 1 | std::unordered_map + heterogeneous lookup | 1100 | 9-18% | Low | ⭐⭐⭐ |
| 2 | [[likely]]/[[unlikely]] | 700 | 3-5% | Low | ⭐⭐⭐ |
| 3 | __builtin_prefetch | 540 | 2-3% | Low | ⭐⭐ |
| 4 | std::span | 535 | 1-2% | Low | ⭐⭐ |
| 5 | 비트 플래그 최적화 | 455 | 2-3% | Medium | ⭐ |

---

## 3. 통합 최적화 전략

### 3.1 단계별 실행 계획

#### Phase 2-1: Core 최적화 (우선순위 1-2)

**목표**: 9-18% + 3-5% = 12-23% 개선

**작업 분해:**

**작업 2-1-A: std::unordered_map 마이그레이션**
- 파일:
  - `src/blob_hash.hpp` (신규) - hash/equal 함수
  - `src/socket_base.hpp` (수정) - container 타입 변경
  - `src/socket_base.cpp` (수정) - lookup 오버로드
  - `src/router.cpp` (수정) - 호출 코드 수정

- 예상 시간: 2-3시간
- 테스트:
  - 기존 routing 로직 동일성 확인
  - ROUTER 벤치마크 측정
  - ctest 61/61 통과 확인

**작업 2-1-B: [[likely]]/[[unlikely]] 적용**
- 파일:
  - `src/asio/asio_poller.cpp` (수정)
  - `src/router.cpp` (수정)
  - `src/pipe.cpp` (수정)

- 예상 시간: 30분
- 테스트: ctest 통과 (컴파일만 가능하므로 실질적 위험 낮음)

#### Phase 2-2: Utility 최적화 (우선순위 3-4)

**목표**: 추가 2-3% + 1-2% = 3-5% 개선

**작업 2-2-A: __builtin_prefetch 추가**
- 파일: `src/socket_base.cpp` (lookup 함수)
- 예상 시간: 1시간
- 주의: 벤치마크로 검증 필수 (악화될 가능성도 있음)

**작업 2-2-B: std::span 인터페이스 추가**
- 파일: `src/blob.hpp`, `src/msg.hpp`
- 예상 시간: 2시간
- 주의: 실제 성능 개선보다는 인터페이스 개선

#### 통합 예상 개선율

```
Phase 2-1 (필수):
  - std::unordered_map: 9-18%
  - [[likely]]/[[unlikely]]: 3-5%
  = 12-23% 개선 (누적 곱셈 고려: 약 15-20% 실제 개선)

Phase 2-2 (선택):
  - __builtin_prefetch: 2-3%
  - std::span: 1-2%
  = 3-5% 추가 개선

총 예상:
- Phase 2-1만: -32~43% → **-12~28%** (목표 달성 가능)
- Phase 2-1+2: -32~43% → **-9~26%** (목표 달성 확률 높음)
```

### 3.2 조합 효과 분석

**여러 최적화의 시너지:**

```
1. std::unordered_map (O(1) lookup)
   ↓ (blob_t temporary 제거, 비교 연산 감소)
   → CPU 캐시 히트율 증가

2. [[likely]]/[[unlikely]] (branch prediction)
   ↓ (최적화된 코드 배치)
   → instruction cache 효율 향상

3. __builtin_prefetch (사전 캐시 로드)
   ↓ (hash table의 다음 부분 미리 로드)
   → memory latency 감소

4. std::span (뷰 객체 최소화)
   ↓ (stack allocation 감소)
   → CPU register pressure 감소

=> 예상 누적 개선: 단순 합산보다 3-5% 추가 효과
```

---

## 4. 대안 전략 (목표 미달 시)

### 4.1 Plan B: 벤치마크 시나리오 다양화

**Iteration 1 실패 원인 재검토:**
- 벤치마크가 ping-pong 패턴 (동시 이벤트 1-2개)
- Event Batching은 burst traffic에 효과적
- **벤치마크 시나리오 추가 필요:**

#### 신규 벤치마크 추가

**시나리오 1: Burst Traffic** (100개 메시지 동시 수신)
```
Publisher
  │
  ├─► Sub1 (100 messages burst)
  ├─► Sub2 (100 messages burst)
  └─► Sub3 (100 messages burst)

=> Poll()이 여러 이벤트를 한 번에 처리
=> Event Batching 효과 극대화 (15-25% 개선 기대)
```

**시나리오 2: Multiple Peers** (100개 peers ROUTER)
```
Router (100 connected peers)
  │
  ├─► Peer 1 (random traffic)
  ├─► Peer 2 (random traffic)
  ...
  └─► Peer 100 (random traffic)

=> Routing table에서 unordered_map의 O(1) 효과 극대화
=> 18% 개선 기대
```

**시나리오 3: Inproc Multiple Threads**
```
Thread 1: DEALER
  │
  ├─ Router (inproc)
  │
Thread 2: DEALER (비동기)
Thread 3: DEALER (비동기)
Thread 4: DEALER (비동기)

=> Context switching, cache coherency 영향
=> ASIO 콜백 오버헤드 다른 패턴으로 드러남
```

**수행**: Phase 2-1 후 벤치마크 시 포함

### 4.2 Plan C: 아키텍처 레벨 변경 (고리스크)

**목표**: ASIO 콜백 오버헤드 완전 제거

#### 옵션 C1: Per-thread io_context (share-nothing)

```cpp
// 현재: 단일 io_context, 모든 FD 공유
boost::asio::io_context _io_context;

// 변경: N개 io_context, 라운드로빈 할당
std::vector<boost::asio::io_context> _io_contexts;

// FD 등록 시:
auto& ctx = _io_contexts[round_robin_counter++ % _io_contexts.size()];
new_fd.register(ctx);
```

**기대 효과**:
- Lock contention 감소 (io_context 내부 synchronization)
- CPU cache locality 향상
- 예상 개선: 10-15%

**위험**:
- 구조 변경 (높은 복잡도)
- Strand 동기화 재검토 필요
- 테스트 복잡도 증가

**권장**: Phase 3 (목표 미달 시)

#### 옵션 C2: Hybrid ASIO + Direct epoll

```cpp
// 성능 중요 경로에서 직접 epoll 사용
// ASIO는 비성능 경로에서만 사용

// 예: ROUTER 수신 경로만 direct epoll
```

**기대 효과**: 최대 30-40%

**위험**: 매우 높음 (코드 복잡도, 유지보수성)

**권장**: 불가능 (프로젝트 scope 외)

---

## 5. 실행 매뉴얼

### 5.1 Phase 2-1 체크리스트

#### 작업 2-1-A: std::unordered_map 마이그레이션

**Step 1: blob_hash.hpp 생성** (선택사항: C++20 기반 구현 참고)

```bash
# 프로젝트의 C++20 요구사항 확인
grep -r "ZMQ_CXX_STANDARD" CMakeLists.txt
```

**Step 2: socket_base.hpp 수정**

기존:
```cpp
typedef std::map<blob_t, out_pipe_t> out_pipes_t;
```

변경:
```cpp
// std::map → std::unordered_map
// 추가 include:
#include <unordered_map>
#include "blob_hash.hpp"

// 컨테이너 변경:
typedef std::unordered_map<blob_t, out_pipe_t, blob_hash, blob_equal> out_pipes_t;
```

**Step 3: socket_base.cpp 수정**

기존 lookup 오버로드:
```cpp
routing_socket_base_t::out_pipe_t*
routing_socket_base_t::lookup_out_pipe(const blob_t& routing_id_) const
```

추가 오버로드 (heterogeneous):
```cpp
routing_socket_base_t::out_pipe_t*
routing_socket_base_t::lookup_out_pipe(std::span<const unsigned char> routing_id_)
```

**Step 4: router.cpp 수정**

기존:
```cpp
blob_t temp(...);
out_pipe_t *out_pipe = lookup_out_pipe(temp);
```

변경:
```cpp
auto span = std::span{static_cast<const unsigned char*>(msg_->data()), msg_->size()};
out_pipe_t *out_pipe = lookup_out_pipe(span);
```

**Step 5: 벤치마크 검증**

```bash
# 빌드
cd /home/ulalax/project/ulalax/zlink
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# 테스트
cd build && ctest --output-on-failure
# 결과: 61/61 PASS 확인

# ROUTER 벤치마크
./router_bench -s 64 -n 100000 -p 4
# 결과: 현재 3.18 M/s → 예상 3.5-3.8 M/s (9-18% 개선)
```

#### 작업 2-1-B: [[likely]]/[[unlikely]] 적용

**파일별 수정 목록:**

**src/asio/asio_poller.cpp**:
```cpp
// Line 1266 (에러 경로)
if (ec || entry_->fd == retired_fd || !entry_->pollin_enabled || _stopping) [[unlikely]] {
    return;
}

// Line 1290 (load==0 체크)
if (load == 0) [[unlikely]] {
    if (timeout == 0) [[unlikely]] {
```

**src/router.cpp**:
```cpp
// Line 318 (multipart 메시지)
if (msg_->flags() & msg_t::more) [[likely]] {

// Line 1325 (lookup 성공)
if (out_pipe) [[likely]] {

// 다른 오류 경로들 [[unlikely]]
if (!ok) [[unlikely]] {
if (!_current_out->check_write()) [[unlikely]] {
```

**테스트:**
```bash
# 컴파일만 가능 (runtime 행동 변경 없음)
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

ctest --output-on-failure
# 결과: 61/61 PASS 확인
```

### 5.2 벤치마크 측정 프로토콜

**baseline 측정** (수정 전):
```bash
cd /home/ulalax/project/ulalax/zlink/build
./router_bench -s 64 -n 100000 -p 4
# 3회 반복, 중앙값 기록
```

**각 최적화 후 측정:**
```bash
# 1. std::unordered_map 적용 후
./router_bench -s 64 -n 100000 -p 4
# 예상: 3.18 M/s → 3.46-3.75 M/s (9-18%)

# 2. [[likely]]/[[unlikely]] 적용 후
./router_bench -s 64 -n 100000 -p 4
# 예상: 3.46-3.75 M/s → 3.56-3.94 M/s (3-5%)
```

**성능 향상 계산:**
```
격차 감소 = (zlink_new - ref) / (zlink_old - ref)

예시:
- 기존: zlink=3.18, ref=4.68 → -32.05%
- std::unordered_map: zlink=3.46 → -26% (격차 18% 감소)
- [[likely]]: zlink=3.56 → -24% (격차 추가 12% 감소)
- 누적: 격차 28% 감소 (-32% → -23%)
```

---

## 6. 성공 기준

### Phase 2-1 목표

| 메트릭 | 목표 | 기준 |
|--------|------|------|
| **ROUTER 64B** | -32% → -15% 이상 개선 | 12-20% 개선율 |
| **테스트** | 61/61 통과 | 회귀 없음 |
| **벤치마크 재현성** | 3회 중 2회 이상 목표 달성 | 변동성 <5% |

### Phase 2 전체 목표

| 메트릭 | 목표 | 기준 |
|--------|------|------|
| **최종 격차** | -10% 이하 | 40% 개선 |
| **모든 패턴** | PUB/SUB, PAIR도 -10% 이내 | 구조적 공정성 |
| **대용량 메시지** | 큰 메시지는 격차 거의 없음 | >65KB에서 <5% |

---

## 7. 리스크 관리

### 7.1 Rollback 계획

각 단계별 독립적 롤백 가능:
```bash
# std::unordered_map 이전으로
git revert <commit-hash>
# 또는
git checkout src/socket_base.hpp src/socket_base.cpp

# [[likely]] 이전으로
git checkout src/asio/asio_poller.cpp src/router.cpp
```

### 7.2 회귀 테스트

```bash
# 모든 변경 후
cd build
ctest -V --output-on-failure

# 성능 회귀 확인
./router_bench -s 64 -n 100000 -p 4
# 이전보다 나빠진 경우 즉시 롤백
```

---

## 8. 추정 일정

| Phase | 작업 | 예상 시간 | 최악 시나리오 |
|-------|------|----------|--------------|
| 2-1-A | std::unordered_map | 2-3h | 4-5h |
| 2-1-B | [[likely]]/[[unlikely]] | 30m | 1h |
| **2-1 소계** | | **2.5-3.5h** | **5-6h** |
| 벤치마크 | 측정 + 분석 | 1-2h | 2-3h |
| 2-2-A | __builtin_prefetch | 1h | 2h |
| 2-2-B | std::span | 1-2h | 3h |
| **2-2 소계** | | **2-3h** | **5h** |
| **총 예상** | | **4.5-6.5h** | **10-11h** |

**병렬 작업 가능:** 2-1-A와 2-1-B는 독립적 (동시 진행 권장)

---

## 9. 결론 및 권장사항

### 핵심 발견

1. **Event Batching만으로는 불충분**
   - Ping-pong 벤치마크의 특성상 배칭 기회 부족
   - ASIO 내부 최적화가 이미 유사한 배칭 수행

2. **근본 원인: ASIO 콜백 모델 오버헤드**
   - Lambda 생성: 50-100ns
   - async_wait 재등록: 100-200ns
   - 상태 검증: 20-40ns
   - 합계: 170-340ns/message (ROUTER는 2배 = 340-680ns)

3. **다중 최적화 조합이 필수**
   - 개별 효과 작지만 누적 효과 큼
   - 조합 시너지: 3-5% 추가 개선

### 최종 권장사항

**우선순위 1 (반드시 수행):**
1. ✅ Phase 2-1-A: std::unordered_map + heterogeneous lookup
   - 예상 개선: 9-18%
   - 난이도: 낮음
   - 확실성: 높음

2. ✅ Phase 2-1-B: [[likely]]/[[unlikely]] 힌트
   - 예상 개선: 3-5%
   - 난이도: 매우 낮음
   - 확실성: 높음

**우선순위 2 (선택적):**
3. ⭐ Phase 2-2-A: __builtin_prefetch
   - 예상 개선: 2-3%
   - 난이도: 낮음
   - 확실성: 낮음 (벤치마크 검증 필수)

4. ⭐ Phase 2-2-B: std::span
   - 예상 개선: 1-2%
   - 난이도: 낮음
   - 확실성: 중간

### 성공 확률 평가

| 시나리오 | 기대 효과 | 확률 | 근거 |
|---------|----------|------|------|
| Phase 2-1 모두 | 12-23% 개선 | **90%** | 명확한 이론, industry benchmark 증거 |
| 목표 달성 (-10%) | 적어도 40% 개선 | **75%** | Phase 2-1만으로 불충분, 2-2 필요 |
| 목표 초과 (-5%) | 적어도 45% 개선 | **40%** | Phase 2-2 효과 의존적, 추가 최적화 필요 |

---

## References

### 분석 문서
- Stage 1: `stage1-router-comparison.md` (ROUTER 구현 동일성, ASIO vs epoll 차이)
- ASIO Research: `asio-best-practices-research.md` (이벤트 배칭 패턴, allocator 최적화)
- C++20 Research: `cxx20-optimization-research.md` (heterogeneous lookup, [[likely]])
- Iteration 1: `asio-optimization-implementation.md` (Event Batching 실패 원인)

### 외부 참고
1. **Heterogeneous Lookup**: C++ Stories, Daily bit(e) of C++ (20-35% 개선 benchmark)
2. **[[likely]]/[[unlikely]]**: John Farrier (branch prediction 10-30 cycles = 10-30ns)
3. **ASIO Pattern**: evpp vs ASIO (5-10% 차이, 고동시성 환경)

---

**문서 작성**: Claude Opus 4.5
**최종 검토**: 2026-01-15
**상태**: 자동 야간 실행 대상
