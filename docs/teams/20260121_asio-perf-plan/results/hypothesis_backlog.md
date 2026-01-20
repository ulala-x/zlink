# ASIO 최적화 가설 백로그

- H1. async_writev 경로를 ASIO buffer sequence로 기본 전환
  - 기대효과: 사용자 정의 writev 루프의 heap/람다 비용 감소
  - 리스크: ASIO 내부 루프 비용 증가 가능
  - 측정: ROUTER_ROUTER tcp 64/65536, 전체 패턴 벤치

- H2. completion handler 체인 단순화(post/dispatch 최소화)
  - 기대효과: 큐잉/스케줄링 오버헤드 감소
  - 리스크: 동작 순서 변경 위험
  - 측정: strace poll/epoll_wait 비중 감소 여부 + 벤치

- H3. read/write pump 재등록 최소화
  - 기대효과: handler 수 감소, wakeup 감소
  - 리스크: backpressure 처리 영향
  - 측정: bench + 핸들러 호출 횟수 계측

- H4. 버퍼 재사용 및 reserve 정규화
  - 기대효과: realloc 감소, cache locality 개선
  - 리스크: 메모리 footprint 증가
  - 측정: large size throughput + RSS 변화

- H5. 타이머 재설정 최소화
  - 기대효과: timer queue churn 감소
  - 리스크: heartbeat/timeout 지연
  - 측정: timer 관련 syscall 감소 + 기능 테스트
