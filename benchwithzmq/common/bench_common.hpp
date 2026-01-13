#ifndef BENCH_COMMON_HPP
#define BENCH_COMMON_HPP

#include <chrono>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <iostream>
#include <iomanip>

// --- Configuration ---
static const std::vector<size_t> MSG_SIZES = {64, 256, 1024, 65536, 131072, 262144};
static const std::vector<std::string> TRANSPORTS = {"tcp", "inproc", "ipc"};

// --- Stopwatch ---
class stopwatch_t {
public:
    void start() { _start = std::chrono::high_resolution_clock::now(); }
    double elapsed_ms() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - _start).count();
    }
private:
    std::chrono::high_resolution_clock::time_point _start;
};

inline void print_result(const std::string& lib_type,
                         const std::string& pattern,
                         const std::string& transport,
                         size_t size,
                         double throughput,
                         double latency) {
    std::cout << "RESULT," << lib_type << "," << pattern << "," << transport << "," << size
              << ",throughput," << std::fixed << std::setprecision(2) << throughput << std::endl;
    std::cout << "RESULT," << lib_type << "," << pattern << "," << transport << "," << size
              << ",latency," << std::fixed << std::setprecision(2) << latency << std::endl;
}

inline std::string make_endpoint(const std::string& transport, const std::string& id) {
    if (transport == "inproc") return "inproc://" + id;
    if (transport == "ipc") return "ipc:///tmp/bench_" + id + ".ipc";
    if (transport == "ws") {
        static int ws_port = 6555;
        return "ws://127.0.0.1:" + std::to_string(ws_port++);
    }
    static int port = 5555;
    return "tcp://127.0.0.1:" + std::to_string(port++);
}

inline int bench_override_count(int fallback) {
    const char *env = std::getenv("BENCH_MSG_COUNT");
    if (!env || !*env) return fallback;
    char *end = nullptr;
    long value = std::strtol(env, &end, 10);
    if (end == env || value <= 0) return fallback;
    if (value > INT_MAX) return fallback;
    return static_cast<int>(value);
}

#endif
