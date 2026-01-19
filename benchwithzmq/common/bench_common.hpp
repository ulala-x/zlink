#ifndef BENCH_COMMON_HPP
#define BENCH_COMMON_HPP

#include <chrono>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <thread>
#include <zmq.h>
#ifndef ZMQ_HAVE_WINDOWS
#include <unistd.h>
#include <sys/stat.h>
#endif
#include "certs/test_certs.hpp"

// --- Configuration ---
static const std::vector<size_t> MSG_SIZES = {64, 256, 1024, 65536, 131072, 262144};
static const std::vector<std::string> TRANSPORTS = {"tcp", "tls", "inproc", "ipc"};
static const size_t MAX_SOCKET_STRING = 256;
static const int SETTLE_TIME_MS = 300;
#if defined(ZMQ_TLS_CERT)
static const char *TLS_HOSTNAME = "localhost";
#endif

#if defined(ZMQ_TLS_CERT)
struct tls_bench_files_t {
    std::string dir;
    std::string ca_cert;
    std::string server_cert;
    std::string server_key;
};
#endif

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

inline bool bench_debug_enabled() {
    static const bool enabled = std::getenv("BENCH_DEBUG") != nullptr;
    return enabled;
}

inline int bench_trace_limit() {
    static const int limit = []() {
        if (!bench_debug_enabled())
            return 0;
        const char *env = std::getenv("BENCH_TRACE_LIMIT");
        if (!env)
            return 20;
        const int val = std::atoi(env);
        return val > 0 ? val : 0;
    }();
    return limit;
}

inline int bench_trace_next_id() {
    static int trace_id = 0;
    return ++trace_id;
}

inline bool set_sockopt_int(void *socket_, int option_, int value_,
                            const char *name_) {
    const int rc = zmq_setsockopt(socket_, option_, &value_, sizeof(value_));
    if (rc != 0 && bench_debug_enabled()) {
        std::cerr << "setsockopt(" << name_ << ") failed: "
                  << zmq_strerror(zmq_errno()) << std::endl;
    }
    if (bench_debug_enabled()) {
        int out = 0;
        size_t out_size = sizeof(out);
        const int grc = zmq_getsockopt(socket_, option_, &out, &out_size);
        if (grc == 0) {
            std::cerr << "setsockopt(" << name_ << ") = " << out << std::endl;
        }
    }
    return rc == 0;
}

#if defined(ZMQ_TLS_CERT)
inline bool write_pem_file(const std::string &path_, const char *pem_) {
    FILE *fp = fopen(path_.c_str(), "wb");
    if (!fp)
        return false;
    const size_t len = strlen(pem_);
    const size_t written = fwrite(pem_, 1, len, fp);
    fclose(fp);
    return written == len;
}

inline void cleanup_tls_files(tls_bench_files_t *files_) {
    if (!files_)
        return;
    remove(files_->ca_cert.c_str());
    remove(files_->server_cert.c_str());
    remove(files_->server_key.c_str());
#ifdef ZMQ_HAVE_WINDOWS
    _rmdir(files_->dir.c_str());
#else
    rmdir(files_->dir.c_str());
#endif
}

inline tls_bench_files_t &tls_bench_files() {
    static tls_bench_files_t files = []() {
        tls_bench_files_t out;
#ifdef ZMQ_HAVE_WINDOWS
        char tmp_dir[MAX_PATH] = "";
        if (tmpnam_s(tmp_dir) == 0) {
            _mkdir(tmp_dir);
            out.dir.assign(tmp_dir);
        }
#else
        char tmp_dir[] = "zmp_tls_XXXXXX";
        char *dir = mkdtemp(tmp_dir);
        if (dir)
            out.dir.assign(dir);
#endif
        out.ca_cert = out.dir + "/ca.crt";
        out.server_cert = out.dir + "/server.crt";
        out.server_key = out.dir + "/server.key";
        write_pem_file(out.ca_cert, zmq::test_certs::ca_cert_pem);
        write_pem_file(out.server_cert, zmq::test_certs::server_cert_pem);
        write_pem_file(out.server_key, zmq::test_certs::server_key_pem);
        return out;
    }();
    static bool registered = false;
    if (!registered) {
        registered = true;
        std::atexit([]() { cleanup_tls_files(&tls_bench_files()); });
    }
    return files;
}

inline bool apply_tls_server_opts(void *socket_) {
    tls_bench_files_t &files = tls_bench_files();
    if (!set_sockopt_int(socket_, ZMQ_TLS_TRUST_SYSTEM, 0, "ZMQ_TLS_TRUST_SYSTEM"))
        return false;
    if (zmq_setsockopt(socket_, ZMQ_TLS_CERT, files.server_cert.c_str(),
                       files.server_cert.size()) != 0)
        return false;
    if (zmq_setsockopt(socket_, ZMQ_TLS_KEY, files.server_key.c_str(),
                       files.server_key.size()) != 0)
        return false;
    return true;
}

inline bool apply_tls_client_opts(void *socket_) {
    tls_bench_files_t &files = tls_bench_files();
    if (!set_sockopt_int(socket_, ZMQ_TLS_TRUST_SYSTEM, 0, "ZMQ_TLS_TRUST_SYSTEM"))
        return false;
    if (zmq_setsockopt(socket_, ZMQ_TLS_CA, files.ca_cert.c_str(),
                       files.ca_cert.size()) != 0)
        return false;
    if (zmq_setsockopt(socket_, ZMQ_TLS_HOSTNAME, TLS_HOSTNAME,
                       strlen(TLS_HOSTNAME)) != 0)
        return false;
    return true;
}

inline void configure_transport_socket(void *socket_, const std::string &transport,
                                      bool is_bind) {
    if (transport != "tls")
        return;
    if (is_bind)
        apply_tls_server_opts(socket_);
    else
        apply_tls_client_opts(socket_);
}
#else
inline void configure_transport_socket(void *, const std::string &, bool) {}
#endif

inline void apply_debug_timeouts(void *socket_, const std::string &transport) {
    if (!bench_debug_enabled())
        return;
    if (transport == "tcp" || transport == "ws" || transport == "tls") {
        const int timeout_ms = 2000;
        set_sockopt_int(socket_, ZMQ_SNDTIMEO, timeout_ms, "ZMQ_SNDTIMEO");
        set_sockopt_int(socket_, ZMQ_RCVTIMEO, timeout_ms, "ZMQ_RCVTIMEO");
    }
}

inline int bench_send(void *socket_, const void *buf_, size_t len_, int flags_,
                      const char *tag_) {
    int trace_id = 0;
    const int limit = bench_trace_limit();
    if (limit > 0) {
        trace_id = bench_trace_next_id();
        if (trace_id <= limit) {
            std::cerr << "send start #" << trace_id << " (" << tag_
                      << ", len=" << len_ << ")" << std::endl;
        }
    }
    const int rc = zmq_send(socket_, buf_, len_, flags_);
    if (rc == -1 && bench_debug_enabled()) {
        std::cerr << "send failed (" << tag_ << "): "
                  << zmq_strerror(zmq_errno()) << std::endl;
    }
    if (limit > 0 && trace_id > 0 && trace_id <= limit) {
        std::cerr << "send done #" << trace_id << " rc=" << rc << std::endl;
    }
    return rc;
}

inline int bench_send_fast(void *socket_, const void *buf_, size_t len_,
                           int flags_, const char *tag_) {
    if (!bench_debug_enabled())
        return zmq_send(socket_, buf_, len_, flags_);
    return bench_send(socket_, buf_, len_, flags_, tag_);
}

inline int bench_recv(void *socket_, void *buf_, size_t len_, int flags_,
                      const char *tag_) {
    int trace_id = 0;
    const int limit = bench_trace_limit();
    if (limit > 0) {
        trace_id = bench_trace_next_id();
        if (trace_id <= limit) {
            std::cerr << "recv start #" << trace_id << " (" << tag_
                      << ", len=" << len_ << ")" << std::endl;
        }
    }
    const int rc = zmq_recv(socket_, buf_, len_, flags_);
    if (rc == -1 && bench_debug_enabled()) {
        std::cerr << "recv failed (" << tag_ << "): "
                  << zmq_strerror(zmq_errno()) << std::endl;
    }
    if (limit > 0 && trace_id > 0 && trace_id <= limit) {
        std::cerr << "recv done #" << trace_id << " rc=" << rc << std::endl;
    }
    return rc;
}

inline int bench_recv_fast(void *socket_, void *buf_, size_t len_,
                           int flags_, const char *tag_) {
    if (!bench_debug_enabled())
        return zmq_recv(socket_, buf_, len_, flags_);
    return bench_recv(socket_, buf_, len_, flags_, tag_);
}

inline std::string make_endpoint(const std::string& transport, const std::string& id) {
    if (transport == "inproc") return "inproc://" + id;
    if (transport == "ipc") return "ipc://*";
    if (transport == "tls") return "tls://127.0.0.1:*";
    if (transport == "ws") return "ws://127.0.0.1:*";
    return "tcp://127.0.0.1:*";
}

inline std::string bind_and_resolve_endpoint(void *socket_,
                                             const std::string& transport,
                                             const std::string& id) {
    std::string endpoint = make_endpoint(transport, id);
    if (zmq_bind(socket_, endpoint.c_str()) != 0) {
        std::cerr << "bind failed for " << endpoint << ": "
                  << zmq_strerror(zmq_errno()) << std::endl;
        return std::string();
    }
    if (transport != "inproc") {
        char last_endpoint[MAX_SOCKET_STRING] = "";
        size_t size = sizeof(last_endpoint);
        if (zmq_getsockopt(socket_, ZMQ_LAST_ENDPOINT, last_endpoint, &size) != 0) {
            std::cerr << "getsockopt(ZMQ_LAST_ENDPOINT) failed: "
                      << zmq_strerror(zmq_errno()) << std::endl;
            return std::string();
        }
        endpoint.assign(last_endpoint);
        if (transport == "tcp" || transport == "ws" || transport == "tls") {
            const std::string tcp_any = "://0.0.0.0:";
            const std::string tcp_ipv6_any = "://[::]:";
            size_t pos = endpoint.find(tcp_any);
            if (pos != std::string::npos) {
                endpoint.replace(pos, tcp_any.size(), "://127.0.0.1:");
            } else {
                pos = endpoint.find(tcp_ipv6_any);
                if (pos != std::string::npos) {
                    endpoint.replace(pos, tcp_ipv6_any.size(), "://127.0.0.1:");
                }
            }
        }
        if (bench_debug_enabled()) {
            std::cerr << "Resolved endpoint (" << transport << "): " << endpoint << std::endl;
        }
    }
    return endpoint;
}

inline bool transport_available(const std::string& transport) {
    if (transport == "ipc") return zmq_has("ipc") != 0;
    if (transport == "tls") {
#if defined(ZMQ_TLS_CERT)
        return zmq_has("tls") != 0;
#else
        return false;
#endif
    }
    return true;
}

inline void settle() {
    std::this_thread::sleep_for(std::chrono::milliseconds(SETTLE_TIME_MS));
}

inline bool connect_checked(void *socket_, const std::string& endpoint) {
    if (zmq_connect(socket_, endpoint.c_str()) != 0) {
        std::cerr << "connect failed for " << endpoint << ": "
                  << zmq_strerror(zmq_errno()) << std::endl;
        return false;
    }
    if (bench_debug_enabled()) {
        std::cerr << "Connected to " << endpoint << std::endl;
    }
    return true;
}

inline int resolve_msg_count(size_t size) {
    int count = (size <= 1024) ? 200000 : 20000;
    if (const char *env = std::getenv("BENCH_MSG_COUNT")) {
        errno = 0;
        const long override = std::strtol(env, NULL, 10);
        if (errno == 0 && override > 0)
            count = static_cast<int>(override);
    }
    return count;
}

inline int resolve_bench_count(const char *env_name, int default_value) {
    if (const char *env = std::getenv(env_name)) {
        errno = 0;
        const long override = std::strtol(env, NULL, 10);
        if (errno == 0 && override > 0)
            return static_cast<int>(override);
    }
    return default_value;
}

#endif
