# RFC: ZMTP Lite to ZMP Format Transition

**Status:** Draft  
**Date:** 2026-01-21  
**Owner:** 팀장님

---

## 1. Motivation

Current ZMTP path is optimized but includes legacy compatibility and optional
features that are not required for zlink-only use cases. The proposal is to
remove non-essential work first (ZMTP Lite), then transition to a simplified
ZMP wire format while keeping performance and semantics stable.

---

## 2. Stage A: ZMTP Lite

### 2.1 Protocol Scope
- ZMTP 3.1 only.
- NULL mechanism only for zlink-only mode.
- Remove legacy downgrade and unversioned greeting paths.

### 2.2 Required Semantics
- Keep ROUTER routing-id exchange semantics.
- Keep PUB/SUB subscription propagation semantics.
- Preserve monitoring events (connected, handshake, disconnected).

---

## 3. Stage B: ZMP Wire Format

### 3.1 Framing
- 8-byte fixed header: magic, version, flags, reserved, length.
- Flags for MORE/CONTROL/IDENTITY/SUBSCRIBE/CANCEL.

### 3.2 Control Frames
- HELLO mandatory once per connection.
- HEARTBEAT optional (if enabled by existing options).

### 3.3 Routing-Id
- IDENTITY frame used only for ROUTER receive paths.
- IDENTITY cannot combine with MORE.

---

## 4. Backward Compatibility

- No external interoperability guaranteed.
- ZMTP Lite remains available for rollback and comparison.

---

## 5. Testing and Benchmarks

- Unit tests must pass in both modes.
- Benchmarks must be run with 10 iterations for baseline and Stage A.
- Stage B must show no large regressions against Stage A.

---

## 6. Decision Criteria

- Stage A improvements observed for 64/256/1024B across tcp/inproc/ipc.
- Stage B allowed only if Stage A stable and measurable improvements exist.
