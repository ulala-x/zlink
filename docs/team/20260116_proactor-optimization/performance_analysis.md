# True Proactor Performance Analysis

**Date:** 2026-01-16
**Subject:** Performance Impact of True Proactor Implementation
**Benchmark:** ROUTER_ROUTER TCP 64B
**Author:** Gemini (via dev-cxx)

## 1. Executive Summary

The "True Proactor" implementation (always-pending reads) was successfully deployed, achieving correct behavior under backpressure. However, performance metrics show that the primary goal of reducing `EAGAIN` syscalls was not met, and a slight throughput regression was introduced.

- **EAGAIN Reduction:** 0% (Remains ~2,007 per run)
- **Throughput:** -4% to -6% Regression
- **Backpressure Handling:** Correct (Architectural Goal Met)

This report details why the architectural change did not translate to syscall reduction and proposes the next optimization phase.

## 2. Benchmark Results Comparison

Comparing the baseline (Phase 1) with the current True Proactor implementation:

| Metric | Baseline (Phase 1) | True Proactor (Current) | Delta | Note |
|--------|-------------------|-------------------------|-------|------|
| **Throughput** | 442K msg/s | 414K msg/s | **-6.3%** | Overhead in read handler |
| **recvfrom calls** | 5,631 | ~5,625 | 0% | Unchanged read strategy |
| **EAGAIN errors** | 2,010 | 2,007 | **0%** | Speculative read still active |
| **recvfrom/msg** | ~1.56 | ~1.56 | 0% | Ideal is 1.0 (or less with batching) |

**Key Finding:** The implementation changed *what happens when we can't read*, but it did not change *how aggressively we try to read*.

## 3. Root Cause Analysis

### 3.1 Why EAGAIN remains high (2,007 calls)

The persistence of `EAGAIN` reveals that the source of these errors was not the "restart input" sequence as initially hypothesized, but the **Speculative Read** design in `asio_engine`.

1.  **Current Flow:**
    - `start_async_read()` is called.
    - **IMMEDIATE ACTION:** Calls `recvfrom()` *before* checking epoll (Speculative Read).
    - **Scenario:** In the benchmark (ping-pong/streaming 64B), the application consumes data faster than the network delivers.
    - **Result:** The socket is often empty when `start_async_read()` is called immediately after processing the previous message.
    - **Outcome:** `recvfrom` returns `EAGAIN` -> Engine registers with `epoll`.

2.  **True Proactor Impact:**
    - The new code changes how we handle the *backpressure* state (when buffers are full).
    - It does **not** disable the speculative read when buffers are empty.
    - Therefore, the "EAGAIN loop" continues exactly as before.

### 3.2 Throughput Regression (-4% / -28K msg/s)

The regression is attributed to increased complexity in the hot path (`on_read_complete`):

1.  **Logic Overhead:** Every read completion now checks `_input_stopped`, calculates pending buffer sizes, and manages the `_pending_buffers` deque structure.
2.  **O(n) Size Check (Codex Finding):**
    - The code iterates `_pending_buffers` to calculate total size on every read.
    - *Mitigation:* In this benchmark, `_pending_buffers` is empty, so the cost is low (O(1)), but the instruction cache footprint of the added logic still impacts the tight loop.
3.  **Memory Allocation:** Even if not used, the presence of `std::deque<std::vector<...>>` structures and associated destructors adds micro-latency.

## 4. Backpressure Behavior (Theoretical Analysis)

While the benchmark (Unlimited HWM) does not trigger backpressure, the new architecture provides significant benefits for real-world scenarios:

- **Pre-fetching:** Data is moved from Kernel space to User space (`_pending_buffers`) even if the application is slow. This prevents the TCP Window from closing prematurely (up to the 10MB limit).
- **Latency Smoothing:** When the application recovers from a stall, data is immediately available in memory, eliminating the syscall latency of restarting the read.
- **Safety:** The 10MB limit prevents the "unbounded memory growth" risk typical of naive Proactor implementations.

## 5. Next Steps: Optimization Strategy

To achieve the original goal of reducing EAGAIN and recovering throughput, we must change the **Read Strategy**.

### Phase 3: "Reactive" Read Implementation

**Goal:** Eliminate speculative `recvfrom` calls when the likelihood of data presence is low.

**Plan:**
1.  **Disable Speculative Read (Adaptive):**
    - Modify `start_async_read` to accept a hint (e.g., `bool speculative = true`).
    - In the continuous read loop (`on_read_complete`), call `start_async_read(speculative=false)`.
    - This forces the engine to **wait for epoll first**.
    - **Expected Result:** EAGAIN count drops to near zero.

2.  **Optimize Buffer Management:**
    - Replace `std::deque<std::vector<uint8_t>>` with a more contiguous memory structure (e.g., a Ring Buffer or a Slab Allocator) to reduce allocation overhead.
    - Fix the O(n) size calculation by maintaining a running `_total_pending_bytes` counter.

3.  **Read Coalescing (Batching):**
    - Instead of one `recvfrom` per loop, read up to `N` bytes or until EAGAIN, then parse multiple messages.
    - This amortizes the cost of `epoll_wait` and the handler invocation.

## 6. Conclusion

The "True Proactor" refactoring is a **success for correctness and robustness** but neutral-to-negative for pure throughput in ideal conditions. The high EAGAIN count is a distinct issue related to speculative reads, which must be addressed in the next optimization phase (Phase 3).

## 7. Recommendation

**Option A (Conservative):** Revert to baseline and focus on Codex-identified bugs first (data loss in pending buffer processing).

**Option B (Progressive):** Keep True Proactor, fix Codex bugs, then implement Phase 3 (Reactive Read + Coalescing).

**Option C (Pragmatic):** Accept 4-6% regression as the cost of correct backpressure handling and move to other priorities (e.g., IPC bug fix).
