# TCP 대용량 벤치마크 결과 (runs=10)

## 실행 조건

- 브랜치: `feature/asio-large-writev`
- 설정: `BENCH_TRANSPORTS=tcp`, 사이즈 `65536,131072,262144`, runs=10
- 로그: `docs/team/20260118_feature-asio-large-writev/01_benchmark_tcp_large_runs10.txt`

## 결과 요약

- 64KB/128KB throughput은 대부분 패턴에서 개선됨
  - 예: PUBSUB 64KB +21.12%, 128KB +8.61%
  - ROUTER_ROUTER_POLL 64KB +9.84%, 128KB +11.49%
  - 일부 예외: PAIR 64KB -0.73%
- 256KB throughput은 전반적으로 여전히 회귀
  - 대다수 패턴에서 -1% ~ -15% 범위
- latency는 대부분 패턴에서 악화
  - 64KB/128KB 구간에서도 음수 diff가 많음
  - PUBSUB은 64KB/128KB에서 latency 개선이 관찰됨

## 판단

- TCP 대용량에서 throughput 개선 신호는 확인됨
- 그러나 256KB 회귀와 latency 악화가 지속되어 “전반적 개선” 기준에는 아직 미달

## 다음 액션 후보

1) 256KB 이상 구간에 대해 임계값/전송 전략을 별도로 분리
2) large-path 사용률/비율 로깅 추가 후 실제 적용 비중 확인
3) async_write 분할/완료 횟수 감소를 위한 설정 조정 검토
