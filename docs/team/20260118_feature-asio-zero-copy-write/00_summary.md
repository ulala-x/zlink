# ASIO zero-copy async write (WIP)

## 변경 내용 요약
- ASIO 엔진의 async write 경로에서 사용자 공간 복사를 제거하고, `_outpos/_outsize`를 그대로 async_write_some에 전달하도록 변경.
- async write 완료 시 `_outpos/_outsize`를 진행시키고, 남은 바이트가 있으면 동일 버퍼로 재전송.
- encoder 문서 주석을 수정해 "async에서도 encode/load_msg 호출이 없으면 zero-copy 가능"함을 명시.

## 핵심 의도
- 큰 메시지(>=8KB 포함)에서 추가 memcpy를 제거해 libzmq 대비 성능 격차(특히 TCP 대용량)를 줄이는 목적.

## 벤치마크 결과 (PAIR, TCP)
- 실행 로그: `docs/team/20260118_feature-asio-zero-copy-write/01_pair_tcp_1k_8k_runs10.txt`
- 설정: `BENCH_TRANSPORTS=tcp BENCH_MSG_COUNT=1000 --runs 10 --msg-sizes 1024,8192`

요약:
- 1024B throughput: +44.31% (latency -1.71%)
- 8192B throughput: +21.69% (latency -1.91%)

## 참고
- `test_pair_tcp` 통과 확인.
