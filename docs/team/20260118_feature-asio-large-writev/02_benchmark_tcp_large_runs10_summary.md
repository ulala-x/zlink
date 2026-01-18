# TCP 대용량 벤치마크 결과 (runs=10)

## 실행 조건

- 브랜치: `feature/asio-large-writev`
- 설정: `BENCH_TRANSPORTS=tcp`, 사이즈 `65536,131072,262144`, runs=10
- 로그:
  - `docs/team/20260118_feature-asio-large-writev/01_benchmark_tcp_large_runs10.txt`
  - `docs/team/20260118_feature-asio-large-writev/03_benchmark_tcp_large_runs10_sync_try.txt`
  - `docs/team/20260118_feature-asio-large-writev/04_benchmark_tcp_large_runs10_sync_loop.txt`
  - `docs/team/20260118_feature-asio-large-writev/05_benchmark_tcp_large_runs10_threshold128k.txt`
  - `docs/team/20260118_feature-asio-large-writev/06_benchmark_tcp_large_runs10_sync_default.txt`
  - `docs/team/20260118_feature-asio-large-writev/09_benchmark_tcp_large_runs10_no_sync_loop_full.txt`
  - `docs/team/20260118_feature-asio-large-writev/10_benchmark_tcp_large_runs10_async_write_some.txt`
  - `docs/team/20260118_feature-asio-large-writev/11_benchmark_tcp_large_runs10_header32.txt`
  - `docs/team/20260118_feature-asio-large-writev/12_benchmark_tcp_large_runs10_threshold512k.txt`
  - `docs/team/20260118_feature-asio-large-writev/13_benchmark_tcp_large_runs10_sync_on_async_write_some.txt`
  - `docs/team/20260118_feature-asio-large-writev/14_benchmark_tcp_large_runs10_recv_burst.txt`

## 결과 요약

- 64KB/128KB throughput은 다수 패턴에서 개선 신호가 존재
  - sync 시도/loop, sync 기본값, 임계값 128KB 비교에서도 경향 유사
- 256KB throughput은 전반적으로 지속 회귀
  - 패턴 전반에서 -1% ~ -15% 범위가 반복적으로 관찰됨
- latency는 대부분 패턴에서 악화
  - 특히 TCP 대형 구간에서 -20% 이상 하락 케이스가 빈번
  - PUBSUB 일부 구간만 예외적으로 개선
- 임계값 128KB 상향 및 sync 경로 강화는 latency 회귀를 해소하지 못함
- sync loop 제거 시도에서도 latency 회귀는 지속됨 (throughput은 일부 개선)
- async_write_some 전환은 throughput 개선 폭을 키웠으나, latency는 여전히 큰 폭의 음수 diff
- header buffer 축소(64B→32B)는 latency 개선에 유의미한 영향이 없었음
- large path 비활성에 가까운 임계값(512KB)에서도 latency 회귀가 유지됨
- sync write 강제(ZMQ_ASIO_TCP_SYNC_WRITE=1)도 latency 회귀 개선 효과는 없음
- recv burst(추가 read_some 최대 4회)는 throughput 개선폭 확대, latency 회귀는 유지

## 판단

- TCP 대용량에서 throughput 개선 신호는 있으나, latency 악화와 256KB 회귀가 잔존
- “전반적 개선” 기준에는 아직 미달

## 다음 액션 후보

1) 256KB 이상 구간에 대해 별도 전송 전략(직접 writev/단일 syscall) 분리 검토
2) large-path 사용률/분할 횟수 로깅 추가 후 실 적용 비중 확인
3) TCP 전송 경로의 syscall 수/완료 횟수를 줄이는 방식(예: transfer-all 정책 변경) 검토
