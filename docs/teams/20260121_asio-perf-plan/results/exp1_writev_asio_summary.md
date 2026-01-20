# Experiment 1 summary: ZMQ_ASIO_WRITEV_USE_ASIO=1

- control: exp1_control_rr_sizes64_65536_runs3_20260121_011939.txt
- variant: exp1_writev_asio_rr_sizes64_65536_runs3_20260121_011955.txt

- inproc 64B Latency: control 0.16 us -> variant 0.16 us, delta +0.00%
- inproc 64B Throughput: control 5.34 Mmsg/s -> variant 5.43 Mmsg/s, delta +1.69%
- inproc 65536B Latency: control 1.98 us -> variant 2.00 us, delta -1.01%
- inproc 65536B Throughput: control 10649.43 MB/s -> variant 10192.68 MB/s, delta -4.29%
- ipc 64B Latency: control 3.31 us -> variant 3.38 us, delta -2.11%
- ipc 64B Throughput: control 3.44 Mmsg/s -> variant 3.58 Mmsg/s, delta +4.07%
- ipc 65536B Latency: control 11.10 us -> variant 11.32 us, delta -1.98%
- ipc 65536B Throughput: control 2187.66 MB/s -> variant 2198.13 MB/s, delta +0.48%
- tcp 64B Latency: control 3.82 us -> variant 3.82 us, delta +0.00%
- tcp 64B Throughput: control 3.55 Mmsg/s -> variant 3.46 Mmsg/s, delta -2.54%
- tcp 65536B Latency: control 11.36 us -> variant 11.37 us, delta -0.09%
- tcp 65536B Throughput: control 2292.91 MB/s -> variant 2223.12 MB/s, delta -3.04%

Summary: improved 2, worsened 6 (threshold ±0.5%).