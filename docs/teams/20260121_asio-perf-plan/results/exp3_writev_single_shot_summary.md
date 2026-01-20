# Experiment 3 summary: ZMQ_ASIO_WRITEV_SINGLE_SHOT=1

- control: exp1_control_rr_sizes64_65536_runs3_20260121_011939.txt
- variant: exp3_writev_single_shot_rr_sizes64_65536_runs3_20260121_012114.txt

- inproc 64B Latency: control 0.16 us -> variant 0.15 us, delta +6.25%
- inproc 64B Throughput: control 5.34 Mmsg/s -> variant 5.29 Mmsg/s, delta -0.94%
- inproc 65536B Latency: control 1.98 us -> variant 1.98 us, delta +0.00%
- inproc 65536B Throughput: control 10649.43 MB/s -> variant 10629.15 MB/s, delta -0.19%
- ipc 64B Latency: control 3.31 us -> variant 3.27 us, delta +1.21%
- ipc 64B Throughput: control 3.44 Mmsg/s -> variant 3.57 Mmsg/s, delta +3.78%
- ipc 65536B Latency: control 11.10 us -> variant 11.18 us, delta -0.72%
- ipc 65536B Throughput: control 2187.66 MB/s -> variant 2082.58 MB/s, delta -4.80%
- tcp 64B Latency: control 3.82 us -> variant 3.75 us, delta +1.83%
- tcp 64B Throughput: control 3.55 Mmsg/s -> variant 3.56 Mmsg/s, delta +0.28%
- tcp 65536B Latency: control 11.36 us -> variant 11.32 us, delta +0.35%
- tcp 65536B Throughput: control 2292.91 MB/s -> variant 2296.80 MB/s, delta +0.17%

Summary: improved 4, worsened 3 (threshold ±0.5%).