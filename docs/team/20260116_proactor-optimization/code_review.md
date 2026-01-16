# True Proactor Rework Code Review

## Findings

1. **Potential data loss when draining `_pending_buffers`**
   - In `restart_input()`, if `_decoder->decode()` returns `rc == 0` (needs more data) while `buffer_remaining > 0`, the loop breaks and the pending buffer is popped, dropping unconsumed bytes. This can happen when a pending buffer is larger than the decoder buffer and decode needs more data to complete a message. Consider continuing the loop when `rc == 0` and `buffer_remaining > 0`, or preserving the remaining bytes in `_pending_buffers`.
   - Ref: `src/asio/asio_engine.cpp:985-1019`.

2. **Pending buffer limit does not include existing partial/decoder data**
   - The 10MB cap only sums `_pending_buffers` sizes. If `_insize` (partial data) or decoder internal buffer already hold data, total buffered bytes can exceed 10MB. This weakens the intended memory bound during sustained backpressure.
   - Ref: `src/asio/asio_engine.cpp:461-478`, `src/asio/asio_engine.cpp:914-935`, `src/asio/asio_engine.hpp:242-245`.

3. **Per-read O(n) size accounting may add overhead under heavy backpressure**
   - Each read sums the entire deque to compute `total_pending`. With many small buffers, this adds noticeable CPU overhead precisely in the backpressure path. Track a running `pending_bytes` counter instead.
   - Ref: `src/asio/asio_engine.cpp:461-465`.

## Review Point Responses

1. **Backpressure pending-read logic correctness**
   - The always-pending read model is correct in principle, but `restart_input()` has a correctness risk (Finding #1) that can drop buffered data. Fixing that is necessary to make the logic safe under large pending buffers.

2. **`_pending_buffers` memory safety (10MB limit)**
   - The limit applies only to deque contents and does not include `_insize` or decoder buffer state, so the true buffered total can exceed 10MB. This is a soft cap, not a hard cap (Finding #2). Consider tracking a total pending byte counter that includes `_insize` if you want a strict bound.

3. **Deadlock possibility**
   - No obvious deadlock introduced. All access to `_pending_buffers` appears to occur on the I/O thread. `terminate()` drains handlers and uses `_terminating` guards, which should avoid blocking callbacks.

4. **Performance impact (EAGAIN unchanged)**
   - The implementation result shows no EAGAIN reduction and a ~4% throughput drop in one benchmark. This aligns with added buffering/copying work during backpressure without measurable I/O savings in the tested scenarios.

5. **Code quality / improvement suggestions**
   - Consider a running `pending_bytes` counter and include `_insize` in the limit to make the cap explicit and O(1).
   - Add a targeted backpressure test that feeds large bursts to force `_pending_buffers` drain and verify no data loss/regression.

## Open Questions / Assumptions

- I assumed `_decoder->decode()` can return `rc == 0` with `buffer_remaining > 0` when pending buffers exceed decoder buffer size. If decoder guarantees `decode_size` is always large enough to consume the provided buffer, then Finding #1 is reduced, but that assumption is not evident here.

## Change Summary (Secondary)

- Implements True Proactor always-pending reads and buffers data during backpressure with a 10MB cap, and drains buffered data on `restart_input()`.
