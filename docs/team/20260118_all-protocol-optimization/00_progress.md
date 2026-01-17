# 20260118 all-protocol optimization progress

- Context: continuation of the all-protocol work in `docs/team/20260117_all-protocol-optimization/`.
- Goal: reduce the tcp 1024B gap without regressing 262144B, while keeping all patterns/protocols in scope.

## 2026-01-18

- Added env overrides for ASIO batch sizes to enable controlled experiments.
  - `ZMQ_ASIO_IN_BATCH_SIZE`
  - `ZMQ_ASIO_OUT_BATCH_SIZE`
- Ran tcp batch-size sweeps with `ZMQ_ASIO_TCP_MAX_TRANSFER=262144` at `BENCH_IO_THREADS=2` (unpinned).
- Verified that 256 KB batch sizes amplify 1024B gains but worsen 262144B.
- Added full-size tcp sweep for PUBSUB/ROUTER_ROUTER/ROUTER_ROUTER_POLL; 64 KB batching helps <=1024B but regresses 64KB+.
- Findings captured in `01_tcp_batch_size_override_test.md`.
- Full-size sweep results in `02_tcp_batch_size_full_size_sweep.md`.
- Added IPC/INPROC full-size sweeps; IPC benefits from 64 KB batching, INPROC impact is mixed.
- Results recorded in `03_ipc_inproc_batch_size_full_size_sweep.md`.
- Added transport-specific batch override envs to isolate IPC gains from TCP regressions.
- Verification recorded in `04_transport_specific_batch_override.md`.
- Implemented size-adaptive batching in ASIO ZMTP output: dual encoders
  (small/large) with selection by first message size to keep zero-copy for
  large frames while preserving small-message batching.
- Built and ran full test suite via `./build.sh` (ctest: 61 passed, 4 skipped
  fuzzers).

## 2026-01-19

- Benchmarks: `benchwithzmq/run_benchmarks.sh --with-libzmq --runs 3`
  - Output: `benchwithzmq/results_adaptive_batching.txt`
- TCP: large message regressions remain for PAIR/DEALER_* (e.g., 64KB+
  ranges still negative), while ROUTER_ROUTER and ROUTER_ROUTER_POLL show
  clear 1KB and 64KB gains with mixed 128KB+ results.
- IPC: mixed; PUBSUB gains at 64KB/128KB but DEALER_* regress, ROUTER_ROUTER
  loses at 64KB+.
- INPROC: mostly positive across patterns with a few regressions at 1KB/256KB+
  depending on pattern.
- Note: the "8KB threshold" comes from the default out_batch_size (8192) in
  options; it is a historical, conservative tradeoff to amortize syscall
  overhead without growing per-connection buffers too much. Zero-copy is not
  a hard 8KB cutoff and only applies when encoder state allows it.

## 2026-01-20

- TCP-only sweep of adaptive batching threshold (small out_batch_size) with
  fixed large batch size to study cutoff impact.
  - Env: `BENCH_TRANSPORTS=tcp`, `ZMQ_ASIO_OUT_BATCH_SIZE_TCP=65536`
  - Small sizes: 4K/8K/16K/32K via `ZMQ_ASIO_OUT_BATCH_SIZE`
  - Outputs:
    - `benchwithzmq/results_adaptive_batching_outbatch_4k.txt`
    - `benchwithzmq/results_adaptive_batching_outbatch_8k.txt`
    - `benchwithzmq/results_adaptive_batching_outbatch_16k.txt`
    - `benchwithzmq/results_adaptive_batching_outbatch_32k.txt`
- Summary:
  - 4K/8K are the most balanced for TCP: consistent 64B/256B/1KB gains across
    patterns, while 64KB+ regressions persist but are generally smaller than
    16K/32K.
  - 16K/32K do not improve 1KB meaningfully and tend to worsen 64KB+ (PAIR and
    DEALER_* still show notable negative deltas at 128KB/256KB).
  - ROUTER_ROUTER(_POLL) benefits at 1KB for all sizes; 4K keeps 64KB/128KB
    slightly positive in throughput, while larger sizes drift negative.
- Additional TCP sweep: 1K/2K small batch sizes.
  - Outputs:
    - `benchwithzmq/results_adaptive_batching_outbatch_1k.txt`
    - `benchwithzmq/results_adaptive_batching_outbatch_2k.txt`
  - Summary:
    - 1K/2K keep 64B/256B/1KB wins but introduce more volatility in 128KB/256KB,
      with some patterns regressing more than 4K/8K.
    - 4K remains the most stable overall across patterns for TCP.
- TCP+IPC sweep with 4K/8K small batch sizes (large fixed at 64K per transport).
  - Env: `BENCH_TRANSPORTS=tcp,ipc`,
    `ZMQ_ASIO_OUT_BATCH_SIZE_TCP=65536`, `ZMQ_ASIO_OUT_BATCH_SIZE_IPC=65536`
  - Outputs:
    - `benchwithzmq/results_adaptive_batching_outbatch_4k_tcp_ipc.txt`
    - `benchwithzmq/results_adaptive_batching_outbatch_8k_tcp_ipc.txt`
  - Summary:
    - TCP: 4K/8K both improve 64B/256B/1KB across most patterns, but 64KB+ still
      regress; 4K keeps 64KB/128KB slightly closer to baseline.
    - IPC: 4K yields broad 64B/256B/1KB gains and more neutral 64KB+, while 8K
      helps PUBSUB/ROUTER_ROUTER_POLL at 64KB/128KB but regresses in
      DEALER_ROUTER/DEALER_DEALER at 64KB+.
- ASIO async write updated to keep encoder buffer live (no copy) to align with
  libzmq stream_engine behavior and improve large-message costs.
  - Output: `benchwithzmq/results_adaptive_batching_zero_copy_async_4k_tcp_ipc.txt`
  - Summary:
    - TCP: large sizes improved in ROUTER_ROUTER(_POLL) and PAIR (64KB+ up to
      +18%), though some 256KB regressions remain in DEALER_ROUTER/PAIR.
    - IPC: improves 64KB/128KB in PUBSUB/ROUTER_ROUTER_POLL; DEALER_* still
      mixed but less negative at large sizes.
- Tried draining large frames synchronously before async fallback; results were
  worse for TCP large-message throughput (PAIR/PUBSUB regressions) so the change
  was dropped.
  - Output: `benchwithzmq/results_adaptive_batching_zero_copy_async_4k_tcp_ipc_v2.txt`
- Large-message header-only encode (separate header then zero-copy body) to
  mimic libzmq's multi-step encode behavior.
  - Output: `benchwithzmq/results_adaptive_batching_header_only_4k_tcp_ipc.txt`
  - Summary:
    - TCP: PUBSUB improves in 64KB+ (64KB/128KB/256KB all positive), but
      ROUTER_ROUTER and DEALER_* regress at 64KB+; overall mixed.
    - IPC: large sizes generally positive in ROUTER_ROUTER_POLL, but DEALER_*
      and PAIR show mixed or negative at 128KB+.

## 2026-01-21

- Dropped the header-only encode experiment after mixed results and reverted
  to the no-copy async write path with adaptive batching.
- TCP-only sweep with a lower "large" threshold (switch to small encoder
  at 32KB) to reduce large-message regressions:
  - Env: `BENCH_TRANSPORTS=tcp`, `ZMQ_ASIO_OUT_BATCH_SIZE_TCP=32768`
  - Small sizes: 4K/2K via `ZMQ_ASIO_OUT_BATCH_SIZE`
  - Outputs:
    - `benchwithzmq/results_adaptive_batching_outbatch_4k_tcp_32k.txt`
    - `benchwithzmq/results_adaptive_batching_outbatch_2k_tcp_32k.txt`
  - Summary (single run; higher variance):
    - 4K/32K keeps 64B/256B/1KB gains but still shows 128KB/256KB regressions
      across multiple patterns.
    - 2K/32K increases 1KB gains but introduces instability at 64KB/128KB
      (notably PUBSUB), and 256KB regressions persist in some patterns.
- Additional TCP-only sweep with 16KB large threshold and 4KB small batch.
  - Env: `BENCH_TRANSPORTS=tcp`, `ZMQ_ASIO_OUT_BATCH_SIZE=4096`,
    `ZMQ_ASIO_OUT_BATCH_SIZE_TCP=16384`
  - Output: `benchwithzmq/results_adaptive_batching_outbatch_4k_tcp_16k.txt`
  - Summary (single run; high variance): 1KB improves in several patterns,
    but large-size stability remains mixed; rerun with more iterations if
    this line looks promising.
- Switched back to a single 8KB batch size and added vectored async writes
  (header + body) for large messages to reduce syscall/encoding overhead.
  - Code: uses encoder header-only encode + async_writev for TCP/IPC.
  - Output: `benchwithzmq/results_async_writev_tcp_8k.txt`
  - Summary (tcp, 3 runs):
    - Small sizes: mostly neutral to small gains at 256B/1KB.
    - Large sizes: mixed; PUBSUB 64KB improved (~+19%), but PAIR/DEALER_ROUTER/
      ROUTER_ROUTER still show 64KB regressions (~-7% to -8%), and 128KB/256KB
      remain pattern-dependent.
- Added writev threshold override to limit vector writes to very large
  messages only.
  - Env: `ZMQ_ASIO_OUT_WRITEV_THRESHOLD[_TCP|_IPC|_INPROC]`
  - TCP+IPC run with 32KB threshold:
    - `benchwithzmq/results_async_writev_tcp_ipc_threshold_32k.txt`
  - Summary (runs=3):
    - TCP: small sizes hold modest gains; 64KB regressions shrink in some
      patterns but not eliminated; 128KB/256KB remain mixed.
    - IPC: 1KB is slightly down in some patterns; large sizes mixed but
      generally close to baseline.
  - TCP+IPC run with 64KB threshold:
    - `benchwithzmq/results_async_writev_tcp_ipc_threshold_64k.txt`
  - Summary (runs=3):
    - TCP: 64KB improves in a few patterns but 128KB/256KB regressions are
      still present; small sizes mixed.
    - IPC: small sizes slightly down overall; large sizes mixed.
  - TCP+IPC run with 128KB threshold:
    - `benchwithzmq/results_async_writev_tcp_ipc_threshold_128k.txt`
  - Summary (runs=3):
    - TCP: small sizes slightly positive; 64KB regressions mostly reduced,
      but 128KB+ remain pattern-dependent.
    - IPC: generally close to baseline; some patterns improved at 64KB+.
  - TCP-only run with 128KB threshold:
    - `benchwithzmq/results_async_writev_tcp_threshold_128k.txt`
  - Summary (runs=3):
    - TCP: 64KB mostly neutral to slightly positive; 128KB mixed;
      256KB still regresses in some patterns (PAIR/DEALER_*).
  - TCP-only run with writev effectively disabled (threshold 1MB):
    - `benchwithzmq/results_async_writev_tcp_threshold_disabled.txt`
  - Summary (runs=3):
    - TCP: large sizes mixed; 64KB mostly neutral, 128KB positive in several
      patterns, 256KB regressions remain in PAIR/PUBSUB/DEALER_*.
- Reintroduced dual batch sizes (small/large) to validate 4KB or 4KB/32KB
  splits while keeping writev threshold at 128KB.
  - Env: `ZMQ_ASIO_OUT_BATCH_SIZE_SMALL[_TCP|_IPC|_INPROC]`
  - TCP-only single 4KB batch:
    - `benchwithzmq/results_dualbatch_tcp_single_4k.txt`
  - Summary (runs=3):
    - Small sizes regress notably across patterns (-10%~-18% at 256B/1KB);
      large sizes mixed, 64KB sometimes positive.
  - TCP-only dual 4KB/32KB:
    - `benchwithzmq/results_dualbatch_tcp_4k_32k.txt`
  - Summary (runs=3):
    - Small sizes improve broadly (+4%~+9% at 256B/1KB).
    - 64KB results mixed (PAIR/PUBSUB/DEALER_DEALER regress; ROUTER_* near
      neutral to slight positive); 128KB+ generally positive or neutral.
  - TCP+IPC with dual 4KB/32KB, writev threshold 128KB:
    - `benchwithzmq/results_dualbatch_tcp_ipc_4k_32k_threshold_128k.txt`
  - Summary (runs=3):
    - TCP: 256B/1KB improve; 64KB mixed; 128KB regress in PAIR/DEALER_ROUTER.
    - IPC: mixed; 256B down in some patterns, 64KB+ mixed.
  - TCP+IPC with dual 4KB/32KB, writev threshold 256KB:
    - `benchwithzmq/results_dualbatch_tcp_ipc_4k_32k_threshold_256k.txt`
  - Summary (runs=3):
    - TCP: 256B/1KB improves, 64KB/128KB mixed; fewer large-size regressions
      than 128KB threshold in DEALER_ROUTER.
    - IPC: 256B/1KB mostly positive; 64KB+ generally positive with some
      256KB regressions.
  - TCP+IPC with dual 4KB/32KB, writev threshold 8KB:
    - `benchwithzmq/results_dualbatch_tcp_ipc_4k_32k_threshold_8k.txt`
  - Summary (runs=3):
    - TCP: small sizes positive; 64KB/128KB mixed; 256KB still regresses in
      some patterns.
    - IPC: small sizes positive; 64KB+ mixed but closer to baseline.
  - TCP+IPC with dual 4KB/32KB, writev threshold 16KB:
    - `benchwithzmq/results_dualbatch_tcp_ipc_4k_32k_threshold_16k.txt`
  - Summary (runs=3):
    - TCP: small sizes positive; 64KB/128KB mixed; 256KB regressions persist
      in some patterns.
    - IPC: small sizes positive; 64KB+ mixed with some large-size regressions.

## Final

- Conclusion: keep the current dual 4KB/32KB batching and default writev
  threshold at 8KB (matching baseline out_batch_size), with overrides only
  via env. This balances small-message wins and limits large-message regressions.
- Default knobs (no env):
  - out batch size: 32KB (large encoder)
  - small out batch size: 4KB (small encoder)
  - writev threshold: 8KB (header/body split + zero-copy)
- Benchmark execution note: Linux runs were CPU pinned via taskset (core 1)
  unless `BENCH_NO_TASKSET=1` was set.

## Next

- Explore size-adaptive batching to keep 1024B gains while limiting 262144B regression.
- Re-run tcp matrix (all patterns) with the best candidate, then extend to ipc/inproc if results hold.
