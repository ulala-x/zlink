# ZMTP Lite -> ZMP Format Transition Plan

**Date:** 2026-01-21  
**Owner:** 팀장님  
**Author:** Codex  
**Scope:** ZMTP hot path slimming, then ZMP wire format conversion  
**Primary Goal:** Max performance while preserving ZMQ socket patterns

---

## 1. Goals

1) Keep ZMQ socket patterns (PAIR/DEALER/ROUTER/PUB/SUB/XPUB/XSUB) intact.
2) Preserve the fastest proven path (ZMTP hot path) while removing unnecessary work.
3) Transition to ZMP wire format only after ZMTP Lite is stable and faster.
4) Keep protocol selection at runtime (env-based) for rollback.
5) Make perf regressions and behavior changes measurable and reversible.

---

## 2. Strategy (Two-Stage)

### Stage A: ZMTP Lite
- Keep ZMTP framing but remove/disable:
  - Greeting downgrades, legacy version handling not needed in zlink-only.
  - Unused security mechanisms and metadata in fast path (keep NULL only).
  - Optional property parsing and extra validation branches.
  - Extra command handling not needed for zlink-only use cases.
- Preserve existing optimized encoder/decoder and batching behavior.

### Stage B: ZMP Format Conversion
- Replace ZMTP framing with ZMP framing.
- Reuse Stage A hot path structure to minimize regressions.
- Keep same socket pattern semantics and routing-id behavior.

---

## 3. Non-Goals

- External interoperability with standard ZMTP peers.
- CURVE or other legacy security mechanisms (TLS only as transport policy).
- Backward compatibility with older ZMTP versions.

---

## 4. Work Items

### Stage A: ZMTP Lite
1) Tighten handshake to ZMTP 3.1 only.
2) Remove downgrade path and legacy greeting handling.
3) Constrain mechanism to NULL in zlink-only mode.
4) Avoid redundant property parsing/metadata when not needed.
5) Ensure routing-id exchange remains correct for ROUTER patterns.
6) Add perf counters/log points only if needed for validation.

### Stage B: ZMP Format
1) Swap encoder/decoder to ZMP framing.
2) Replace greeting/READY with HELLO (ZMP).
3) Ensure routing-id frame semantics match Stage A.
4) Keep heartbeat behavior consistent.

---

## 5. Deliverables

- `docs/team/20260121_zmtp-lite_to_zmp/plan.md` (this plan)
- `docs/team/20260121_zmtp-lite_to_zmp/design.md`
- `docs/team/20260121_zmtp-lite_to_zmp/rfc.md`
- `docs/team/20260121_zmtp-lite_to_zmp/benchmarks.md`
- `docs/team/20260121_zmtp-lite_to_zmp/implementation_notes.md`

---

## 6. References (Code)

- ZMTP engine: `src/asio/asio_zmtp_engine.cpp`, `src/asio/asio_zmtp_engine.hpp`
- ASIO core engine: `src/asio/asio_engine.cpp`, `src/asio/asio_engine.hpp`
- Mechanism: `src/null_mechanism.cpp`, `src/mechanism.cpp`
- Encoder/Decoder: `src/v2_encoder.cpp`, `src/v2_decoder.cpp`
- Session/pipe: `src/session_base.cpp`, `src/pipe.cpp`
- Bench: `benchwithzmq/run_benchmarks.sh`, `benchwithzmq/run_comparison.py`

---

## 7. Benchmarks (Acceptance)

- Stage A: ZMTP Lite must meet or exceed current zmtp baseline for 64/256/1024B on tcp/inproc/ipc.
- Stage B: ZMP format must match Stage A (within +/-5%) on 64/256/1024B, and not regress >10% on large sizes.

---

## 8. Risks

- IPC regressions due to higher CPU cost vs transport cost.
- Routing-id edge cases on ROUTER patterns.
- Inaccurate benchmark results due to measurement anomalies.

---

## 9. Next Steps

1) Implement Stage A (ZMTP Lite) on main.
2) Run baseline and Stage A benchmarks with 10 runs.
3) Document perf deltas and confirm stability.
4) Proceed to Stage B only after Stage A results are positive.
