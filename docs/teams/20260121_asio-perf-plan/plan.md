# ASIO 성능 개선 계획 (zlink, proactor 중심)

목표
- 전 패턴/전송에서 전반적인 처리량/지연 개선을 노린다.
- 회귀 없이 공통 효과가 있는 변경을 우선한다.

배경 및 구조 관점
- 기존 reactor 최적화는 poll/epoll 호출과 이벤트 처리 루프가 핵심 병목이었다.
- 현재는 proactor(ASIO) 구조로 전환되어 completion handler, 큐잉, 스케줄링,
  버퍼 재사용이 주요 병목이 될 가능성이 높다.
- 따라서 최적화 포인트도 "이벤트 감지"보다 "완료 핸들러 경로"에 맞춘다.

범위
- ASIO 경로: tcp/ipc transport, asio_engine, zmtp/zmp engine,
  encoder/decoder hot loop.
- 공통 비용: handler 할당, 큐잉/디스패치, timer/wakeup, write coalescing,
  버퍼 재사용.

비범위
- ASIO backend 교체.
- API/프로토콜 의미 변경.
- allocator 교체(미malloc 제거 완료).

기준선 및 측정
1) 빌드
   - Release, build/bench 사용.
   - 고정 플래그: -DBUILD_BENCHMARKS=ON -DZMQ_CXX_STANDARD=20.
2) 벤치마크
   - benchwithzmq/run_benchmarks.sh --runs 10 --reuse-build
   - 프로토콜 기본값: ZMP (ZLINK_PROTOCOL unset).
   - taskset 기본 유지, BENCH_NO_TASKSET 사용 시 기록.
   - 결과: docs/teams/20260121_asio-perf-plan/results/에 저장.
3) 성공 기준
   - 전 패턴/전송에서 일관된 회귀가 없어야 한다.
   - 개선이 1~2개 패턴에만 제한되면 보류한다.

프로파일링 계획 (proactor 중심, WSL2 환경 대응)
- 대표 케이스에서 공통 핫스팟 수집:
  - 패턴: PAIR, PUBSUB, ROUTER_ROUTER
  - 사이즈: 64B, 64KB, 256KB
  - 전송: tcp, inproc, ipc
- WSL2에서는 perf 사용이 불가하므로 다음 대안을 사용한다:
  - syscall 비중: strace -c -f -o /tmp/strace.txt -- <bench>
  - 코드 레벨: valgrind --tool=callgrind --callgrind-out-file=/tmp/callgrind.out <bench>
  - 필요 시, 주요 경로에 간단 타이밍 계측 추가(조건부 로그).
- 관찰 포인트:
  - handler 할당/해제 비용 (std::function/boost::bind 등)
  - post/dispatch 오버헤드, strand 직렬화 비용
  - async_read/write 체인 길이와 빈도
  - 버퍼 copy/resize, writev 구성 비용
  - timer reschedule 및 wakeup 빈도
  - syscalls 비율 (writev/write, recv, epoll_wait)

최적화 가설 (proactor 맞춤)
1) handler 할당 최소화
   - per-connection handler allocator 재사용.
   - hot callback에서 동적 할당 제거.
2) 완료 핸들러 체인 단순화
   - 불필요한 post/dispatch 제거.
   - 즉시 완료 경로에서 추가 큐잉을 피한다.
3) write coalescing / batching
   - 작은 async_write 호출 병합.
   - gather/writev 사용 시 copy를 최소화.
4) read/write pump 정리
   - 매 메시지마다 async_* 재등록하지 않고, 가능한 범위에서
     지속적(read-ahead/write-ahead) 수행.
5) 버퍼 재사용 전략
   - read/write 버퍼 재사용, reserve 일관화.
6) timer/wakeup 최소화
   - 불필요한 타이머 재설정 방지.
   - idle 구간 wakeup 횟수 축소.
7) io_context 실행 정책
   - stop/start 반복 방지, io_thread 당 작업량 균형.
   - 과도한 strand 직렬화 제거.
8) 전송 옵션(선택적, gated)
   - TCP_NODELAY, quickack, busy-poll은 효과 검증 후만 적용.

실험 절차
1) 가설 1개 선택, 최소 변경으로 구현.
2) 소규모 측정(1 패턴 x 2 사이즈)으로 방향 확인.
3) 긍정적이면 전체 패턴 벤치 수행.
4) 회귀 시 즉시 롤백하고 기록.
5) 복수 변경을 한 커밋에 묶지 않는다.

리스크 및 완화
- 노이즈: runs=10 고정, 환경 기록.
- 착시 개선: 전체 패턴 통과 조건.
- ASIO 변경의 correctness 리스크: ctest 수행.

산출물
- 변경별 벤치 로그/요약.
- perf trace 및 flamegraph.
- 최종 요약(전/후 비교, 채택 변경 리스트).

실행 TODO (중단 없이 진행)
- [ ] results/ 디렉토리 준비: docs/teams/20260121_asio-perf-plan/results/
- [ ] 환경 기록: git commit, 브랜치, uname -a, WSL 버전, CPU/메모리
- [ ] 기준선 고정:
  - runs=10, ZMP 기본, taskset 기본 유지
  - 명령: benchwithzmq/run_benchmarks.sh --runs 10 --reuse-build --output <파일>
- [ ] 기준선 저장 규칙:
  - 예: results/bench_baseline_zmp_runs10_YYYYMMDD.txt
  - ZMTP는 results/bench_baseline_zmtp_runs10_YYYYMMDD.txt
- [ ] 프로파일링(WSL2 대안):
  - strace: strace -c -f -o /tmp/strace_pair_tcp_64.txt -- build/bench/bin/comp_zlink_pair zlink tcp 64
  - callgrind: valgrind --tool=callgrind --callgrind-out-file=/tmp/callgrind_pair_tcp_64.out build/bench/bin/comp_zlink_pair zlink tcp 64
  - 결과 요약을 results/ 아래에 텍스트로 정리
- [ ] 가설 백로그 작성:
  - handler 할당, post/dispatch, strand, write coalesce, buffer reuse, timer/wakeup, io_context 정책
  - 각 항목에 예상 효과/리스크/측정 방법을 1줄로 기록
- [ ] 반복 루프(항목당 1회씩):
  - [ ] 가설 1개 선택, 최소 변경 구현
  - [ ] 미니 벤치(1 패턴 x 2 사이즈) 결과 기록
  - [ ] 긍정 시 전체 벤치 실행 및 기록
  - [ ] 회귀 시 즉시 롤백/기록
  - [ ] ctest 수행(변경 확정 시)
- [ ] 종료 조건:
  - 전 패턴/전송에서 일관된 개선이 1회 이상 확인되거나
  - 3회 연속 의미 있는 개선이 없으면 중단/재평가
- [ ] 최종 요약 작성:
  - 채택 변경 목록, 전/후 델타, 남은 리스크

다음 단계
1) results/ 디렉토리 생성.
2) baseline runs=10 저장.
3) tcp/ipc 대표 케이스 perf 수집.
4) 공통 핫스팟 Top3 선정 후 1개부터 실험.
