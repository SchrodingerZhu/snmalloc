# RBTree Replacement Benchmark Report (2026-02-25)

## Scope
- Tree backend variants:
  - 0: Red-Black tree (baseline)
  - 1: WAVL 2-bit diff
  - 2: WAVL 1-bit parity
- Large alloc benchmark workload increased from 100000 to 400000 iterations (x4).
- Repetitions increased to 10 runs per variant for statistics (mean/stddev/min/max).
- Hyperfine also run with 10 repetitions.

## Package + Remote Run
- Archive: /tmp/snmalloc-rbtree-variants-20260225.zip
- Uploaded to: spark:/tmp/snmalloc-rbtree-variants-20260225.zip
- Remote workdir: /tmp/snmalloc-rbtree-variants-20260225

## Environments
- Local: Linux mtheory 6.19.3-2-cachyos #1 SMP PREEMPT_DYNAMIC Thu, 19 Feb 2026 21:03:04 +0000 x86_64 GNU/Linux
- Spark: Linux promaxgb10-edf0 6.14.0-1015-nvidia #15-Ubuntu SMP PREEMPT_DYNAMIC Tue Nov 25 18:02:16 UTC 2025 aarch64 aarch64 aarch64 GNU/Linux

## Local 10-Run Metric Stats (ns)
| variant | metric | n | mean_ns | stddev_ns | min_ns | max_ns | delta_vs_rb_ns | delta_vs_rb_pct |
|---|---|---:|---:|---:|---:|---:|---:|---:|
| rb | alloc_dealloc | 10 | 34553400.70 | 1310666.08 | 32439904 | 37218008 | 0.00 | +0.00% |
| w2 | alloc_dealloc | 10 | 32305511.90 | 1422450.42 | 30445550 | 34602621 | -2247888.80 | -6.51% |
| w1 | alloc_dealloc | 10 | 30988844.30 | 1278594.28 | 28944532 | 33868143 | -3564556.40 | -10.32% |
| rb | batch_alloc_dealloc | 10 | 88992666.30 | 3468105.48 | 84547398 | 95170524 | 0.00 | +0.00% |
| w2 | batch_alloc_dealloc | 10 | 68550446.70 | 2775490.79 | 64034170 | 71758820 | -20442219.60 | -22.97% |
| w1 | batch_alloc_dealloc | 10 | 69224794.00 | 1657977.26 | 66713419 | 71818604 | -19767872.30 | -22.21% |
| rb | alloc_touch_dealloc | 10 | 37034922.30 | 1461485.12 | 35165642 | 39571968 | 0.00 | +0.00% |
| w2 | alloc_touch_dealloc | 10 | 33708889.80 | 1158505.79 | 31419122 | 35025455 | -3326032.50 | -8.98% |
| w1 | alloc_touch_dealloc | 10 | 32279160.80 | 1207555.98 | 30970969 | 34691720 | -4755761.50 | -12.84% |

## Local Hyperfine (10 runs)
| Command | Mean [ms] | Min [ms] | Max [ms] | Relative |
|:---|---:|---:|---:|---:|
| `./build-rb/perf-large_alloc-fast` | 162.5 ± 4.8 | 157.3 | 169.7 | 1.03 ± 0.08 |
| `./build-w2/perf-large_alloc-fast` | 157.2 ± 10.9 | 148.8 | 182.5 | 1.00 |
| `./build-w1/perf-large_alloc-fast` | 176.3 ± 12.7 | 157.7 | 191.0 | 1.12 ± 0.11 |

## Spark (aarch64) 10-Run Metric Stats (ns)
| variant | metric | n | mean_ns | stddev_ns | min_ns | max_ns | delta_vs_rb_ns | delta_vs_rb_pct |
|---|---|---:|---:|---:|---:|---:|---:|---:|
| rb | alloc_dealloc | 10 | 36912757.90 | 8786222.89 | 32479343 | 54836176 | 0.00 | +0.00% |
| w2 | alloc_dealloc | 10 | 46133636.00 | 9692479.56 | 32085710 | 57150088 | 9220878.10 | +24.98% |
| w1 | alloc_dealloc | 10 | 39381360.00 | 10398906.56 | 30973994 | 55649475 | 2468602.10 | +6.69% |
| rb | batch_alloc_dealloc | 10 | 120928117.90 | 4419886.10 | 115008482 | 125408191 | 0.00 | +0.00% |
| w2 | batch_alloc_dealloc | 10 | 88191151.10 | 204937.33 | 87790801 | 88468595 | -32736966.80 | -27.07% |
| w1 | batch_alloc_dealloc | 10 | 87219709.70 | 315244.24 | 86645261 | 87587249 | -33708408.20 | -27.87% |
| rb | alloc_touch_dealloc | 10 | 32501404.10 | 100263.29 | 32212927 | 32582160 | 0.00 | +0.00% |
| w2 | alloc_touch_dealloc | 10 | 32077673.10 | 152537.46 | 31795581 | 32269678 | -423731.00 | -1.30% |
| w1 | alloc_touch_dealloc | 10 | 31466960.90 | 102171.68 | 31324316 | 31684637 | -1034443.20 | -3.18% |

## Spark Hyperfine (10 runs)
| Command | Mean [ms] | Min [ms] | Max [ms] | Relative |
|:---|---:|---:|---:|---:|
| `./build-rb/perf-large_alloc-fast` | 192.3 ± 9.3 | 180.9 | 206.2 | 1.24 ± 0.09 |
| `./build-w2/perf-large_alloc-fast` | 156.5 ± 6.3 | 152.3 | 168.6 | 1.01 ± 0.07 |
| `./build-w1/perf-large_alloc-fast` | 155.2 ± 8.0 | 149.6 | 169.4 | 1.00 |

## Notes
- In-program metric timers (ns lines from perf-large_alloc-fast) and hyperfine wall-time can rank variants differently.
- This report is post-fix and supersedes earlier numbers from intermediate iterations.
