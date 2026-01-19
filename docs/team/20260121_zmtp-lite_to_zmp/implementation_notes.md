# Implementation Notes

**Date:** 2026-01-21  
**Owner:** 팀장님

---

## Stage A: ZMTP Lite

### Target Areas
- `src/asio/asio_zmtp_engine.cpp`
- `src/asio/asio_zmtp_engine.hpp`
- `src/asio/asio_engine.cpp`
- `src/null_mechanism.cpp`
- `src/v2_encoder.cpp`
- `src/v2_decoder.cpp`

### Changes (Planned)
- Remove greeting downgrade paths.
- Enforce ZMTP 3.1 only.
- Restrict mechanism to NULL for zlink-only mode.
- Avoid unnecessary metadata parsing.
- Keep routing-id flow unchanged.

---

## Stage B: ZMP Format

### Target Areas
- `src/asio/asio_zmp_engine.cpp`
- `src/zmp_encoder.cpp`
- `src/zmp_decoder.cpp`
- `src/zmp_protocol.hpp`

### Changes (Planned)
- Use fixed header and flags.
- Replace greeting with HELLO.
- Reuse Stage A engine flow.
- Keep routing-id semantics identical to Stage A.

---

## Testing

- `ctest --output-on-failure`
- Targeted protocol tests for ROUTER and PUB/SUB.
