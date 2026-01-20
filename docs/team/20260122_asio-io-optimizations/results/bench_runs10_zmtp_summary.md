# ZMTP benchmark summary (runs=10, reuse build, skip libzmq)

Command:
- ZLINK_PROTOCOL=zmtp ./run_benchmarks.sh --runs 10 --reuse-build --skip-libzmq

Build dir:
- /home/ulalax/project/ulalax/zlink/build/bench

Baseline:
- libzmq results were loaded from cache (benchwithzmq/libzmq_cache.json)

Notes:
- Diff (%) is zlink vs libzmq. Throughput: positive = faster. Latency: positive = lower latency.
- This summary was reconstructed from console output (no raw log file saved).

## Results

### PATTERN: PAIR

#### Transport: tcp
| Size   | Metric     |  Standard libzmq |            zlink |  Diff (%) |
|--------|------------|------------------|------------------|-----------|
| 64B    | Throughput |      4.75 Mmsg/s |      4.83 Mmsg/s |   +1.72% |
| 64B    | Latency    |          4.79 us |          5.08 us |   -6.06% |
| 256B   | Throughput |      2.64 Mmsg/s |      2.69 Mmsg/s |   +1.94% |
| 256B   | Latency    |          4.87 us |          5.06 us |   -4.11% |
| 1024B  | Throughput |      0.93 Mmsg/s |      0.98 Mmsg/s |   +5.38% |
| 1024B  | Latency    |          4.87 us |          5.19 us |   -6.47% |
| 65536B | Throughput |     2204.39 MB/s |     2238.82 MB/s |   +1.56% |
| 65536B | Latency    |         12.04 us |         12.39 us |   -2.86% |
| 131072B | Throughput |     2250.53 MB/s |     2502.04 MB/s |  +11.18% |
| 131072B | Latency    |         18.04 us |         20.05 us |  -11.20% |
| 262144B | Throughput |     2808.36 MB/s |     2572.95 MB/s |   -8.38% |
| 262144B | Latency    |         29.57 us |         34.08 us |  -15.24% |

#### Transport: inproc
| Size   | Metric     |  Standard libzmq |            zlink |  Diff (%) |
|--------|------------|------------------|------------------|-----------|
| 64B    | Throughput |      6.88 Mmsg/s |      6.44 Mmsg/s |   -6.38% |
| 64B    | Latency    |          0.07 us |          0.08 us |  -21.43% |
| 256B   | Throughput |      5.46 Mmsg/s |      5.82 Mmsg/s |   +6.61% |
| 256B   | Latency    |          0.08 us |          0.08 us |   +6.25% |
| 1024B  | Throughput |      2.80 Mmsg/s |      2.91 Mmsg/s |   +3.58% |
| 1024B  | Latency    |          0.09 us |          0.12 us |  -33.33% |
| 65536B | Throughput |     9855.58 MB/s |    10687.23 MB/s |   +8.44% |
| 65536B | Latency    |          1.89 us |          1.94 us |   -2.38% |
| 131072B | Throughput |    13905.58 MB/s |    13808.91 MB/s |   -0.70% |
| 131072B | Latency    |          3.37 us |          3.50 us |   -4.16% |
| 262144B | Throughput |    17318.12 MB/s |    16900.37 MB/s |   -2.41% |
| 262144B | Latency    |          6.62 us |          6.76 us |   -2.19% |

#### Transport: ipc
| Size   | Metric     |  Standard libzmq |            zlink |  Diff (%) |
|--------|------------|------------------|------------------|-----------|
| 64B    | Throughput |      4.85 Mmsg/s |      4.82 Mmsg/s |   -0.53% |
| 64B    | Latency    |          4.30 us |          4.58 us |   -6.27% |
| 256B   | Throughput |      2.79 Mmsg/s |      2.73 Mmsg/s |   -1.86% |
| 256B   | Latency    |          4.33 us |          4.60 us |   -6.24% |
| 1024B  | Throughput |      1.05 Mmsg/s |      0.95 Mmsg/s |   -9.47% |
| 1024B  | Latency    |          4.29 us |          5.05 us |  -17.69% |
| 65536B | Throughput |     2147.93 MB/s |     1921.82 MB/s |  -10.53% |
| 65536B | Latency    |         11.85 us |         12.45 us |   -5.02% |
| 131072B | Throughput |     2372.32 MB/s |     2350.26 MB/s |   -0.93% |
| 131072B | Latency    |         18.00 us |         19.20 us |   -6.67% |
| 262144B | Throughput |     2585.76 MB/s |     2756.43 MB/s |   +6.60% |
| 262144B | Latency    |         28.90 us |         33.01 us |  -14.22% |

### PATTERN: PUBSUB

#### Transport: tcp
| Size   | Metric     |  Standard libzmq |            zlink |  Diff (%) |
|--------|------------|------------------|------------------|-----------|
| 64B    | Throughput |      4.24 Mmsg/s |      4.31 Mmsg/s |   +1.79% |
| 64B    | Latency    |          0.24 us |          0.23 us |   +4.17% |
| 256B   | Throughput |      2.51 Mmsg/s |      2.60 Mmsg/s |   +3.57% |
| 256B   | Latency    |          0.40 us |          0.39 us |   +3.75% |
| 1024B  | Throughput |      0.92 Mmsg/s |      0.98 Mmsg/s |   +6.79% |
| 1024B  | Latency    |          1.09 us |          1.02 us |   +6.42% |
| 65536B | Throughput |     2178.81 MB/s |     2299.03 MB/s |   +5.52% |
| 65536B | Latency    |         30.08 us |         28.50 us |   +5.24% |
| 131072B | Throughput |     2396.32 MB/s |     2121.49 MB/s |  -11.47% |
| 131072B | Latency    |         55.23 us |         61.79 us |  -11.88% |
| 262144B | Throughput |     2610.07 MB/s |     2410.57 MB/s |   -7.64% |
| 262144B | Latency    |        100.44 us |        108.75 us |   -8.27% |

#### Transport: inproc
| Size   | Metric     |  Standard libzmq |            zlink |  Diff (%) |
|--------|------------|------------------|------------------|-----------|
| 64B    | Throughput |      6.14 Mmsg/s |      6.26 Mmsg/s |   +2.00% |
| 64B    | Latency    |          0.16 us |          0.16 us |   +0.00% |
| 256B   | Throughput |      5.06 Mmsg/s |      4.87 Mmsg/s |   -3.91% |
| 256B   | Latency    |          0.20 us |          0.21 us |   -2.50% |
| 1024B  | Throughput |      2.78 Mmsg/s |      3.00 Mmsg/s |   +7.69% |
| 1024B  | Latency    |          0.36 us |          0.34 us |   +6.94% |
| 65536B | Throughput |    10199.96 MB/s |    10780.82 MB/s |   +5.69% |
| 65536B | Latency    |          6.42 us |          6.08 us |   +5.37% |
| 131072B | Throughput |    13988.76 MB/s |    14589.91 MB/s |   +4.30% |
| 131072B | Latency    |          9.37 us |          8.98 us |   +4.11% |
| 262144B | Throughput |    16984.54 MB/s |    17295.23 MB/s |   +1.83% |
| 262144B | Latency    |         15.43 us |         15.16 us |   +1.81% |

#### Transport: ipc
| Size   | Metric     |  Standard libzmq |            zlink |  Diff (%) |
|--------|------------|------------------|------------------|-----------|
| 64B    | Throughput |      4.42 Mmsg/s |      4.47 Mmsg/s |   +1.06% |
| 64B    | Latency    |          0.23 us |          0.22 us |   +4.35% |
| 256B   | Throughput |      2.66 Mmsg/s |      2.73 Mmsg/s |   +2.51% |
| 256B   | Latency    |          0.38 us |          0.36 us |   +2.67% |
| 1024B  | Throughput |      0.99 Mmsg/s |      0.99 Mmsg/s |   +0.22% |
| 1024B  | Latency    |          1.01 us |          1.00 us |   +0.50% |
| 65536B | Throughput |     2108.81 MB/s |     2112.80 MB/s |   +0.19% |
| 65536B | Latency    |         31.07 us |         31.02 us |   +0.18% |
| 131072B | Throughput |     2181.72 MB/s |     2384.03 MB/s |   +9.27% |
| 131072B | Latency    |         60.08 us |         54.98 us |   +8.49% |
| 262144B | Throughput |     2826.05 MB/s |     2644.87 MB/s |   -6.41% |
| 262144B | Latency    |         92.76 us |         99.12 us |   -6.86% |

### PATTERN: DEALER_DEALER

#### Transport: tcp
| Size   | Metric     |  Standard libzmq |            zlink |  Diff (%) |
|--------|------------|------------------|------------------|-----------|
| 64B    | Throughput |      4.59 Mmsg/s |      4.55 Mmsg/s |   -0.79% |
| 64B    | Latency    |          4.81 us |          5.05 us |   -5.09% |
| 256B   | Throughput |      2.58 Mmsg/s |      2.66 Mmsg/s |   +3.05% |
| 256B   | Latency    |          4.90 us |          5.08 us |   -3.67% |
| 1024B  | Throughput |      0.91 Mmsg/s |      0.97 Mmsg/s |   +6.28% |
| 1024B  | Latency    |          4.90 us |          5.22 us |   -6.53% |
| 65536B | Throughput |     2244.46 MB/s |     2251.30 MB/s |   +0.30% |
| 65536B | Latency    |         12.16 us |         12.39 us |   -1.89% |
| 131072B | Throughput |     2610.90 MB/s |     2611.06 MB/s |   +0.01% |
| 131072B | Latency    |         18.13 us |         19.62 us |   -8.19% |
| 262144B | Throughput |     2573.37 MB/s |     2508.94 MB/s |   -2.50% |
| 262144B | Latency    |         29.41 us |         34.26 us |  -16.49% |

#### Transport: inproc
| Size   | Metric     |  Standard libzmq |            zlink |  Diff (%) |
|--------|------------|------------------|------------------|-----------|
| 64B    | Throughput |      6.92 Mmsg/s |      6.63 Mmsg/s |   -4.24% |
| 64B    | Latency    |          0.08 us |          0.08 us |   +0.00% |
| 256B   | Throughput |      5.75 Mmsg/s |      5.78 Mmsg/s |   +0.54% |
| 256B   | Latency    |          0.08 us |          0.08 us |   +0.00% |
| 1024B  | Throughput |      2.84 Mmsg/s |      2.88 Mmsg/s |   +1.50% |
| 1024B  | Latency    |          0.10 us |          0.10 us |   +0.00% |
| 65536B | Throughput |    10093.86 MB/s |    10991.03 MB/s |   +8.89% |
| 65536B | Latency    |          1.91 us |          1.92 us |   -0.26% |
| 131072B | Throughput |    13692.50 MB/s |    14772.87 MB/s |   +7.89% |
| 131072B | Latency    |          3.44 us |          3.50 us |   -1.89% |
| 262144B | Throughput |    16422.62 MB/s |    17557.67 MB/s |   +6.91% |
| 262144B | Latency    |          6.69 us |          6.83 us |   -2.09% |

#### Transport: ipc
| Size   | Metric     |  Standard libzmq |            zlink |  Diff (%) |
|--------|------------|------------------|------------------|-----------|
| 64B    | Throughput |      4.75 Mmsg/s |      4.79 Mmsg/s |   +0.87% |
| 64B    | Latency    |          4.29 us |          4.51 us |   -5.01% |
| 256B   | Throughput |      2.76 Mmsg/s |      2.81 Mmsg/s |   +1.77% |
| 256B   | Latency    |          4.34 us |          4.54 us |   -4.37% |
| 1024B  | Throughput |      1.05 Mmsg/s |      1.04 Mmsg/s |   -1.00% |
| 1024B  | Latency    |          4.36 us |          4.64 us |   -6.54% |
| 65536B | Throughput |     2175.82 MB/s |     2056.99 MB/s |   -5.46% |
| 65536B | Latency    |         11.78 us |         12.59 us |   -6.88% |
| 131072B | Throughput |     2465.62 MB/s |     2336.85 MB/s |   -5.22% |
| 131072B | Latency    |         17.87 us |         19.56 us |   -9.46% |
| 262144B | Throughput |     2654.59 MB/s |     2585.97 MB/s |   -2.59% |
| 262144B | Latency    |         30.02 us |         33.53 us |  -11.71% |

### PATTERN: DEALER_ROUTER

#### Transport: tcp
| Size   | Metric     |  Standard libzmq |            zlink |  Diff (%) |
|--------|------------|------------------|------------------|-----------|
| 64B    | Throughput |      3.72 Mmsg/s |      3.70 Mmsg/s |   -0.63% |
| 64B    | Latency    |          4.80 us |          5.18 us |   -7.92% |
| 256B   | Throughput |      2.30 Mmsg/s |      2.30 Mmsg/s |   +0.12% |
| 256B   | Latency    |          4.90 us |          5.16 us |   -5.31% |
| 1024B  | Throughput |      0.89 Mmsg/s |      0.93 Mmsg/s |   +4.03% |
| 1024B  | Latency    |          4.92 us |          5.21 us |   -6.00% |
| 65536B | Throughput |     2187.99 MB/s |     2265.46 MB/s |   +3.54% |
| 65536B | Latency    |         12.03 us |         12.57 us |   -4.53% |
| 131072B | Throughput |     2153.14 MB/s |     2214.99 MB/s |   +2.87% |
| 131072B | Latency    |         18.48 us |         20.23 us |   -9.44% |
| 262144B | Throughput |     2609.31 MB/s |     2816.60 MB/s |   +7.94% |
| 262144B | Latency    |         29.96 us |         34.20 us |  -14.13% |

#### Transport: inproc
| Size   | Metric     |  Standard libzmq |            zlink |  Diff (%) |
|--------|------------|------------------|------------------|-----------|
| 64B    | Throughput |      6.50 Mmsg/s |      6.68 Mmsg/s |   +2.69% |
| 64B    | Latency    |          0.10 us |          0.11 us |   -5.00% |
| 256B   | Throughput |      5.39 Mmsg/s |      5.24 Mmsg/s |   -2.81% |
| 256B   | Latency    |          0.10 us |          0.10 us |   +0.00% |
| 1024B  | Throughput |      2.90 Mmsg/s |      2.87 Mmsg/s |   -1.01% |
| 1024B  | Latency    |          0.12 us |          0.12 us |   +0.00% |
| 65536B | Throughput |     9898.46 MB/s |    10382.35 MB/s |   +4.89% |
| 65536B | Latency    |          1.88 us |          1.90 us |   -0.53% |
| 131072B | Throughput |    13594.86 MB/s |    13899.12 MB/s |   +2.24% |
| 131072B | Latency    |          3.55 us |          3.64 us |   -2.54% |
| 262144B | Throughput |    16919.78 MB/s |    17338.67 MB/s |   +2.48% |
| 262144B | Latency    |          6.94 us |          6.67 us |   +3.89% |

#### Transport: ipc
| Size   | Metric     |  Standard libzmq |            zlink |  Diff (%) |
|--------|------------|------------------|------------------|-----------|
| 64B    | Throughput |      3.85 Mmsg/s |      3.77 Mmsg/s |   -2.04% |
| 64B    | Latency    |          4.36 us |          4.69 us |   -7.58% |
| 256B   | Throughput |      2.50 Mmsg/s |      2.40 Mmsg/s |   -4.32% |
| 256B   | Latency    |          4.44 us |          5.03 us |  -13.42% |
| 1024B  | Throughput |      0.97 Mmsg/s |      0.95 Mmsg/s |   -2.03% |
| 1024B  | Latency    |          4.45 us |          4.83 us |   -8.77% |
| 65536B | Throughput |     2172.23 MB/s |     2114.55 MB/s |   -2.66% |
| 65536B | Latency    |         11.82 us |         12.29 us |   -3.93% |
| 131072B | Throughput |     2547.19 MB/s |     2402.95 MB/s |   -5.66% |
| 131072B | Latency    |         18.05 us |         19.18 us |   -6.29% |
| 262144B | Throughput |     2554.46 MB/s |     2709.29 MB/s |   +6.06% |
| 262144B | Latency    |         29.29 us |         32.76 us |  -11.85% |

### PATTERN: ROUTER_ROUTER

#### Transport: tcp
| Size   | Metric     |  Standard libzmq |            zlink |  Diff (%) |
|--------|------------|------------------|------------------|-----------|
| 64B    | Throughput |      3.52 Mmsg/s |      3.48 Mmsg/s |   -0.97% |
| 64B    | Latency    |          3.99 us |          3.81 us |   +4.51% |
| 256B   | Throughput |      2.18 Mmsg/s |      2.22 Mmsg/s |   +1.65% |
| 256B   | Latency    |          4.08 us |          3.87 us |   +5.15% |
| 1024B  | Throughput |      0.90 Mmsg/s |      0.93 Mmsg/s |   +2.65% |
| 1024B  | Latency    |          4.09 us |          3.95 us |   +3.66% |
| 65536B | Throughput |     2213.81 MB/s |     2256.99 MB/s |   +1.95% |
| 65536B | Latency    |         11.21 us |         11.31 us |   -0.89% |
| 131072B | Throughput |     2156.27 MB/s |     2529.00 MB/s |  +17.29% |
| 131072B | Latency    |         17.78 us |         19.13 us |   -7.62% |
| 262144B | Throughput |     2551.83 MB/s |     2866.15 MB/s |  +12.32% |
| 262144B | Latency    |         37.89 us |         40.23 us |   -6.18% |

#### Transport: inproc
| Size   | Metric     |  Standard libzmq |            zlink |  Diff (%) |
|--------|------------|------------------|------------------|-----------|
| 64B    | Throughput |      5.50 Mmsg/s |      5.37 Mmsg/s |   -2.39% |
| 64B    | Latency    |          0.16 us |          0.16 us |   +0.00% |
| 256B   | Throughput |      4.60 Mmsg/s |      4.33 Mmsg/s |   -5.85% |
| 256B   | Latency    |          0.17 us |          0.16 us |   +3.03% |
| 1024B  | Throughput |      2.72 Mmsg/s |      2.68 Mmsg/s |   -1.16% |
| 1024B  | Latency    |          0.19 us |          0.19 us |   +0.00% |
| 65536B | Throughput |     9953.30 MB/s |    10401.83 MB/s |   +4.51% |
| 65536B | Latency    |          1.99 us |          2.00 us |   -0.75% |
| 131072B | Throughput |    13598.97 MB/s |    14212.39 MB/s |   +4.51% |
| 131072B | Latency    |          3.65 us |          3.67 us |   -0.69% |
| 262144B | Throughput |    17090.81 MB/s |    16312.42 MB/s |   -4.55% |
| 262144B | Latency    |          6.91 us |          6.84 us |   +0.94% |

#### Transport: ipc
| Size   | Metric     |  Standard libzmq |            zlink |  Diff (%) |
|--------|------------|------------------|------------------|-----------|
| 64B    | Throughput |      3.55 Mmsg/s |      3.55 Mmsg/s |   -0.02% |
| 64B    | Latency    |          3.56 us |          3.27 us |   +8.15% |
| 256B   | Throughput |      2.33 Mmsg/s |      2.29 Mmsg/s |   -1.62% |
| 256B   | Latency    |          3.60 us |          3.38 us |   +5.97% |
| 1024B  | Throughput |      0.99 Mmsg/s |      0.92 Mmsg/s |   -7.04% |
| 1024B  | Latency    |          3.68 us |          3.60 us |   +2.04% |
| 65536B | Throughput |     2167.02 MB/s |     2095.94 MB/s |   -3.28% |
| 65536B | Latency    |         12.42 us |         11.58 us |   +6.76% |
| 131072B | Throughput |     2507.30 MB/s |     2400.92 MB/s |   -4.24% |
| 131072B | Latency    |         22.52 us |         18.99 us |  +15.66% |
| 262144B | Throughput |     2780.72 MB/s |     2698.18 MB/s |   -2.97% |
| 262144B | Latency    |         36.14 us |         33.39 us |   +7.61% |

### PATTERN: ROUTER_ROUTER_POLL

#### Transport: tcp
| Size   | Metric     |  Standard libzmq |            zlink |  Diff (%) |
|--------|------------|------------------|------------------|-----------|
| 64B    | Throughput |      3.51 Mmsg/s |      3.39 Mmsg/s |   -3.19% |
| 64B    | Latency    |          4.45 us |          4.27 us |   +4.04% |
| 256B   | Throughput |      2.18 Mmsg/s |      2.17 Mmsg/s |   -0.45% |
| 256B   | Latency    |          4.53 us |          4.33 us |   +4.42% |
| 1024B  | Throughput |      0.84 Mmsg/s |      0.88 Mmsg/s |   +5.13% |
| 1024B  | Latency    |          4.54 us |          4.38 us |   +3.52% |
| 65536B | Throughput |     2071.02 MB/s |     2186.43 MB/s |   +5.57% |
| 65536B | Latency    |         11.85 us |         11.54 us |   +2.66% |
| 131072B | Throughput |     2295.86 MB/s |     2543.21 MB/s |  +10.77% |
| 131072B | Latency    |         18.11 us |         19.36 us |   -6.87% |
| 262144B | Throughput |     2714.30 MB/s |     2887.25 MB/s |   +6.37% |
| 262144B | Latency    |         37.73 us |         40.25 us |   -6.67% |

#### Transport: inproc
| Size   | Metric     |  Standard libzmq |            zlink |  Diff (%) |
|--------|------------|------------------|------------------|-----------|
| 64B    | Throughput |      5.56 Mmsg/s |      5.37 Mmsg/s |   -3.40% |
| 64B    | Latency    |          0.53 us |          0.47 us |  +11.32% |
| 256B   | Throughput |      4.46 Mmsg/s |      4.50 Mmsg/s |   +0.95% |
| 256B   | Latency    |          0.52 us |          0.46 us |  +11.54% |
| 1024B  | Throughput |      2.57 Mmsg/s |      2.67 Mmsg/s |   +3.64% |
| 1024B  | Latency    |          0.55 us |          0.49 us |  +10.91% |
| 65536B | Throughput |     8243.68 MB/s |     9454.01 MB/s |  +14.68% |
| 65536B | Latency    |          2.38 us |          2.33 us |   +1.68% |
| 131072B | Throughput |    11847.65 MB/s |    12711.23 MB/s |   +7.29% |
| 131072B | Latency    |          4.17 us |          4.03 us |   +3.24% |
| 262144B | Throughput |    15543.32 MB/s |    15441.51 MB/s |   -0.66% |
| 262144B | Latency    |          7.27 us |          7.35 us |   -1.03% |

#### Transport: ipc
| Size   | Metric     |  Standard libzmq |            zlink |  Diff (%) |
|--------|------------|------------------|------------------|-----------|
| 64B    | Throughput |      3.61 Mmsg/s |      3.61 Mmsg/s |   +0.06% |
| 64B    | Latency    |          4.04 us |          3.79 us |   +6.30% |
| 256B   | Throughput |      2.33 Mmsg/s |      2.34 Mmsg/s |   +0.12% |
| 256B   | Latency    |          4.05 us |          3.90 us |   +3.83% |
| 1024B  | Throughput |      0.94 Mmsg/s |      0.83 Mmsg/s |  -12.16% |
| 1024B  | Latency    |          4.10 us |          4.09 us |   +0.12% |
| 65536B | Throughput |     2057.28 MB/s |     1948.45 MB/s |   -5.29% |
| 65536B | Latency    |         12.83 us |         11.91 us |   +7.21% |
| 131072B | Throughput |     2416.48 MB/s |     2295.09 MB/s |   -5.02% |
| 131072B | Latency    |         23.35 us |         19.13 us |  +18.07% |
| 262144B | Throughput |     2699.13 MB/s |     2602.29 MB/s |   -3.59% |
| 262144B | Latency    |         37.05 us |         32.66 us |  +11.82% |
