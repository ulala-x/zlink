# Proactor 패턴 최적화 계획서

**날짜:** 2026-01-16
**작성자:** Team
**프로젝트:** zlink v0.2.0+
**목표:** Proactor 패턴에 맞게 백프레셔 처리 및 입력 재개 로직 최적화

---

## 1. 현재 문제 분석

### 1.1 배경

zlink는 ASIO (Proactor 패턴) 기반 I/O 엔진을 사용하지만, 내부 로직은 여전히 libzmq의 Reactor 스타일 구조를 유지하고 있습니다. 특히 백프레셔(backpressure) 처리 및 입력 재개(restart_input) 로직이 Proactor 패턴의 철학과 맞지 않아 불필요한 syscall 오버헤드가 발생하고 있습니다.

### 1.2 Phase 1 분석 결과 요약

**벤치마크 환경:**
- 패턴: ROUTER_ROUTER
- Transport: TCP
- Message Size: 64B
- Messages: ~3,600

**Syscall 비교 (zlink vs libzmq):**

| Metric | zlink | libzmq | 차이 |
|--------|-------|--------|------|
| recvfrom 총 호출 | 5,631 | 3,633 | **+55%** |
| recvfrom EAGAIN | 2,010 (35.7%) | 9 (0.25%) | **+22,233%** |
| epoll_wait | 3,644 | 3,644 | 동일 |
| Throughput | 442K msg/s | 421K msg/s | +5% |

**핵심 발견:**
- zlink는 libzmq 대비 recvfrom을 **55% 더 많이** 호출
- EAGAIN 에러가 **223배 더 자주** 발생 (2,010 vs 9)
- 불필요한 syscall 오버헤드로 인한 성능 손실 추정: **12.3%**

### 1.3 Proactor vs Reactor 패턴 차이

#### Reactor 패턴 (libzmq)
```
[epoll 알림] → [EAGAIN까지 동기 읽기 루프] → [처리] → [대기]
```
- **특징:** epoll이 "readable"을 알려주면 **EAGAIN까지 읽어서 버퍼를 비움**
- **장점:** 한 번의 epoll 알림으로 여러 메시지 처리 가능
- **EAGAIN 빈도:** 매우 낮음 (초기화 시점만 발생)

#### Proactor 패턴 (ASIO)
```
[비동기 read 요청] → [I/O 완료] → [콜백 호출] → [처리] → [다음 read 요청]
```
- **특징:** I/O 완료 후 **콜백으로 데이터 전달**
- **장점:** 동기 대기 없이 비동기 처리
- **이상적인 동작:** I/O가 완료되면 알려줌 → **준비되지 않았을 때는 호출하지 않음**

### 1.4 zlink의 잘못된 Proactor 사용

현재 zlink는 Proactor 패턴을 사용하지만, **Reactor처럼 동작**하려다 실패하고 있습니다.

**문제 1: 즉시 재시도 (백프레셔 해제 시)**

```cpp
// src/asio/asio_engine.cpp:848-905 (restart_input)
bool zmq::asio_engine_t::restart_input ()
{
    // ... 버퍼에 남은 데이터 처리 ...

    if (rc == -1 && errno == EAGAIN)
        _session->flush ();
    // ... error handling ...
    else {
        _input_stopped = false;
        _session->flush ();

        start_async_read ();  // ← 문제: 무조건 즉시 read 재시도
    }

    return true;
}
```

**호출 경로:**
```
process_input()
  → push_msg() 실패 (session 버퍼 가득)
  → _input_stopped = true
  → (나중에) write_activated() 호출 (session_base.cpp:261)
  → restart_input()
  → start_async_read() 무조건 호출
  → 소켓에 데이터 없음 → recvfrom() → EAGAIN
```

**문제 2: 핸드셰이크 중 즉시 재시도**

```cpp
// src/asio/asio_engine.cpp:468-477 (on_read_complete)
if (!process_input ()) {
    if (_handshaking && errno == EAGAIN) {
        if (_outsize > 0)
            start_async_write ();
        start_async_read ();  // ← 문제: EAGAIN 발생 직후 즉시 재시도
    }
    return;
}
```

**왜 문제인가?**
- Proactor는 "데이터가 준비되면 알려줌"이 핵심
- 하지만 zlink는 **"준비 안 되었는데 시도"를 반복**
- 매 시도마다 recvfrom syscall → EAGAIN → epoll 재등록
- **순수한 낭비:** 2,010회의 불필요한 syscall

### 1.5 strace 로그 패턴 비교

**zlink 패턴 (문제):**
```
recvfrom(8, ..., 8192, 0, NULL, NULL) = 66       # 성공
recvfrom(8, ..., 8192, 0, NULL, NULL) = -1 EAGAIN  # 즉시 실패!
recvfrom(7, ..., 8192, 0, NULL, NULL) = 66       # 성공
recvfrom(7, ..., 8192, 0, NULL, NULL) = -1 EAGAIN  # 즉시 실패!
epoll_wait(...)                                   # 다시 대기
```
→ 매 성공 직후 **즉시 다시 시도** → EAGAIN

**libzmq 패턴 (올바름):**
```
recvfrom(11, ..., 8192, 0, NULL, NULL) = 66  # 성공
recvfrom(12, ..., 8192, 0, NULL, NULL) = 66  # 성공
recvfrom(11, ..., 8192, 0, NULL, NULL) = 66  # 성공
...
```
→ **연속 성공**, EAGAIN 거의 없음

---

## 2. 최적화 목표

### 2.1 정량적 목표

| Metric | 현재 (zlink) | 목표 | 개선율 |
|--------|-------------|------|--------|
| recvfrom EAGAIN | 2,010 | **< 50** | **-98%** |
| 총 recvfrom 호출 | 5,631 | **~3,650** | **-35%** |
| Throughput | 442K msg/s | **460K+ msg/s** | **+4%** |
| EAGAIN 비율 | 35.7% | **< 1.4%** | libzmq 수준 |

### 2.2 정성적 목표

1. **Proactor 패턴 원칙 준수**
   - 비동기 I/O 완료 시에만 처리
   - 준비되지 않았을 때 즉시 재시도하지 않음

2. **불필요한 syscall 제거**
   - EAGAIN을 유발하는 투기적(speculative) read 제거
   - epoll이 알려줄 때까지 대기

3. **백프레셔 처리 개선**
   - `restart_input()` 호출 시 **버퍼에 남은 데이터만 처리**
   - 새로운 데이터가 필요하면 **비동기 I/O 완료를 기다림**

---

## 3. 구현 전략

### 3.1 전략 A: 백프레셔 재개 시 즉시 read 제거 (핵심)

**문제 코드:**
```cpp
// src/asio/asio_engine.cpp:897-901
else {
    _input_stopped = false;
    _session->flush ();

    start_async_read ();  // ← 제거 대상
}
```

**개선안:**
```cpp
else {
    _input_stopped = false;
    _session->flush ();

    // start_async_read() 제거 - 비동기 I/O 완료를 기다림
    // 새로운 데이터는 on_read_complete()에서 처리됨
}
```

**논리:**
- `restart_input()`은 **버퍼에 남은 데이터를 처리**하는 함수
- 버퍼를 다 처리했으면 **새 데이터를 기다려야 함**
- `start_async_read()`는 이미 `on_read_complete()`에서 호출됨 (line 482)
- 여기서 다시 호출하면 **중복 요청 → EAGAIN**

**영향:**
- EAGAIN 횟수: 2,010 → **< 100** (추정)
- recvfrom 호출: 5,631 → **~3,700** (추정)

### 3.2 전략 B: 핸드셰이크 EAGAIN 즉시 재시도 제거

**문제 코드:**
```cpp
// src/asio/asio_engine.cpp:468-477
if (!process_input ()) {
    if (_handshaking && errno == EAGAIN) {
        if (_outsize > 0)
            start_async_write ();
        start_async_read ();  // ← 조건부 제거
    }
    return;
}
```

**개선안:**
```cpp
if (!process_input ()) {
    if (_handshaking && errno == EAGAIN) {
        if (_outsize > 0)
            start_async_write ();
        // EAGAIN 시 즉시 read 재시도하지 않음
        // 다음 비동기 I/O 완료 시 처리
    }
    return;
}
```

**논리:**
- EAGAIN은 "아직 준비 안 됨"을 의미
- 즉시 재시도해도 똑같이 EAGAIN (무한 루프 위험)
- **다음 I/O 완료를 기다리는 것이 올바름**

**주의사항:**
- 핸드셰이크 단계에서 **메시지가 부분적으로 도착**할 수 있음
- 현재 `on_read_complete()` 로직이 부분 데이터 처리 가능 (line 448-464)
- 이 경우 `_insize > 0`이면 자동으로 `start_async_read()` 호출 (line 482)
- 따라서 **안전하게 제거 가능**

**영향:**
- 핸드셰이크 단계 EAGAIN: ~100회 → **< 10회** (추정)

### 3.3 전략 C: start_async_read 가드 강화

**현재 가드:**
```cpp
// src/asio/asio_engine.cpp:320-323
void zmq::asio_engine_t::start_async_read ()
{
    if (_read_pending || _input_stopped || _io_error)
        return;
    // ...
}
```

**개선안:**
```cpp
void zmq::asio_engine_t::start_async_read ()
{
    if (_read_pending || _input_stopped || _io_error)
        return;

    // 추가 가드: 버퍼에 처리할 데이터가 남아있으면 먼저 처리
    // (현재는 _insize > 0이어도 read를 요청하여 EAGAIN 유발)
    // 단, 이 가드는 조심스럽게 적용 - 무한 대기 방지 필요
    // ...
}
```

**논리:**
- `_insize > 0`인 경우: **버퍼에 부분 메시지가 남아있음**
  - 이 경우에는 **더 많은 데이터 필요** → read 허용
- `_insize == 0`인 경우: **버퍼가 비어있음**
  - restart_input()에서 호출 시: 불필요 (이미 처리 완료)
  - on_read_complete()에서 호출 시: 정상 (다음 메시지 대기)

**주의:**
- 이 가드는 **매우 신중하게 적용** 필요
- 잘못하면 데드락 가능성
- **전략 A가 핵심, 전략 C는 보조**

### 3.4 단계별 구현 계획

**Phase 1: 핵심 수정 (전략 A)**
1. `restart_input()`에서 `start_async_read()` 제거
2. 유닛 테스트 실행 (64개 테스트 통과 확인)
3. ROUTER_ROUTER 벤치마크 실행
4. EAGAIN 횟수 측정

**Phase 2: 핸드셰이크 개선 (전략 B)**
1. `on_read_complete()`에서 핸드셰이크 EAGAIN 즉시 재시도 제거
2. 유닛 테스트 재실행
3. 벤치마크 재측정

**Phase 3: 검증 및 최적화**
1. 전체 transport matrix 테스트 (tcp, ipc, inproc, ws, wss, tls)
2. 모든 소켓 패턴 테스트 (PAIR, PUB/SUB, ROUTER/DEALER, etc.)
3. strace 재측정 및 syscall 분석
4. 성능 벤치마크 비교

**Phase 4: 추가 최적화 (선택)**
1. 전략 C 적용 고려
2. 배칭 최적화 (한 번의 read에서 여러 메시지 처리)
3. 버퍼 크기 튜닝

---

## 4. 기술적 세부사항

### 4.1 영향받는 코드 경로

**1. 백프레셔 경로:**
```
session_base_t::write_activated()
  → asio_engine_t::restart_input()
  → (버퍼 내 데이터 처리)
  → start_async_read()  ← 제거 대상
```

**2. 정상 read 경로:**
```
start_async_read()
  → async_read_some()
  → on_read_complete()
  → process_input()
  → start_async_read()  ← 정상 (유지)
```

**3. 핸드셰이크 EAGAIN 경로:**
```
on_read_complete()
  → process_input() 실패 (EAGAIN)
  → start_async_read()  ← 조건부 제거
```

### 4.2 버퍼 관리 로직

**현재 상태:**
- `_inpos`: 현재 읽기 위치
- `_insize`: 남은 바이트 수
- `_read_buffer_ptr`: 읽기 버퍼 포인터
- `_input_in_decoder_buffer`: 데이터가 디코더 버퍼에 있는지 여부

**부분 메시지 처리:**
```cpp
// on_read_complete (line 448-464)
if (_decoder && _insize > 0) {
    // 부분 메시지가 있으면 새 데이터를 이어붙임
    _insize = partial_size + bytes_transferred;
} else {
    // 새 메시지 시작
    _inpos = _read_buffer_ptr;
    _insize = bytes_transferred;
}
```

**중요:**
- `_insize > 0`이면 **부분 메시지가 남아있음** → 더 읽어야 함
- `_insize == 0`이면 **완전히 처리됨** → 다음 I/O 대기

### 4.3 데드락 방지

**잠재적 데드락 시나리오:**
1. `restart_input()`에서 read를 호출하지 않으면
2. 버퍼에 남은 데이터가 없고
3. 새로운 I/O가 시작되지 않으면
4. **영원히 대기**

**방지 방법:**
- `on_read_complete()`의 끝에서 **항상** `start_async_read()` 호출 (line 482-483)
- 조건: `!_input_stopped`
- 따라서 `restart_input()`에서 `_input_stopped = false`만 설정하면
- 다음 I/O 완료 시 자동으로 read 재개

**현재 로직 분석:**
```cpp
// on_read_complete (line 479-483)
if (!_input_stopped)
    start_async_read ();
```
→ `restart_input()`이 `_input_stopped = false`로 설정하면
→ **기존 비동기 I/O가 완료되면** 자동으로 다음 read 시작
→ **문제 없음!**

### 4.4 테스트 커버리지

**영향받는 테스트:**
- `test_transport_matrix` (핵심)
  - PAIR × (tcp, ipc, inproc, ws, wss, tls)
  - PUB/SUB × (tcp, ipc, inproc, ws, wss, tls)
  - ROUTER/DEALER × (tcp, ipc, inproc)
  - ROUTER/ROUTER × (tcp, ipc, inproc)

- `test_pubsub_filter_xpub`
  - 백프레셔 시나리오 포함

- `test_router_multiple_dealers`
  - 여러 연결 동시 처리

**검증 항목:**
1. 모든 테스트 통과 (64개 중 59개, 5개 skip)
2. EAGAIN 횟수 < 50
3. recvfrom 호출 < 3,700
4. Throughput >= 460K msg/s

---

## 5. 성공 기준

### 5.1 기능 검증

- [ ] 유닛 테스트 64개 모두 통과 (5개 skip)
- [ ] Transport matrix 테스트 통과 (모든 조합)
- [ ] 백프레셔 시나리오 정상 동작
- [ ] 핸드셰이크 정상 완료

### 5.2 성능 기준

**필수 (Must-Have):**
- [ ] EAGAIN 횟수 < 50 (현재: 2,010)
- [ ] recvfrom 호출 < 3,700 (현재: 5,631)
- [ ] Throughput >= 현재 (442K msg/s) 유지

**목표 (Should-Have):**
- [ ] EAGAIN 횟수 < 20 (libzmq 수준)
- [ ] recvfrom 호출 ~3,650 (libzmq 수준)
- [ ] Throughput >= 460K msg/s (+4% 향상)

**이상적 (Nice-to-Have):**
- [ ] EAGAIN 비율 < 1% (현재: 35.7%)
- [ ] recvfrom 호출 < 3,650
- [ ] Throughput >= 470K msg/s (+6% 향상)

### 5.3 코드 품질

- [ ] 주석 추가 (왜 제거했는지 명확히)
- [ ] 디버그 로그 추가 (ASIO_ENGINE_DEBUG)
- [ ] 코드 리뷰 완료
- [ ] 문서 업데이트

---

## 6. 리스크 및 완화 방안

### 6.1 리스크 1: 데드락 가능성

**설명:**
`restart_input()`에서 read를 호출하지 않으면, 특정 상황에서 새로운 I/O가 시작되지 않아 무한 대기 가능성

**완화 방안:**
1. `on_read_complete()` 로직 정밀 분석 완료 (line 479-483)
   - `!_input_stopped`이면 **항상** `start_async_read()` 호출
   - `restart_input()`이 `_input_stopped = false` 설정
   - 따라서 **다음 I/O 완료 시 자동 재개**

2. 테스트 케이스 추가
   - 백프레셔 → 해제 → 메시지 수신 확인
   - Timeout 테스트 (무한 대기 검출)

3. 디버그 로그 추가
   ```cpp
   ENGINE_DBG("restart_input: cleared input_stopped, waiting for next I/O");
   ```

**확률:** 낮음 (현재 로직 분석 결과 안전)

### 6.2 리스크 2: 핸드셰이크 타임아웃

**설명:**
핸드셰이크 중 EAGAIN 시 즉시 재시도를 제거하면, 핸드셰이크가 완료되지 않을 가능성

**완화 방안:**
1. 부분 메시지 처리 로직 확인 완료 (line 448-464)
   - `_insize > 0`이면 자동으로 read 재개
   - 핸드셰이크 데이터는 보통 작아서 한 번에 도착

2. 핸드셰이크 타이머 활용
   - 기존 `_options.handshake_ivl` 유지
   - 타임아웃 시 에러 발생 (정상 동작)

3. 테스트 강화
   - 느린 네트워크 시뮬레이션
   - 부분 핸드셰이크 메시지 테스트

**확률:** 낮음 (기존 핸드셰이크 타이머 보호)

### 6.3 리스크 3: 성능 회귀

**설명:**
최적화 후 오히려 성능이 낮아질 가능성

**완화 방안:**
1. Phase별 성능 측정
   - 각 전략 적용 전후 벤치마크 실행
   - 회귀 발견 시 즉시 롤백

2. 다양한 워크로드 테스트
   - 작은 메시지 (64B)
   - 큰 메시지 (1KB, 64KB)
   - 버스트 패턴
   - 저지연 패턴

3. strace 비교 분석
   - syscall 패턴 시각화
   - 이상 패턴 조기 발견

**확률:** 낮음 (이론적으로 syscall 감소 = 성능 향상)

### 6.4 리스크 4: 에지 케이스 미발견

**설명:**
특정 소켓 패턴이나 transport에서만 발생하는 문제

**완화 방안:**
1. Transport matrix 전체 테스트
   - tcp, ipc, inproc (기본)
   - ws, wss, tls (TLS 포함)

2. 소켓 패턴 전체 테스트
   - PAIR, PUB/SUB, XPUB/XSUB
   - DEALER/ROUTER, ROUTER/ROUTER
   - Multiple dealers

3. CI/CD 통합
   - GitHub Actions에서 자동 테스트
   - 모든 플랫폼 (Linux, macOS, Windows)

**확률:** 중간 (복잡한 시스템)

---

## 7. 일정 및 마일스톤

### 7.1 Phase 1: 핵심 수정 (1일)

**Day 1 Morning:**
- [ ] `restart_input()` 수정 (전략 A 적용)
- [ ] 로컬 빌드 및 유닛 테스트
- [ ] ROUTER_ROUTER 벤치마크 + strace

**Day 1 Afternoon:**
- [ ] EAGAIN 횟수 분석
- [ ] 결과 문서화
- [ ] 코드 리뷰 준비

### 7.2 Phase 2: 핸드셰이크 개선 (0.5일)

**Day 2 Morning:**
- [ ] `on_read_complete()` 수정 (전략 B 적용)
- [ ] 유닛 테스트 재실행
- [ ] 벤치마크 재측정

### 7.3 Phase 3: 검증 (0.5일)

**Day 2 Afternoon:**
- [ ] Transport matrix 전체 테스트
- [ ] 모든 소켓 패턴 테스트
- [ ] 성능 결과 문서화

### 7.4 Phase 4: 문서화 및 배포 (0.5일)

**Day 3:**
- [ ] CLAUDE.md 업데이트
- [ ] 결과 문서 작성 (results.md)
- [ ] PR 생성 및 리뷰
- [ ] 머지 후 릴리스 노트 작성

**총 예상 소요 시간:** 2.5일

---

## 8. 성공 사례 시나리오

### 8.1 이상적인 결과

**Before (현재):**
```
[strace output]
recvfrom(8, ...) = 66       # 성공
recvfrom(8, ...) = -1 EAGAIN  # 실패!
recvfrom(7, ...) = 66       # 성공
recvfrom(7, ...) = -1 EAGAIN  # 실패!
epoll_wait(...)

Total: 5,631 recvfrom, 2,010 EAGAIN (35.7%)
Throughput: 442K msg/s
```

**After (목표):**
```
[strace output]
recvfrom(11, ...) = 66  # 성공
recvfrom(12, ...) = 66  # 성공
recvfrom(11, ...) = 66  # 성공
recvfrom(12, ...) = 66  # 성공
epoll_wait(...)

Total: 3,650 recvfrom, < 20 EAGAIN (< 1%)
Throughput: 465K msg/s
```

**개선:**
- recvfrom: **-35%** (5,631 → 3,650)
- EAGAIN: **-99%** (2,010 → < 20)
- Throughput: **+5%** (442K → 465K)

### 8.2 벤치마크 비교

| Metric | Before | After | libzmq | zlink vs libzmq |
|--------|--------|-------|--------|-----------------|
| recvfrom | 5,631 | 3,650 | 3,633 | **+0.5%** |
| EAGAIN | 2,010 | 20 | 9 | 2x (허용 범위) |
| Throughput | 442K | 465K | 421K | **+10%** |

→ **libzmq 수준의 syscall 효율성 달성**
→ **ASIO의 비동기 성능 장점 활용**
→ **Best of both worlds!**

---

## 9. 참고 자료

### 9.1 관련 문서

- `docs/team/20260116_syscall-optimization/phase1_results.md` - Phase 1 분석 결과
- `docs/team/20260116_syscall-optimization/plan.md` - 전체 최적화 계획
- `/tmp/claude/-home-ulalax-project-ulalax-zlink/tasks/b157497.output` - Codex의 이전 분석

### 9.2 핵심 코드 파일

- `src/asio/asio_engine.cpp` - 주요 수정 대상
  - Line 848-905: `restart_input()` 함수
  - Line 468-477: 핸드셰이크 EAGAIN 처리
  - Line 420-484: `on_read_complete()` 함수

- `src/session_base.cpp` - 백프레셔 호출 위치
  - Line 261: `write_activated()` → `restart_input()` 호출

### 9.3 벤치마크 명령어

```bash
# 벤치마크 빌드
cd benchwithzmq
./run_benchmarks.sh --pattern ROUTER_ROUTER --runs 1 --zlink-only

# syscall 프로파일링
strace -f -c -e trace=read,write,sendto,recvfrom,epoll_wait \
  taskset -c 1 ./build/bench/bin/comp_zlink_router_router zlink tcp 64

# 상세 로그
strace -f -e trace=recvfrom \
  taskset -c 1 ./build/bench/bin/comp_zlink_router_router zlink tcp 64 \
  2>&1 | tee /tmp/zlink_recvfrom_after.log
```

### 9.4 디버그 빌드

```bash
# ASIO 디버그 로그 활성화
cmake -B build -DZMQ_ASIO_DEBUG=1 -DBUILD_TESTS=ON
cmake --build build
cd build && ctest --output-on-failure
```

---

## 10. 결론

현재 zlink의 ASIO 엔진은 Proactor 패턴을 사용하지만, Reactor 스타일의 즉시 재시도 로직으로 인해 불필요한 syscall 오버헤드가 발생하고 있습니다.

**핵심 문제:**
- `restart_input()`에서 무조건 `start_async_read()` 호출 → EAGAIN 2,010회
- 핸드셰이크 중 EAGAIN 즉시 재시도 → 추가 EAGAIN

**해결 방법:**
- 버퍼 처리 완료 후 **비동기 I/O 완료를 기다림**
- Proactor 패턴 원칙 준수: "준비되면 알려줌"

**예상 효과:**
- EAGAIN 99% 감소 (2,010 → < 20)
- recvfrom 35% 감소 (5,631 → 3,650)
- Throughput 5% 향상 (442K → 465K)

이 최적화를 통해 zlink는 **libzmq 수준의 syscall 효율성**과 **ASIO의 비동기 성능 장점**을 모두 달성할 수 있을 것으로 기대됩니다.

---

**Next Steps:**
1. Phase 1 구현 시작 (`restart_input()` 수정)
2. 벤치마크 측정 및 검증
3. Phase 2-4 순차 진행
