# Current Baseline (1-run, build/bin)

## Notes

- `build/bench/bin`의 zlink ipc 실행이 hang되어 `--build-dir build/bin` 사용.
- baseline은 1-run 기준이며 변동성 가능.
- PAIR tcp 65536B latency가 음수로 출력되는 이상치가 관찰됨.

## Benchmark Command

```
BENCH_TRANSPORTS=inproc,tcp,ipc BENCH_MSG_SIZES=64,256,1024,65536,131072,262144 \
  ./benchwithzmq/run_comparison.py <PATTERN> --runs 1 --refresh-libzmq --build-dir build/bin
```

## PAIR

### inproc

| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   6.73 M/s |   7.55 M/s |  +12.11% |
| | Latency |     0.07 us |     0.07 us |   +0.00% (inv) |
| 256B | Throughput |   5.92 M/s |   5.02 M/s |  -15.28% |
| | Latency |     0.08 us |     0.09 us |  -12.50% (inv) |
| 1024B | Throughput |   2.90 M/s |   2.86 M/s |   -1.13% |
| | Latency |     0.09 us |     0.12 us |  -33.33% (inv) |
| 65536B | Throughput |   0.16 M/s |   0.18 M/s |  +11.74% |
| | Latency |     1.95 us |     1.78 us |   +8.72% (inv) |
| 131072B | Throughput |   0.11 M/s |   0.12 M/s |   +9.48% |
| | Latency |     3.46 us |     3.47 us |   -0.29% (inv) |
| 262144B | Throughput |   0.07 M/s |   0.07 M/s |   -1.13% |
| | Latency |     6.69 us |     7.08 us |   -5.83% (inv) |

### tcp

| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.59 M/s |   4.64 M/s |   +1.05% |
| | Latency |     4.75 us |     4.98 us |   -4.84% (inv) |
| 256B | Throughput |   2.68 M/s |   2.67 M/s |   -0.35% |
| | Latency |     4.92 us |     5.13 us |   -4.27% (inv) |
| 1024B | Throughput |   0.94 M/s |   0.98 M/s |   +4.59% |
| | Latency |     4.92 us |     5.15 us |   -4.67% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.04 M/s |  +16.79% |
| | Latency |  -142.38 us |    13.68 us |   +0.00% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |  -24.19% |
| | Latency |    17.27 us |    21.42 us |  -24.03% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |   -0.42% |
| | Latency |    27.94 us |    38.56 us |  -38.01% (inv) |

Note: tcp 65536B latency는 1-run 측정에서 음수로 기록되어 이상치로 판단됨.

### ipc

| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.98 M/s |   4.78 M/s |   -3.99% |
| | Latency |     4.29 us |     4.43 us |   -3.26% (inv) |
| 256B | Throughput |   2.96 M/s |   2.76 M/s |   -6.81% |
| | Latency |     4.37 us |     4.60 us |   -5.26% (inv) |
| 1024B | Throughput |   1.10 M/s |   1.04 M/s |   -4.71% |
| | Latency |     4.16 us |     4.51 us |   -8.41% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.03 M/s |   -3.57% |
| | Latency |    11.56 us |    12.86 us |  -11.25% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |   -4.62% |
| | Latency |    17.51 us |    20.69 us |  -18.16% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  -17.25% |
| | Latency |    28.51 us |    37.10 us |  -30.13% (inv) |

## PUBSUB

### inproc

| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   6.11 M/s |   6.11 M/s |   -0.12% |
| | Latency |     0.16 us |     0.16 us |   +0.00% (inv) |
| 256B | Throughput |   4.91 M/s |   5.33 M/s |   +8.60% |
| | Latency |     0.20 us |     0.19 us |   +5.00% (inv) |
| 1024B | Throughput |   2.88 M/s |   3.11 M/s |   +7.94% |
| | Latency |     0.35 us |     0.32 us |   +8.57% (inv) |
| 65536B | Throughput |   0.17 M/s |   0.17 M/s |   +0.73% |
| | Latency |     6.02 us |     5.97 us |   +0.83% (inv) |
| 131072B | Throughput |   0.11 M/s |   0.11 M/s |   +3.56% |
| | Latency |     9.35 us |     9.02 us |   +3.53% (inv) |
| 262144B | Throughput |   0.07 M/s |   0.07 M/s |   -0.28% |
| | Latency |    15.27 us |    15.31 us |   -0.26% (inv) |

### tcp

| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.42 M/s |   3.95 M/s |  -10.69% |
| | Latency |     0.23 us |     0.25 us |   -8.70% (inv) |
| 256B | Throughput |   2.51 M/s |   2.49 M/s |   -1.02% |
| | Latency |     0.40 us |     0.40 us |   +0.00% (inv) |
| 1024B | Throughput |   0.97 M/s |   0.93 M/s |   -4.81% |
| | Latency |     1.03 us |     1.08 us |   -4.85% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.03 M/s |   +3.59% |
| | Latency |    31.41 us |    30.33 us |   +3.44% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |  +10.02% |
| | Latency |    58.03 us |    52.75 us |   +9.10% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  -29.28% |
| | Latency |    80.75 us |   114.18 us |  -41.40% (inv) |

### ipc

| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.41 M/s |   4.18 M/s |   -5.17% |
| | Latency |     0.23 us |     0.24 us |   -4.35% (inv) |
| 256B | Throughput |   2.78 M/s |   2.66 M/s |   -4.12% |
| | Latency |     0.36 us |     0.38 us |   -5.56% (inv) |
| 1024B | Throughput |   1.05 M/s |   1.04 M/s |   -1.43% |
| | Latency |     0.95 us |     0.96 us |   -1.05% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.03 M/s |  +12.52% |
| | Latency |    32.29 us |    28.69 us |  +11.15% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |  -24.62% |
| | Latency |    48.14 us |    63.86 us |  -32.65% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  -23.31% |
| | Latency |    87.09 us |   113.55 us |  -30.38% (inv) |

## DEALER_DEALER

### inproc

| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   6.59 M/s |   7.12 M/s |   +8.02% |
| | Latency |     0.09 us |     0.07 us |  +22.22% (inv) |
| 256B | Throughput |   5.87 M/s |   5.03 M/s |  -14.27% |
| | Latency |     0.08 us |     0.08 us |   +0.00% (inv) |
| 1024B | Throughput |   2.92 M/s |   2.84 M/s |   -2.80% |
| | Latency |     0.10 us |     0.09 us |  +10.00% (inv) |
| 65536B | Throughput |   0.16 M/s |   0.18 M/s |  +10.20% |
| | Latency |     2.02 us |     1.70 us |  +15.84% (inv) |
| 131072B | Throughput |   0.10 M/s |   0.11 M/s |   +7.75% |
| | Latency |     3.40 us |     3.48 us |   -2.35% (inv) |
| 262144B | Throughput |   0.07 M/s |   0.06 M/s |   -3.12% |
| | Latency |     6.66 us |     6.71 us |   -0.75% (inv) |

### tcp

| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.73 M/s |   4.48 M/s |   -5.34% |
| | Latency |     4.79 us |     5.00 us |   -4.38% (inv) |
| 256B | Throughput |   2.58 M/s |   2.53 M/s |   -1.92% |
| | Latency |     4.78 us |     5.19 us |   -8.58% (inv) |
| 1024B | Throughput |   0.92 M/s |   0.96 M/s |   +5.13% |
| | Latency |     5.10 us |     5.16 us |   -1.18% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.03 M/s |  +22.24% |
| | Latency |    11.93 us |    12.93 us |   -8.38% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |  -11.85% |
| | Latency |    17.49 us |    21.22 us |  -21.33% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  -14.09% |
| | Latency |    28.73 us |    38.33 us |  -33.41% (inv) |

### ipc

| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   4.52 M/s |   4.59 M/s |   +1.65% |
| | Latency |     4.40 us |     4.51 us |   -2.50% (inv) |
| 256B | Throughput |   2.77 M/s |   2.75 M/s |   -0.68% |
| | Latency |     4.46 us |     4.46 us |   +0.00% (inv) |
| 1024B | Throughput |   1.07 M/s |   1.05 M/s |   -1.41% |
| | Latency |     4.31 us |     4.52 us |   -4.87% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.03 M/s |  -25.11% |
| | Latency |    11.64 us |    12.72 us |   -9.28% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |  +14.15% |
| | Latency |    17.45 us |    19.95 us |  -14.33% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  -13.06% |
| | Latency |    28.81 us |    35.48 us |  -23.15% (inv) |

## DEALER_ROUTER

### inproc

| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   6.79 M/s |   6.33 M/s |   -6.67% |
| | Latency |     0.10 us |     0.10 us |   +0.00% (inv) |
| 256B | Throughput |   5.40 M/s |   5.26 M/s |   -2.58% |
| | Latency |     0.10 us |     0.11 us |  -10.00% (inv) |
| 1024B | Throughput |   2.88 M/s |   3.02 M/s |   +5.07% |
| | Latency |     0.15 us |     0.13 us |  +13.33% (inv) |
| 65536B | Throughput |   0.16 M/s |   0.16 M/s |   +2.47% |
| | Latency |     1.94 us |     1.85 us |   +4.64% (inv) |
| 131072B | Throughput |   0.11 M/s |   0.11 M/s |   +7.42% |
| | Latency |     3.53 us |     3.38 us |   +4.25% (inv) |
| 262144B | Throughput |   0.07 M/s |   0.07 M/s |   -0.31% |
| | Latency |     6.74 us |     6.58 us |   +2.37% (inv) |

### tcp

| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   3.77 M/s |   3.82 M/s |   +1.23% |
| | Latency |     4.77 us |     4.93 us |   -3.35% (inv) |
| 256B | Throughput |   2.49 M/s |   2.45 M/s |   -1.58% |
| | Latency |     4.94 us |     4.97 us |   -0.61% (inv) |
| 1024B | Throughput |   0.93 M/s |   0.93 M/s |   -0.19% |
| | Latency |     4.89 us |     5.05 us |   -3.27% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.03 M/s |   +0.34% |
| | Latency |    11.90 us |    12.86 us |   -8.07% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |  +10.23% |
| | Latency |    18.37 us |    21.31 us |  -16.00% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |   +0.14% |
| | Latency |    29.27 us |    36.59 us |  -25.01% (inv) |

### ipc

| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   3.80 M/s |   3.72 M/s |   -1.98% |
| | Latency |     4.27 us |     4.61 us |   -7.96% (inv) |
| 256B | Throughput |   2.40 M/s |   2.37 M/s |   -1.38% |
| | Latency |     4.40 us |     4.42 us |   -0.45% (inv) |
| 1024B | Throughput |   1.02 M/s |   0.98 M/s |   -3.87% |
| | Latency |     4.36 us |     4.68 us |   -7.34% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.04 M/s |   +5.48% |
| | Latency |    11.66 us |    12.69 us |   -8.83% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |   -3.35% |
| | Latency |    17.74 us |    20.59 us |  -16.07% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |   +2.11% |
| | Latency |    28.90 us |    35.93 us |  -24.33% (inv) |

## ROUTER_ROUTER

### inproc

| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.15 M/s |   5.04 M/s |   -2.22% |
| | Latency |     0.16 us |     0.16 us |   +0.00% (inv) |
| 256B | Throughput |   4.51 M/s |   4.72 M/s |   +4.67% |
| | Latency |     0.18 us |     0.17 us |   +5.56% (inv) |
| 1024B | Throughput |   2.58 M/s |   2.87 M/s |  +11.26% |
| | Latency |     0.20 us |     0.18 us |  +10.00% (inv) |
| 65536B | Throughput |   0.15 M/s |   0.17 M/s |  +13.47% |
| | Latency |     1.96 us |     1.90 us |   +3.06% (inv) |
| 131072B | Throughput |   0.11 M/s |   0.11 M/s |   +4.11% |
| | Latency |     3.91 us |     3.56 us |   +8.95% (inv) |
| 262144B | Throughput |   0.07 M/s |   0.06 M/s |  -12.64% |
| | Latency |     7.14 us |     8.10 us |  -13.45% (inv) |

### tcp

| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   3.62 M/s |   3.53 M/s |   -2.42% |
| | Latency |     3.97 us |     3.76 us |   +5.29% (inv) |
| 256B | Throughput |   2.22 M/s |   2.33 M/s |   +4.65% |
| | Latency |     4.02 us |     3.82 us |   +4.98% (inv) |
| 1024B | Throughput |   0.87 M/s |   0.91 M/s |   +4.33% |
| | Latency |     4.15 us |     3.91 us |   +5.78% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.03 M/s |  +29.60% |
| | Latency |    11.12 us |    12.03 us |   -8.18% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |  -13.03% |
| | Latency |    17.21 us |    20.33 us |  -18.13% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |  -15.53% |
| | Latency |    36.21 us |    44.81 us |  -23.75% (inv) |

### ipc

| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   3.69 M/s |   3.33 M/s |   -9.66% |
| | Latency |     3.54 us |     3.27 us |   +7.63% (inv) |
| 256B | Throughput |   2.47 M/s |   2.28 M/s |   -7.71% |
| | Latency |     3.76 us |     3.32 us |  +11.70% (inv) |
| 1024B | Throughput |   0.99 M/s |   0.88 M/s |  -10.92% |
| | Latency |     3.74 us |     3.51 us |   +6.15% (inv) |
| 65536B | Throughput |   0.04 M/s |   0.03 M/s |   -7.20% |
| | Latency |    12.66 us |    12.71 us |   -0.39% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |   +3.32% |
| | Latency |    20.75 us |    19.93 us |   +3.95% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |   -8.39% |
| | Latency |    39.25 us |    36.73 us |   +6.42% (inv) |

## ROUTER_ROUTER_POLL

### inproc

| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   5.35 M/s |   5.47 M/s |   +2.29% |
| | Latency |     0.53 us |     0.46 us |  +13.21% (inv) |
| 256B | Throughput |   4.38 M/s |   4.58 M/s |   +4.48% |
| | Latency |     0.52 us |     0.50 us |   +3.85% (inv) |
| 1024B | Throughput |   2.65 M/s |   2.73 M/s |   +2.74% |
| | Latency |     0.55 us |     0.52 us |   +5.45% (inv) |
| 65536B | Throughput |   0.13 M/s |   0.15 M/s |  +19.29% |
| | Latency |     2.37 us |     2.21 us |   +6.75% (inv) |
| 131072B | Throughput |   0.09 M/s |   0.10 M/s |   +8.89% |
| | Latency |     4.15 us |     4.05 us |   +2.41% (inv) |
| 262144B | Throughput |   0.06 M/s |   0.06 M/s |   -3.86% |
| | Latency |     7.43 us |     8.02 us |   -7.94% (inv) |

### tcp

| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   3.58 M/s |   3.59 M/s |   +0.44% |
| | Latency |     4.47 us |     4.39 us |   +1.79% (inv) |
| 256B | Throughput |   2.15 M/s |   2.22 M/s |   +2.84% |
| | Latency |     4.89 us |     4.31 us |  +11.86% (inv) |
| 1024B | Throughput |   0.88 M/s |   0.88 M/s |   -0.20% |
| | Latency |     4.49 us |     4.38 us |   +2.45% (inv) |
| 65536B | Throughput |   0.04 M/s |   0.03 M/s |  -21.63% |
| | Latency |    11.86 us |    12.86 us |   -8.43% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |  +14.38% |
| | Latency |    17.17 us |    21.01 us |  -22.36% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |   +6.72% |
| | Latency |    36.32 us |    45.42 us |  -25.06% (inv) |

### ipc

| Size | Metric | Standard libzmq | zlink | Diff (%) |
|------|--------|-----------------|-------|----------|
| 64B | Throughput |   3.52 M/s |   3.51 M/s |   -0.12% |
| | Latency |     3.97 us |     3.81 us |   +4.03% (inv) |
| 256B | Throughput |   2.52 M/s |   2.31 M/s |   -8.56% |
| | Latency |     3.98 us |     3.96 us |   +0.50% (inv) |
| 1024B | Throughput |   0.97 M/s |   0.83 M/s |  -14.35% |
| | Latency |     4.08 us |     4.06 us |   +0.49% (inv) |
| 65536B | Throughput |   0.03 M/s |   0.03 M/s |  -11.61% |
| | Latency |    12.03 us |    12.31 us |   -2.33% (inv) |
| 131072B | Throughput |   0.02 M/s |   0.02 M/s |   +9.19% |
| | Latency |    22.06 us |    21.09 us |   +4.40% (inv) |
| 262144B | Throughput |   0.01 M/s |   0.01 M/s |   +4.27% |
| | Latency |    35.53 us |    39.53 us |  -11.26% (inv) |
