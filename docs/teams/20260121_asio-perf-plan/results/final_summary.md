# Final summary (ASIO perf plan execution)

What was done
- Baseline runs captured (runs=10) and stored under results/.
- WSL2 profiling performed with strace (perf unavailable).
- Hypothesis backlog created.
- 3 micro-experiments executed (env toggles) with control/variant comparison.

Key observations
- strace shows futex/poll/epoll_wait dominating time, consistent with proactor scheduling/handler overhead.
- No tested config toggle yielded consistent improvements across transports/sizes.

Artifacts
- Baseline:
  - bench_baseline_zmp_runs10_20260121_004020.txt (partial)
  - bench_baseline_zmp_runs10_20260121_010045_DEALER_ROUTER.txt
  - bench_baseline_zmp_runs10_20260121_010710_ROUTER_ROUTER.txt
  - bench_baseline_zmp_runs10_20260121_011215_ROUTER_ROUTER_POLL.txt
- Profiling:
  - profiling_summary.md
  - strace_* and bench_* logs
- Experiments:
  - exp1_writev_asio_summary.md
  - exp2_async_write_some_summary.md
  - exp3_writev_single_shot_summary.md

Outcome
- No code changes applied; no global improvement identified via safe config toggles.
- Next meaningful improvements likely require structural ASIO changes (handler allocation, pump loop, buffer reuse)
  and should be validated on native Linux with perf.
