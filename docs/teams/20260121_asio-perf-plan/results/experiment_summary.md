# Experiment summary (proactor/ASIO)

Experiment 1: ZMQ_ASIO_WRITEV_USE_ASIO=1
- Result: mixed, overall worse (2 improved / 6 worsened).
- Decision: not adopt.
- Summary file: exp1_writev_asio_summary.md

Experiment 2: ZMQ_ASIO_TCP_ASYNC_WRITE_SOME=1
- Result: mixed, overall worse (5 improved / 6 worsened) and potential semantics risk.
- Decision: not adopt.
- Summary file: exp2_async_write_some_summary.md

Experiment 3: ZMQ_ASIO_WRITEV_SINGLE_SHOT=1
- Result: mixed, small deltas (4 improved / 3 worsened), no clear win.
- Decision: not adopt.
- Summary file: exp3_writev_single_shot_summary.md

Conclusion
- No configuration change showed a consistent improvement across transports/sizes in the micro test.
- Stop condition reached (3 consecutive attempts without meaningful improvement).
