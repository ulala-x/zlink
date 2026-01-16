# 중대 발견: Codex Plan의 데드락 문제

**날짜:** 2026-01-16
**발견자:** Senior Backend Developer (dev-ko-h)
**상태:** Codex 계획 실행 중단 필요

---

## 요약

Codex와 Gemini가 승인한 Phase 1 계획(`restart_input()`에서 `start_async_read()` 제거)은 **데드락을 유발**합니다.

---

## 데드락 시나리오

### 백프레셔 발생 시

1. `on_read_complete()` 호출 (데이터 수신 완료)
2. `process_input()` 실행:
   - `process_msg()` 실패 (EAGAIN - 백프레셔)
   - `_input_stopped = true` 설정 (`asio_engine.cpp:609`)
   - 하지만 `return true` (정상 종료)
3. `on_read_complete()` 종료 부분 (line 482-483):
   ```cpp
   if (!_input_stopped)  // false이므로 스킵
       start_async_read ();  // 호출되지 않음!
   ```
4. **결과: 펜딩 read 없음**

### 백프레셔 해제 시 (Codex Plan 적용 시)

5. `write_activated()` → `restart_input()` 호출
6. `_input_stopped = false` 설정
7. Codex 제안대로 `start_async_read()` **호출하지 않음**
8. **결과: 펜딩 read 여전히 없음 → 새 데이터 수신 불가 → 데드락**

---

## EAGAIN의 실제 원인

### Proactor 패턴의 정상 동작

ASIO의 `async_read_some()`은 최적화를 위해:
1. 먼저 동기적으로 `recvfrom()` 시도
2. **EAGAIN이면** epoll에 등록하고 대기
3. 데이터가 있으면 즉시 콜백 호출

이것이 strace에서 EAGAIN이 보이는 이유입니다. **Proactor에서 EAGAIN은 내부 최적화의 일부**입니다.

### Reactor vs Proactor 혼재 구조

현재 코드는:
- **libzmq의 Reactor 방식** (epoll → read until EAGAIN → process)
- **ASIO로 감싼 Proactor 방식** (async_read → callback → process)
- 이 혼재 구조에서 EAGAIN은 **필연적**

---

## 시니어 개발자 제안

### 방안 A: 현재 구조 유지

**장점:**
- 코드 안정성 유지
- 작은 수정만으로 가능

**단점:**
- EAGAIN 2,010개는 여전히 발생 (하지만 이것은 정상 동작)
- 근본적인 최적화 불가

**현실적 선택:**
- EAGAIN을 "버그"가 아닌 "Proactor의 정상 동작"으로 받아들임
- strace 프로파일링 시 EAGAIN 횟수는 무시
- 실제 성능 지표(throughput, latency)에만 집중

### 방안 B: True Proactor 재설계

**구조 변경:**
1. 백프레셔 발생 시에도 **펜딩 read를 유지** (취소하지 않음)
2. 데이터가 도착하면 `on_read_complete()` 콜백
3. `_input_stopped` 체크 → **버퍼에 저장하고 처리 안 함**
4. 백프레셔 해제 시 버퍼의 데이터 처리

**장점:**
- 진정한 Proactor 패턴
- EAGAIN 대폭 감소
- 더 효율적인 I/O

**단점:**
- **대규모 코드 변경** 필요
- 위험도 높음
- 예상 기간: 1-2주

---

## 결론 및 권장 사항

### Codex Plan 실행 중단

- ❌ `restart_input()`에서 `start_async_read()` 제거 → 데드락
- ❌ 핸드셰이크 경로 수정 → 부분 메시지 처리 문제 가능성

### 실행 가능한 대안

**Option 1: IPC 버그 수정으로 전환 (권장)**
- Phase 2 (IPC 0.00 M/s 버그)가 더 긴급
- 실제 기능이 동작하지 않는 문제
- EAGAIN은 "정상 동작"으로 수용

**Option 2: Senior Analysis의 Read Coalescing 구현**
- Phase 3 항목
- `on_read_complete()` 내부에서 루프로 여러 메시지 처리
- EAGAIN 횟수는 그대로, 핸들러 호출 오버헤드만 감소

**Option 3: 방안 B (True Proactor) 재설계**
- 장기 프로젝트로 전환
- 1-2주 소요
- 위험도 높음
- 사용자 승인 필요

---

## 참고 문서

- 시니어 분석: Agent a2ccbcd
- Codex 계획: `docs/team/20260116_proactor-optimization/plan.md`
- Gemini 리뷰: `docs/team/20260116_proactor-optimization/review.md`
- 코드: `src/asio/asio_engine.cpp` (line 482-483, 609, 897-905)

---

## 다음 단계

사용자에게 다음을 보고하고 지시를 받아야 합니다:

1. Codex Plan은 데드락을 유발하므로 실행 불가
2. EAGAIN 2,010개는 Proactor의 정상 동작 (버그 아님)
3. 다음 중 선택:
   - Option 1: IPC 버그 수정으로 전환
   - Option 2: Read Coalescing 구현
   - Option 3: True Proactor 재설계 (장기 프로젝트)
