# ZMTP Lite -> ZMP Format Design

**Date:** 2026-01-21  
**Owner:** 팀장님  
**Author:** Codex

---

## 1. Overview

This design defines a two-stage approach:
- Stage A: ZMTP Lite (keep ZMTP wire format, remove unused paths)
- Stage B: ZMP format conversion (new wire format, reuse Stage A hot path)

The design explicitly separates the optimization phase from the format change.

---

## 2. Stage A (ZMTP Lite)

### 2.1 Handshake Simplification
- Allow only ZMTP 3.1 greeting path.
- Remove downgrade to v3.0, v2.0, v1.0 and unversioned modes.
- Require NULL mechanism only in zlink-only mode.

### 2.2 Mechanism and Metadata
- Use `null_mechanism_t` only (no CURVE/PLAIN/GSSAPI).
- Skip optional metadata collection unless needed by tests or monitoring.
- Avoid property parsing in critical path when not required.

### 2.3 Routing-Id Handling
- Preserve current ROUTER routing-id exchange flow.
- Ensure routing-id is injected only when `recv_routing_id` is enabled.

### 2.4 Encoder/Decoder
- Keep v2 encoder/decoder as-is for batching behavior.
- Avoid extra checks in the hot path beyond what is required for safety.

---

## 3. Stage B (ZMP Format)

### 3.1 Wire Format
- Replace ZMTP frames with ZMP fixed header + flags + length.
- Implement HELLO control frame (fixed format).
- Maintain heartbeat frames if needed, otherwise keep minimal control set.

### 3.2 Behavior Compatibility
- ZMQ socket pattern semantics unchanged.
- Routing-id rules preserved to avoid ROUTER regressions.
- Subscription/cancel handling mirrors existing SUB/XPUB behavior.

### 3.3 Engine Integration
- Use Stage A engine flow structure (handshake -> encoder/decoder -> steady state).
- Replace handshake and framing only.

---

## 4. Runtime Selection

- Environment variable `ZLINK_PROTOCOL=zmtp|zmp`.
- Stage A affects ZMTP path; Stage B adds ZMP path in parallel.

---

## 5. Open Questions

- Which ZMTP metadata can be removed without breaking monitoring/tests?
- Should ZAP remain available in ZMTP Lite mode?
- Do we keep heartbeat in ZMP or rely on transport timeouts?

---

## 6. References (Code)

- `src/asio/asio_zmtp_engine.cpp`
- `src/asio/asio_engine.cpp`
- `src/null_mechanism.cpp`
- `src/v2_encoder.cpp`
- `src/v2_decoder.cpp`
- `src/session_base.cpp`
- `src/pipe.cpp`
