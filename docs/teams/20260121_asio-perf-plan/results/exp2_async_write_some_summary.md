# Experiment 2 summary: ZMQ_ASIO_TCP_ASYNC_WRITE_SOME=1

- control: exp1_control_rr_sizes64_65536_runs3_20260121_011939.txt
- variant: exp2_async_write_some_rr_sizes64_65536_runs3_20260121_012037.txt

- inproc 64B Latency: control 0.16 us -> variant 0.16 us, delta +0.00%
- inproc 64B Throughput: control 5.34 Mmsg/s -> variant 5.26 Mmsg/s, delta -1.50%
- inproc 65536B Latency: control 1.98 us -> variant 2.02 us, delta -2.02%
- inproc 65536B Throughput: control 10649.43 MB/s -> variant 10287.70 MB/s, delta -3.40%
- ipc 64B Latency: control 3.31 us -> variant 3.27 us, delta +1.21%
- ipc 64B Throughput: control 3.44 Mmsg/s -> variant 3.58 Mmsg/s, delta +4.07%
- ipc 65536B Latency: control 11.10 us -> variant 11.32 us, delta -1.98%
- ipc 65536B Throughput: control 2187.66 MB/s -> variant 2268.89 MB/s, delta +3.71%
- tcp 64B Latency: control 3.82 us -> variant 3.80 us, delta +0.52%
- tcp 64B Throughput: control 3.55 Mmsg/s -> variant 3.38 Mmsg/s, delta -4.79%
- tcp 65536B Latency: control 11.36 us -> variant 11.28 us, delta +0.70%
- tcp 65536B Throughput: control 2292.91 MB/s -> variant 2251.43 MB/s, delta -1.81%

Summary: improved 5, worsened 6 (threshold ±0.5%).