# ASIO Large Writev Path 계획

## 배경

- 브랜치: `feature/asio-large-writev`
- 목표: ASIO를 유지하면서 64KB+ 대용량 성능 개선
- 현재 상태: allocator/PMR + mimalloc은 소/중형 개선은 있었지만, 대형 구간(특히 latency, inproc 대형 throughput)은 여전히 회귀

## 현재 베이스라인 요약 (공유된 결과 기준)

- 64B/256B/1024B: tcp/ipc는 throughput 개선 경향, inproc은 큰 폭 개선
- 64KB+: throughput은 혼재, tcp/ipc에서 latency 회귀가 빈번
- inproc 대형 throughput은 지속적으로 회귀
- 위 경향은 PAIR/PUBSUB/DEALER/ROUTER 계열 전반에 공통

## 가설

- 대형 회귀의 주 원인은 ASIO write 경로
  - 대형 payload가 여러 async completion으로 분할됨
  - encoder 배치/헤더 처리로 추가 복사가 발생
  - 헤더+바디가 같은 버퍼로 합쳐지며 zero-copy가 거의 타지 않음

## 제안 변경안 (ASIO 유지 전제)

- 대형 메시지 전용 fast path 추가
  - 작은 헤더 버퍼 + 바디 버퍼를 분리 구성
  - scatter/gather async write로 한 번에 전송
  - 대형 바디를 encoder 배치 버퍼에 복사하지 않음
- 임계값(예: 64KB+)을 기준으로 분기하고 테스트로 튜닝
- 대상 범위는 TCP로 한정하고, TLS/WSS는 별도 분석 대상으로 유지

## 범위 (예정)

- ZMTP ASIO 엔진의 write 경로만 변경
- 프로토콜/와이어 포맷 변경 없음
- allocator 변경 없음

## 리스크/주의사항

- 단일 메시지가 단일 write 체인/완료로 유지되도록 보장
- async 전송 중 바디 버퍼 수명 보장
- 소형 메시지 throughput 회귀 방지
- TLS/WSS는 암호화 계층에서 재버퍼링이 발생할 수 있어 기대 효과가 낮을 수 있음
- async_write의 transfer_all 처리 시 분할 전송/에러 처리 경로를 명확히 해야 함

## 다음 단계

1) 대형 메시지 header+body scatter/gather async write 경로 구현
2) 대형 경로 사용 여부를 확인할 로그/지표 추가
3) 64KB/128KB/256KB 재측정 (tcp/ipc/inproc)
4) large-path 사용률/비율 로깅 추가
5) baseline 대비 throughput/latency 비교

## 벤치마크 로그

- `docs/team/20260118_feature-asio-large-writev/01_benchmark_tcp_large_runs10.txt`
- `docs/team/20260118_feature-asio-large-writev/02_benchmark_tcp_large_runs10_summary.md`
- `docs/team/20260118_feature-asio-large-writev/03_benchmark_tcp_large_runs10_sync_try.txt`
- `docs/team/20260118_feature-asio-large-writev/04_benchmark_tcp_large_runs10_sync_loop.txt`
- `docs/team/20260118_feature-asio-large-writev/05_benchmark_tcp_large_runs10_threshold128k.txt`
- `docs/team/20260118_feature-asio-large-writev/06_benchmark_tcp_large_runs10_sync_default.txt`
- `docs/team/20260118_feature-asio-large-writev/07_benchmark_tcp_large_runs10_no_sync_loop.txt` (중단)
- `docs/team/20260118_feature-asio-large-writev/08_benchmark_tcp_large_runs10_no_sync_loop_full.txt` (중단)
- `docs/team/20260118_feature-asio-large-writev/09_benchmark_tcp_large_runs10_no_sync_loop_full.txt`
- `docs/team/20260118_feature-asio-large-writev/10_benchmark_tcp_large_runs10_async_write_some.txt`

## 진행 현황 (2026-01-19)

- 구현 완료: 대형 메시지 header/body 분리 + scatter/gather writev 경로
- 보강: sync write 우선 시도 + 부분 전송 offset 처리
- 튜닝 시도: 64KB 임계값 유지/128KB 상향 비교
- 튜닝 시도: 대형 write path의 sync loop 제거 (partial write는 async로 전환)
- 결론: 64KB/128KB throughput 개선 신호는 있으나, 256KB 및 latency 회귀 지속
