# Proactor 최적화 계획 검토 및 통합 제안서

**날짜:** 2026-01-16
**검토자:** Gemini (System Architect)
**대상 문서:**
1. `plan.md` (Codex: Proactor 패턴 수정 및 EAGAIN 감소)
2. `senior_analysis.md` (Senior Dev: 구조적 성능 최적화)

---

## 1. 개요 및 범위 비교

두 문서는 zlink의 성능을 개선하기 위해 서로 다른 관점에서 접근하고 있습니다.

| 항목 | Codex (plan.md) | Senior Developer (senior_analysis.md) |
|------|-----------------|-----------------------------------|
| **핵심 문제** | `recvfrom` EAGAIN 호출 과다 (2,000회 이상/벤치마크) | 구조적 비효율성 (Latency, Throughput) |
| **접근 방식** | **Bug Fix**: 잘못된 Proactor 패턴 구현 수정 (Logic level) | **Optimization**: 아키텍처 및 I/O 모델 개선 (Architectural level) |
| **주요 제안** | `restart_input` 시 무조건 `start_async_read` 호출 제거 | 1. Speculative Write (즉시 쓰기)<br>2. Zero-Copy Write<br>3. Read Coalescing (읽기 융합) |
| **예상 효과** | EAGAIN 99% 감소, syscall 효율성 증대 | Throughput/Latency 50~100% 향상 |

---

## 2. 기존 작업 확인 (Fact Check)

**중요 발견:** `docs/team/20260114_ASIO-성능개선/final_results_report.md` 및 코드 분석 결과, **Senior Developer가 제안한 Priority 1 항목들은 이미 구현되어 있습니다.**

### 구현 완료 항목

1. **Speculative Write (구현 완료 ✅)**
   - `src/asio/asio_engine.cpp`에 `speculative_write()` 함수가 존재하며, `write_some`을 통해 동기 쓰기를 우선 시도합니다.
   - Senior Analysis의 "Speculative Write 도입" 제안은 **중복**입니다.

2. **Zero-Copy Write (구현 완료 ✅)**
   - 이전 프로젝트 Phase 3에서 `Encoder Zero-Copy`가 구현되었습니다.
   - `write_some` 호출 시 인코더 버퍼를 직접 사용하는 로직이 확인되었습니다.

### 미해결 항목

3. **IPC Critical Issue (미해결 ⚠️)**
   - 1월 14일 보고서에 따르면 IPC Transport에서 작은 메시지(64B~1KB) 처리 시 **Throughput 0.00 M/s** 문제가 발생 중입니다.
   - Senior Analysis는 이 심각한 버그를 간과했습니다.

---

## 3. 우선순위 및 통합 제안

Codex의 계획은 "비정상적인 동작(EAGAIN Flood) 수정"이므로 가장 먼저 수행되어야 합니다. Senior의 계획 중 "Read Coalescing"은 유효한 추가 최적화입니다.

### 통합 로드맵

**Phase 1: Proactor 정상화 (Codex Plan) [즉시 실행 ⚡]**
- **목표:** 불필요한 `recvfrom` 루프 제거 (EAGAIN 99% 감소).
- **작업:** `restart_input()` 및 핸드셰이크 로직에서 `start_async_read()` 무조건 호출 제거.
- **이유:** 현재 동작은 CPU 사이클을 낭비하고 strace 분석을 어렵게 만드는 "버그"에 가깝습니다. 가장 먼저 해결해야 합니다.
- **예상 기간:** 1.0일
- **위험도:** 낮음

**Phase 2: IPC 버그 수정 (Previous Report) [긴급 🚨]**
- **목표:** IPC Transport의 0.00 M/s 문제 해결.
- **작업:** `src/asio/ipc_transport.cpp`의 `write_some` 구현체 디버깅.
- **이유:** 기능이 동작하지 않는 치명적 문제입니다.
- **예상 기간:** 2.0일
- **위험도:** 높음

**Phase 3: Read Coalescing (Senior Plan - Item 3) [후속 최적화 🔧]**
- **목표:** 읽기 핸들러 오버헤드 감소.
- **작업:** `on_read_complete` 내부에서 루프를 돌며 소켓 버퍼가 비워질 때까지 처리.
- **이유:** 이미 구현된 Speculative Write(쓰기 최적화)에 대응하는 읽기 측 최적화입니다.
- **예상 기간:** 3.0일
- **위험도:** 중간

---

## 4. 실행 가능성 및 일정

| 단계 | 담당 계획 | 예상 소요 | 위험도 | 비고 |
|------|-----------|-----------|--------|------|
| **Phase 1** | Codex | 1.0일 | 낮음 | **즉시 승인 및 실행 권장**. 코드가 명확하고 영향 범위가 국소적임. |
| **Phase 2** | (New) | 2.0일 | 높음 | IPC 이슈 원인 파악 필요. Phase 1 완료 직후 착수해야 함. |
| **Phase 3** | Senior | 3.0일 | 중간 | 구조 변경 필요. Phase 1, 2 안정화 후 진행. |

**총 예상 기간:** 6.0일

---

## 5. Senior Analysis 항목별 검토

| 순위 | 항목 | 상태 | 검토 의견 |
|------|------|------|-----------|
| **1** | Speculative Write | ✅ **구현 완료** | `20260114_ASIO-성능개선`에서 이미 구현됨 |
| **2** | Zero-Copy Write | ✅ **구현 완료** | 인코더 Zero-Copy 이미 적용됨 |
| **3** | Read Coalescing | 🔄 **검토 필요** | Phase 3에서 구현 고려 |
| 4 | Timer Integration | 📋 **후순위** | 성능 영향 검증 후 결정 |
| 5 | Custom Allocator | 📋 **후순위** | Micro-optimization, 필요성 재평가 |
| 6 | Strand 오버헤드 | 📋 **후순위** | 단일 스레드 환경이면 불필요 |

---

## 6. 결론 및 권고

### 주요 발견

1. **Senior Analysis의 오류 수정:**
   - Speculative Write와 Zero-Copy는 이미 구현된 상태임을 인지하고, 해당 항목을 제외합니다.
   - 실제로 필요한 새로운 최적화는 **Read Coalescing**뿐입니다.

2. **Codex 계획의 우수성:**
   - `plan.md`의 내용은 정확하며 즉시 실행 가능합니다.
   - `recvfrom` 폭주를 막는 것이 우선입니다.

3. **긴급 이슈 발견:**
   - IPC Transport의 0.00 M/s 버그는 즉시 해결이 필요합니다.

### 통합 실행 계획

**Step 1: Codex의 계획대로 `asio_engine.cpp` 수정 (EAGAIN 잡기)**
- 목표: EAGAIN 2,010 → < 50
- 기간: 1일
- **즉시 승인 및 실행 권장**

**Step 2: IPC 0 throughput 버그 수정 (이전 리포트 기반)**
- 목표: IPC 64B 메시지 처리 정상화
- 기간: 2일
- Phase 1 완료 후 즉시 착수

**Step 3: Read Coalescing 구현 검토**
- 목표: 핸들러 호출 오버헤드 감소
- 기간: 3일
- Phase 1, 2 안정화 후 진행

### 최종 승인

**Codex의 `plan.md`를 우선 실행하십시오.**

- ✅ 기술적 정확성 검증 완료
- ✅ 실행 가능성 확인
- ✅ 예상 효과 합리적
- ✅ 리스크 관리 적절
- ⚠️ Phase 2 (IPC 버그) 준비 필요

---

## 7. 참고 문서

- `docs/team/20260114_ASIO-성능개선/final_results_report.md` - Speculative Write, Zero-Copy 구현 확인
- `docs/team/20260116_syscall-optimization/phase1_results.md` - EAGAIN 문제 분석
- `src/asio/asio_engine.cpp` - 주요 수정 대상 파일
- `src/asio/ipc_transport.cpp` - IPC 버그 수정 대상
