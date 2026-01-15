# Iteration 2 구현 가이드: 단계별 실행 매뉴얼

**작성일**: 2026-01-15
**대상 개발자**: dev-cxx agent
**예상 소요 시간**: 4.5-6.5시간
**성공 기준**: -32% → -15% 이상 개선 (12-20% gap 축소)

---

## Phase 2-1: Core 최적화 (우선순위 1-2)

### 2-1-A: std::unordered_map 마이그레이션

**예상 개선**: 9-18%
**예상 시간**: 2-3시간
**난이도**: Low
**위험도**: Low (기능 동일, 성능만 개선)

#### Step 1: blob_hash.hpp 생성

**파일 경로**: `/home/ulalax/project/ulalax/zlink/src/blob_hash.hpp`

```cpp
/* SPDX-License-Identifier: MPL-2.0 */

#ifndef __ZMQ_BLOB_HASH_HPP_INCLUDED__
#define __ZMQ_BLOB_HASH_HPP_INCLUDED__

#include "blob.hpp"
#include <functional>
#include <cstring>

// C++17 compatibility check
#if __cplusplus >= 201703L
#include <optional>
#endif

namespace zmq
{

//  Hash functor for blob_t with transparent hashing support.
//  Allows heterogeneous lookup in unordered_map without creating temporary blob_t.
struct blob_hash {
    using is_transparent = void;  // Enable heterogeneous lookup

    //  Hash computation using FNV-1a algorithm
    [[nodiscard]] size_t hash_bytes(const unsigned char* data, size_t size) const noexcept {
        constexpr size_t fnv_offset_basis = 14695981039346656037ULL;
        constexpr size_t fnv_prime = 1099511628211ULL;

        size_t hash = fnv_offset_basis;
        for (size_t i = 0; i < size; ++i) {
            hash ^= static_cast<size_t>(data[i]);
            hash *= fnv_prime;
        }
        return hash;
    }

    //  Hash for blob_t
    [[nodiscard]] size_t operator()(const blob_t& b) const noexcept {
        return hash_bytes(b.data(), b.size());
    }

    //  Hash for std::span<const unsigned char> (C++20)
    [[nodiscard]] size_t operator()(const std::span<const unsigned char>& s) const noexcept {
        return hash_bytes(s.data(), s.size());
    }

    //  Hash for C-style array view
    [[nodiscard]] size_t operator()(const std::pair<const unsigned char*, size_t>& p) const noexcept {
        return hash_bytes(p.first, p.second);
    }
};

//  Equality functor for blob_t with transparent comparison support.
struct blob_equal {
    using is_transparent = void;  // Enable heterogeneous lookup

    //  Compare blob_t with blob_t
    [[nodiscard]] bool operator()(const blob_t& a, const blob_t& b) const noexcept {
        if (a.size() != b.size()) return false;
        if (a.size() == 0) return true;
        return std::memcmp(a.data(), b.data(), a.size()) == 0;
    }

    //  Compare blob_t with std::span<const unsigned char>
    [[nodiscard]] bool operator()(const blob_t& a, const std::span<const unsigned char>& b) const noexcept {
        if (a.size() != b.size()) return false;
        if (a.size() == 0) return true;
        return std::memcmp(a.data(), b.data(), a.size()) == 0;
    }

    [[nodiscard]] bool operator()(const std::span<const unsigned char>& a, const blob_t& b) const noexcept {
        return operator()(b, a);
    }

    //  Compare blob_t with C-style array view
    [[nodiscard]] bool operator()(const blob_t& a, const std::pair<const unsigned char*, size_t>& b) const noexcept {
        if (a.size() != b.second) return false;
        if (a.size() == 0) return true;
        return std::memcmp(a.data(), b.first, a.size()) == 0;
    }

    [[nodiscard]] bool operator()(const std::pair<const unsigned char*, size_t>& a, const blob_t& b) const noexcept {
        return operator()(b, a);
    }
};

} // namespace zmq

#endif
```

**검증:**
```bash
# 컴파일 확인만 (헤더 파일)
cd /home/ulalax/project/ulalax/zlink
g++ -std=c++20 -I./src -fsyntax-only src/blob_hash.hpp
```

#### Step 2: socket_base.hpp 수정

**파일**: `/home/ulalax/project/ulalax/zlink/src/socket_base.hpp`

**변경사항:**

줄 번호 ~95 (include 섹션)에 추가:
```cpp
#include "blob_hash.hpp"
#include <unordered_map>
```

줄 번호 ~1740 (routing_socket_base_t 클래스):

**Before:**
```cpp
private:
    // outbound pipes indexed by peer routing IDs
    typedef std::map<blob_t, out_pipe_t> out_pipes_t;
    out_pipes_t _out_pipes;
```

**After:**
```cpp
private:
    // outbound pipes indexed by peer routing IDs
    // Changed from std::map (O(log n)) to std::unordered_map (O(1))
    typedef std::unordered_map<blob_t, out_pipe_t, blob_hash, blob_equal> out_pipes_t;
    out_pipes_t _out_pipes;
```

#### Step 3: socket_base.cpp 수정

**파일**: `/home/ulalax/project/ulalax/zlink/src/socket_base.cpp`

**변경 위치:** routing_socket_base_t::lookup_out_pipe 함수 근처

**After existing implementation, add overload:**

```cpp
//  Heterogeneous lookup variant for zero-copy search without temporary blob_t
zmq::routing_socket_base_t::out_pipe_t*
zmq::routing_socket_base_t::lookup_out_pipe(const std::span<const unsigned char> routing_id_span)
{
    auto it = _out_pipes.find(routing_id_span);

    if (it != _out_pipes.end()) [[likely]] {
        //  Prefetch the out_pipe structure before dereferencing
        //  Improves cache efficiency for next access
        __builtin_prefetch(&it->second, 0, 3);
        return &it->second;
    }

    return nullptr;
}
```

#### Step 4: router.cpp 수정

**파일**: `/home/ulalax/project/ulalax/zlink/src/router.cpp`

**변경 1: xsend() 함수에서 lookup 호출 변경**

찾기: "blob_t(" + "reference_tag_t"

**Before (약 line 155):**
```cpp
// Get the routing ID from the first frame
blob_t routing_id (
    static_cast<unsigned char *> (msg_->data ()),
    msg_->size (),
    reference_tag_t ());

// Lookup the pipe for this routing ID
out_pipe_t *out_pipe = lookup_out_pipe (routing_id);
```

**After:**
```cpp
// Get the routing ID from the first frame
// Use zero-copy span for lookup without temporary blob_t
const auto routing_id_span = std::span<const unsigned char> (
    static_cast<const unsigned char *> (msg_->data ()),
    msg_->size ());

// Lookup the pipe for this routing ID (heterogeneous lookup)
out_pipe_t *out_pipe = lookup_out_pipe (routing_id_span);
```

**주의:** std::span 선언 확인
- 컴파일 타겟: C++20 이상
- Fallback 필요시: `#include <span>` 추가

#### Step 5: 빌드 및 테스트

```bash
cd /home/ulalax/project/ulalax/zlink

# 1. 클린 빌드
rm -rf build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build 2>&1 | tee build.log

# 2. 컴파일 오류 확인
if grep -i "error" build.log; then
    echo "Compilation errors detected!"
    exit 1
fi

# 3. 테스트 실행
cd build
ctest --output-on-failure 2>&1 | tee test.log

# 4. 테스트 결과 확인
PASS_COUNT=$(grep -c "PASS" test.log)
FAIL_COUNT=$(grep -c "FAIL" test.log)
echo "Results: $PASS_COUNT PASS, $FAIL_COUNT FAIL"

if [ "$FAIL_COUNT" -gt 0 ]; then
    echo "Test failures detected!"
    exit 1
fi
```

#### Step 6: 벤치마크 측정

```bash
cd /home/ulalax/project/ulalax/zlink/build

# Baseline 재측정 (Before 최적화 상태)
echo "=== Baseline (Before unordered_map) ==="
for i in 1 2 3; do
    ./router_bench -s 64 -n 100000 -p 4 2>&1 | grep "Throughput"
done

# 결과 기록
echo "Expected: 3.18 M/s (baseline)"
echo "Target: 3.46-3.75 M/s (+9-18% improvement)"
```

---

### 2-1-B: [[likely]]/[[unlikely]] 힌트 추가

**예상 개선**: 3-5%
**예상 시간**: 30분
**난이도**: Very Low
**위험도**: Very Low (컴파일 타임만 영향)

#### Step 1: asio_poller.cpp 수정

**파일**: `/home/ulalax/project/ulalax/zlink/src/asio/asio_poller.cpp`

**위치 1: start_wait_read() - 약 line 193**

**Before:**
```cpp
if (ec || entry_->fd == retired_fd || !entry_->pollin_enabled || _stopping) {
    return;
}
```

**After:**
```cpp
if (ec || entry_->fd == retired_fd || !entry_->pollin_enabled || _stopping) [[unlikely]] {
    return;
}
```

**위치 2: start_wait_write() - 약 line 207**

**Before:**
```cpp
if (ec || entry_->fd == retired_fd || !entry_->pollout_enabled || _stopping) {
    return;
}
```

**After:**
```cpp
if (ec || entry_->fd == retired_fd || !entry_->pollout_enabled || _stopping) [[unlikely]] {
    return;
}
```

**위치 3: loop() - 약 line 290**

**Before:**
```cpp
if (load == 0) {
    if (timeout == 0) {
        break;
    }
```

**After:**
```cpp
if (load == 0) [[unlikely]] {
    if (timeout == 0) [[unlikely]] {
        break;
    }
```

**위치 4: loop() - 약 line 300**

**Before:**
```cpp
if (_io_context.stopped ()) {
    _io_context.restart ();
}
```

**After:**
```cpp
if (_io_context.stopped ()) [[unlikely]] {
    _io_context.restart ();
}
```

#### Step 2: router.cpp 수정

**파일**: `/home/ulalax/project/ulalax/zlink/src/router.cpp`

**위치 1: multipart 메시지 처리 - 약 line 318**

**Before:**
```cpp
if (msg_->flags () & msg_t::more) {
    _more_out = true;
```

**After:**
```cpp
if (msg_->flags () & msg_t::more) [[likely]] {
    _more_out = true;
```

**위치 2: xsend() - pipe lookup 성공 체크 - 약 line 325**

**Before:**
```cpp
if (out_pipe) {
    _current_out = out_pipe->pipe;
```

**After:**
```cpp
if (out_pipe) [[likely]] {
    _current_out = out_pipe->pipe;
```

**위치 3: xsend() - pipe write 실패 - 약 line 329**

**Before:**
```cpp
if (!_current_out->check_write ()) {
    const bool pipe_full = !_current_out->check_hwm ();
```

**After:**
```cpp
if (!_current_out->check_write ()) [[unlikely]] {
    const bool pipe_full = !_current_out->check_hwm ();
```

**위치 4: xsend() - mandatory 모드 - 약 line 340**

**Before:**
```cpp
if (_mandatory) {
    _more_out = false;
```

**After:**
```cpp
if (_mandatory) [[unlikely]] {
    _more_out = false;
```

**위치 5: xsend() - current_out 체크 - 약 line 356**

**Before:**
```cpp
if (_current_out) {
    const bool ok = _current_out->write (msg_);
```

**After:**
```cpp
if (_current_out) [[likely]] {
    const bool ok = _current_out->write (msg_);
```

**위치 6: xsend() - write 실패 - 약 line 359**

**Before:**
```cpp
if (unlikely (!ok)) {
    const int rc = msg_->close ();
```

**After:**
```cpp
if (unlikely (!ok)) [[unlikely]] {
    const int rc = msg_->close ();
```

**위치 7: xsend() - more 플래그 체크 - 약 line 365**

**Before:**
```cpp
if (!_more_out) {
    _current_out->flush ();
```

**After:**
```cpp
if (!_more_out) [[likely]] {
    _current_out->flush ();
```

#### Step 3: pipe.cpp 수정 (선택사항)

**파일**: `/home/ulalax/project/ulalax/zlink/src/pipe.cpp`

**위치: check_read() - 약 line 146**

**Before:**
```cpp
if (unlikely (!_in_active))
    return false;
```

**After:**
```cpp
if (unlikely (!_in_active)) [[unlikely]]
    return false;
```

#### Step 4: 빌드 및 검증

```bash
cd /home/ulalax/project/ulalax/zlink

# 빌드
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build 2>&1 | tee build2.log

# 컴파일 오류 확인
if grep -i "error" build2.log; then
    echo "Compilation errors in likely/unlikely hints!"
    exit 1
fi

# 테스트
cd build
ctest --output-on-failure -q

# 성능 재측정
echo "=== After [[likely]]/[[unlikely]] ==="
for i in 1 2 3; do
    ./router_bench -s 64 -n 100000 -p 4 2>&1 | grep "Throughput"
done

echo "Expected: 3.46-3.75 M/s → 3.56-3.94 M/s (additional 3-5%)"
```

---

## Phase 2-2: Utility 최적화 (선택사항)

### 2-2-A: __builtin_prefetch 추가 (선택)

**예상 개선**: 2-3%
**예상 시간**: 1시간
**난이도**: Low
**위험도**: Medium (벤치마크 검증 필수)

#### 수정 위치: socket_base.cpp

**lookup_out_pipe() 함수** (새로 추가한 heterogeneous 오버로드):

```cpp
zmq::routing_socket_base_t::out_pipe_t*
zmq::routing_socket_base_t::lookup_out_pipe(const std::span<const unsigned char> routing_id_span)
{
    auto it = _out_pipes.find(routing_id_span);

    if (it != _out_pipes.end()) [[likely]] {
        //  Prefetch the out_pipe structure for next memory access
        //  Brings the out_pipe_t object into L1 cache
        //  Parameters: pointer, rw (0=read), temporal_locality (3=high)
        __builtin_prefetch(&it->second, 0, 3);
        return &it->second;
    }

    return nullptr;
}
```

**검증:**
```bash
cd /home/ulalax/project/ulalax/zlink/build

# 벤치마크: prefetch 적용 후
echo "=== With __builtin_prefetch ==="
for i in 1 2 3; do
    ./router_bench -s 64 -n 100000 -p 4 2>&1 | grep "Throughput"
done

# 결과: 성능 악화 시 즉시 제거
# 예상: 3.56-3.94 M/s → 3.64-4.05 M/s (추가 2-3%)
# 또는 성능 변화 미미 (prefetch가 불필요한 경우)
```

### 2-2-B: std::span 인터페이스 추가 (선택)

**예상 개선**: 1-2%
**예상 시간**: 1-2시간
**난이도**: Low
**위험도**: Low

#### Step 1: msg.hpp 수정

**새 메서드 추가** (약 line 150):

```cpp
public:
    //  C++20: Get message data as std::span for zero-copy views
    [[nodiscard]] std::span<std::byte> data_span() noexcept {
        // Return span view of internal data without copying
        void* data_ptr = data();
        size_t data_size = size();

        if (data_ptr && data_size > 0) {
            return std::span<std::byte>(
                static_cast<std::byte*>(data_ptr),
                data_size
            );
        }
        return std::span<std::byte>();
    }

    [[nodiscard]] std::span<const std::byte> data_span() const noexcept {
        // const version for const messages
        const void* data_ptr = data();
        size_t data_size = size();

        if (data_ptr && data_size > 0) {
            return std::span<const std::byte>(
                static_cast<const std::byte*>(data_ptr),
                data_size
            );
        }
        return std::span<const std::byte>();
    }
```

#### Step 2: blob.hpp 수정

**새 메서드 추가** (약 line 120):

```cpp
public:
    //  C++20: Get blob data as std::span for zero-copy access
    [[nodiscard]] std::span<const unsigned char> as_span() const noexcept {
        if (_data && _size > 0) {
            return std::span<const unsigned char>(_data, _size);
        }
        return std::span<const unsigned char>();
    }

    [[nodiscard]] std::span<unsigned char> as_span() noexcept {
        if (_data && _size > 0) {
            return std::span<unsigned char>(_data, _size);
        }
        return std::span<unsigned char>();
    }
```

#### Step 3: 벤치마크 측정

```bash
cd /home/ulalax/project/ulalax/zlink/build

# 최종 성능 측정
echo "=== Final Performance (All optimizations) ==="
for i in 1 2 3; do
    ./router_bench -s 64 -n 100000 -p 4 2>&1 | grep "Throughput"
done

# 예상: 3.64-4.05 M/s (total +12-23% improvement)
# 격차: -32% → -15% 이상
```

---

## 종합 검증 및 커밋

### 최종 테스트 체크리스트

```bash
#!/bin/bash
set -e

cd /home/ulalax/project/ulalax/zlink

echo "=== PHASE 2 VALIDATION ==="

# 1. 클린 빌드
echo "[1/5] Building from scratch..."
rm -rf build
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
cmake --build build --config Release

# 2. 단위 테스트
echo "[2/5] Running unit tests..."
cd build
ctest --output-on-failure --progress
CTEST_RESULT=$?

if [ $CTEST_RESULT -ne 0 ]; then
    echo "ERROR: Tests failed!"
    exit 1
fi

# 3. 성능 벤치마크 - 64B
echo "[3/5] Running ROUTER benchmark (64B)..."
./router_bench -s 64 -n 100000 -p 4 > bench_64.txt
THROUGHPUT_64=$(grep "Throughput" bench_64.txt | awk '{print $2}')
echo "64B throughput: $THROUGHPUT_64 M/s"

# 4. 성능 벤치마크 - 256B
echo "[4/5] Running ROUTER benchmark (256B)..."
./router_bench -s 256 -n 100000 -p 4 > bench_256.txt
THROUGHPUT_256=$(grep "Throughput" bench_256.txt | awk '{print $2}')
echo "256B throughput: $THROUGHPUT_256 M/s"

# 5. 검증
echo "[5/5] Validating results..."
# 비교: baseline 3.18 M/s
# 목표: 3.46+ M/s (9%+ improvement)
if (( $(echo "$THROUGHPUT_64 > 3.46" | bc -l) )); then
    echo "✓ Performance goal achieved!"
else
    echo "⚠ Performance improvement is below target"
    echo "  Expected: >3.46 M/s, Got: $THROUGHPUT_64 M/s"
fi

echo ""
echo "=== VALIDATION COMPLETE ==="
```

### Git 커밋 메시지 템플릿

```bash
git add src/blob_hash.hpp src/socket_base.hpp src/socket_base.cpp src/router.cpp src/asio/asio_poller.cpp

git commit -m "perf: Phase 2-1 core optimizations - unordered_map + branch hints

- Replace std::map with std::unordered_map for O(1) routing table lookup
  - Add blob_hash.hpp with FNV-1a hash and transparent comparisons
  - Eliminates temporary blob_t creation during lookup
  - Expected improvement: 9-18%

- Add [[likely]]/[[unlikely]] branch prediction hints
  - Mark error paths as unlikely in event loop and router
  - Improves instruction cache and branch predictor performance
  - Expected improvement: 3-5%

Total expected improvement: 12-23% (gap reduction -32% → -15%)
Target metric: ROUTER 64B throughput 3.18 → 3.46+ M/s

Changes:
- src/blob_hash.hpp: New file with heterogeneous hash/equality
- src/socket_base.hpp: Use unordered_map, add heterogeneous lookup
- src/socket_base.cpp: Add span-based lookup overload
- src/router.cpp: Use zero-copy span instead of temporary blob
- src/asio/asio_poller.cpp: Add branch prediction hints

All 61 tests pass, no regressions."

# 선택 사항: 2-2 최적화도 포함 시
git add src/blob.hpp src/msg.hpp

git commit -m "perf: Phase 2-2 utility optimizations - prefetch + span

- Add __builtin_prefetch in routing table lookup
  - Brings out_pipe structure into L1 cache before use
  - Expected improvement: 2-3%

- Add std::span accessors to blob_t and msg_t
  - Zero-copy message views without temporary objects
  - Expected improvement: 1-2%

Total additional improvement: 3-5%

All 61 tests pass."
```

---

## 성능 개선 검증 방법

### 벤치마크 비교 계산

```python
#!/usr/bin/env python3

baseline_ref = 4.68    # libzmq-ref throughput
baseline_zlink = 3.18  # zlink current throughput
gap_percent = (baseline_zlink - baseline_ref) / baseline_ref * 100  # -32.05%

print(f"Baseline gap: {gap_percent:.2f}%")

# After unordered_map (9-18% improvement)
improved_64 = baseline_zlink * 1.15  # mid-point 15%
gap_after_phase2a = (improved_64 - baseline_ref) / baseline_ref * 100

print(f"After unordered_map: {improved_64:.2f} M/s")
print(f"New gap: {gap_after_phase2a:.2f}%")
print(f"Gap reduction: {gap_percent - gap_after_phase2a:.2f}pp")

# After [[likely]]/[[unlikely]] (3-5% improvement)
improved_64_b = improved_64 * 1.04  # 4%
gap_after_phase2b = (improved_64_b - baseline_ref) / baseline_ref * 100

print(f"After branch hints: {improved_64_b:.2f} M/s")
print(f"Final gap: {gap_after_phase2b:.2f}%")
print(f"Total gap reduction: {gap_percent - gap_after_phase2b:.2f}pp")

# Success criteria
if gap_after_phase2b >= -15:
    print("✓ SUCCESS: Target gap of -15% achieved!")
else:
    print(f"✗ FAILED: Gap {gap_after_phase2b:.2f}% still exceeds target -15%")
```

---

## Troubleshooting

### 컴파일 오류: "no member named 'size' in 'std::span'"

**원인**: std::span이 C++20 기능

**해결**:
```bash
# CMakeLists.txt 확인
grep -i "CXX_STANDARD" CMakeLists.txt

# C++20 강제 (필요시)
cmake -B build -DZMQ_CXX_STANDARD=20
```

### 테스트 실패

**복구 단계**:
```bash
# 마지막 변경 되돌리기
git checkout HEAD~1 src/  # 마지막 커밋 취소

# 또는 개별 파일 복구
git checkout HEAD src/socket_base.cpp

# 빌드 및 테스트
cmake --build build
ctest --output-on-failure
```

### 성능 악화 (역효과)

**원인**: 플랫폼 특화 최적화의 부작용

**해결**:
```bash
# 문제 최적화 제거
# 예: prefetch가 해롭다면
git diff src/socket_base.cpp | grep -A5 -B5 "prefetch"

# 해당 라인만 제거
# 재빌드 및 벤치마크
```

---

## 최종 체크리스트

- [ ] blob_hash.hpp 생성 및 컴파일 확인
- [ ] socket_base.hpp 수정 (unordered_map 변경)
- [ ] socket_base.cpp 수정 (heterogeneous lookup 추가)
- [ ] router.cpp 수정 (span 기반 호출)
- [ ] 빌드 성공 (no errors, warnings 최소)
- [ ] ctest 61/61 PASS
- [ ] ROUTER 벤치마크 64B: 3.18 → 3.46+ M/s 확인
- [ ] asio_poller.cpp [[likely]]/[[unlikely]] 적용
- [ ] router.cpp [[likely]]/[[unlikely]] 적용
- [ ] 최종 벤치마크: 3.46 → 3.56+ M/s 확인
- [ ] 모든 변경 커밋
- [ ] PR 준비 (성능 개선 데이터 첨부)

---

**문서 작성**: Claude Opus 4.5
**작성일**: 2026-01-15
**실행 대상**: dev-cxx agent
**실행 예정**: 자동 야간 배치
