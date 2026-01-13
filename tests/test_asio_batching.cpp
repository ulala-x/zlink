/* SPDX-License-Identifier: MPL-2.0 */

/*
 * Test suite for ASIO write batching (Phase 3)
 *
 * These tests validate batching behavior using debug counters.
 */

#include "testutil.hpp"
#include "testutil_unity.hpp"

#include <unity.h>

#if defined ZMQ_IOTHREAD_POLLER_USE_ASIO && defined ZMQ_ASIO_WRITE_BATCHING

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <string>

#if defined ZMQ_DEBUG_COUNTERS
extern "C" {
    size_t zmq_debug_get_batch_flush_count();
    size_t zmq_debug_get_batch_timeout_flush_count();
    size_t zmq_debug_get_batch_size_flush_count();
    size_t zmq_debug_get_batch_count_flush_count();
    size_t zmq_debug_get_batch_messages_total();
    size_t zmq_debug_get_batch_bytes_total();
    size_t zmq_debug_get_zerocopy_count();
    void zmq_debug_reset_counters();
}
#endif

void setUp ()
{
    setup_test_context ();
}

void tearDown ()
{
    teardown_test_context ();
}

static void set_socket_timeouts (void *socket, int timeout_ms)
{
    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_setsockopt (socket, ZMQ_SNDTIMEO, &timeout_ms, sizeof (int)));
    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_setsockopt (socket, ZMQ_RCVTIMEO, &timeout_ms, sizeof (int)));
}

static void set_socket_hwm (void *socket, int hwm)
{
    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_setsockopt (socket, ZMQ_SNDHWM, &hwm, sizeof (int)));
    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_setsockopt (socket, ZMQ_RCVHWM, &hwm, sizeof (int)));
}

static void set_env_value (const char *name, const char *value)
{
#if defined ZMQ_HAVE_WINDOWS
    _putenv_s (name, value);
#else
    setenv (name, value, 1);
#endif
}

static void unset_env_value (const char *name)
{
#if defined ZMQ_HAVE_WINDOWS
    _putenv_s (name, "");
#else
    unsetenv (name);
#endif
}

static std::string snapshot_env_value (const char *name)
{
    const char *value = getenv (name);
    if (!value)
        return std::string ();
    return std::string (value);
}

static void restore_env_value (const char *name,
                               const std::string &value)
{
    if (value.empty ())
        unset_env_value (name);
    else
        set_env_value (name, value.c_str ());
}

static void send_messages (void *socket,
                           size_t msg_size,
                           int count,
                           char fill)
{
    char *buffer = static_cast<char *> (malloc (msg_size));
    TEST_ASSERT_NOT_NULL (buffer);
    memset (buffer, fill, msg_size);

    for (int i = 0; i < count; ++i) {
        TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                               zmq_send (socket, buffer, msg_size, 0));
    }

    free (buffer);
}

static void recv_messages (void *socket,
                           size_t msg_size,
                           int count,
                           char fill)
{
    char *buffer = static_cast<char *> (malloc (msg_size));
    TEST_ASSERT_NOT_NULL (buffer);

    for (int i = 0; i < count; ++i) {
        TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                               zmq_recv (socket, buffer, msg_size, 0));
        TEST_ASSERT_EQUAL_INT (fill, buffer[0]);
        TEST_ASSERT_EQUAL_INT (fill, buffer[msg_size - 1]);
    }

    free (buffer);
}

void test_batching_timeout_flush ()
{
#if !defined ZMQ_DEBUG_COUNTERS
    TEST_IGNORE_MESSAGE ("Debug counters not enabled, skipping batching test");
    return;
#endif

    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    set_socket_timeouts (server, 2000);
    set_socket_timeouts (client, 2000);
    set_socket_hwm (server, 1000);
    set_socket_hwm (client, 1000);

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));
    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

    msleep (SETTLE_TIME);

    zmq_debug_reset_counters ();
    const size_t before = zmq_debug_get_batch_timeout_flush_count ();

    send_messages (server, 64, 8, 'A');

    msleep (5);

    recv_messages (client, 64, 8, 'A');

    const size_t after = zmq_debug_get_batch_timeout_flush_count ();
    TEST_ASSERT_TRUE (after > before);

    TEST_ASSERT_TRUE (zmq_debug_get_batch_flush_count () > 0);
    TEST_ASSERT_TRUE (zmq_debug_get_batch_messages_total () >= 8);

    test_context_socket_close (client);
    test_context_socket_close (server);
}

void test_batching_size_limit ()
{
#if !defined ZMQ_DEBUG_COUNTERS
    TEST_IGNORE_MESSAGE ("Debug counters not enabled, skipping batching test");
    return;
#endif

    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    set_socket_timeouts (server, 2000);
    set_socket_timeouts (client, 2000);
    set_socket_hwm (server, 1000);
    set_socket_hwm (client, 1000);

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));
    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

    msleep (SETTLE_TIME);

    zmq_debug_reset_counters ();
    const size_t before = zmq_debug_get_batch_size_flush_count ();

    send_messages (server, 1024, 10, 'B');

    msleep (5);

    recv_messages (client, 1024, 10, 'B');

    const size_t after = zmq_debug_get_batch_size_flush_count ();
    TEST_ASSERT_TRUE (after > before);

    test_context_socket_close (client);
    test_context_socket_close (server);
}

void test_batching_count_limit ()
{
#if !defined ZMQ_DEBUG_COUNTERS
    TEST_IGNORE_MESSAGE ("Debug counters not enabled, skipping batching test");
    return;
#endif

    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    set_socket_timeouts (server, 2000);
    set_socket_timeouts (client, 2000);
    set_socket_hwm (server, 2000);
    set_socket_hwm (client, 2000);

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));
    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

    msleep (SETTLE_TIME);

    zmq_debug_reset_counters ();
    const size_t before = zmq_debug_get_batch_count_flush_count ();

    send_messages (server, 64, 120, 'C');

    msleep (5);

    recv_messages (client, 64, 120, 'C');

    const size_t after = zmq_debug_get_batch_count_flush_count ();
    TEST_ASSERT_TRUE (after > before);

    test_context_socket_close (client);
    test_context_socket_close (server);
}

void test_batching_large_message_priority ()
{
#if !defined ZMQ_DEBUG_COUNTERS
    TEST_IGNORE_MESSAGE ("Debug counters not enabled, skipping batching test");
    return;
#endif

    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    set_socket_timeouts (server, 2000);
    set_socket_timeouts (client, 2000);
    set_socket_hwm (server, 1000);
    set_socket_hwm (client, 1000);

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));
    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

    msleep (SETTLE_TIME);

    zmq_debug_reset_counters ();

    send_messages (server, 64, 5, 'D');
    send_messages (server, 128 * 1024, 1, 'L');

    recv_messages (client, 64, 5, 'D');
    recv_messages (client, 128 * 1024, 1, 'L');

    TEST_ASSERT_TRUE (zmq_debug_get_batch_flush_count () > 0);
#if defined ZMQ_ASIO_ZEROCOPY_WRITE
    TEST_ASSERT_TRUE (zmq_debug_get_zerocopy_count () > 0);
#endif

    test_context_socket_close (client);
    test_context_socket_close (server);
}

void test_batching_timer_cancellation ()
{
#if !defined ZMQ_DEBUG_COUNTERS
    TEST_IGNORE_MESSAGE ("Debug counters not enabled, skipping batching test");
    return;
#endif

    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    set_socket_timeouts (server, 2000);
    set_socket_timeouts (client, 2000);
    set_socket_hwm (server, 1000);
    set_socket_hwm (client, 1000);

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));
    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

    msleep (SETTLE_TIME);

    zmq_debug_reset_counters ();
    const size_t timeout_before = zmq_debug_get_batch_timeout_flush_count ();
    const size_t size_before = zmq_debug_get_batch_size_flush_count ();

    send_messages (server, 4096, 6, 'E');
    recv_messages (client, 4096, 6, 'E');

    msleep (5);

    const size_t timeout_after = zmq_debug_get_batch_timeout_flush_count ();
    const size_t size_after = zmq_debug_get_batch_size_flush_count ();
    TEST_ASSERT_TRUE (timeout_after > timeout_before);
    TEST_ASSERT_TRUE (size_after > size_before);

    test_context_socket_close (client);
    test_context_socket_close (server);
}

void test_batching_strand_safety ()
{
#if !defined ZMQ_DEBUG_COUNTERS
    TEST_IGNORE_MESSAGE ("Debug counters not enabled, skipping batching test");
    return;
#endif

    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    set_socket_timeouts (server, 2000);
    set_socket_timeouts (client, 2000);
    set_socket_hwm (server, 2000);
    set_socket_hwm (client, 2000);

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));
    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

    msleep (SETTLE_TIME);

    zmq_debug_reset_counters ();

    send_messages (server, 128, 200, 'F');
    recv_messages (client, 128, 200, 'F');

    TEST_ASSERT_TRUE (zmq_debug_get_batch_flush_count () > 0);
    TEST_ASSERT_TRUE (zmq_debug_get_batch_messages_total () >= 200);

    test_context_socket_close (client);
    test_context_socket_close (server);
}

void test_batching_feature_flag_off ()
{
#if !defined ZMQ_DEBUG_COUNTERS
    TEST_IGNORE_MESSAGE ("Debug counters not enabled, skipping batching test");
    return;
#endif

    const std::string saved =
      snapshot_env_value ("ZMQ_ASIO_WRITE_BATCHING");
    teardown_test_context ();
    set_env_value ("ZMQ_ASIO_WRITE_BATCHING", "0");
    setup_test_context ();

    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    set_socket_timeouts (server, 2000);
    set_socket_timeouts (client, 2000);
    set_socket_hwm (server, 1000);
    set_socket_hwm (client, 1000);

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));
    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

    msleep (SETTLE_TIME);

    zmq_debug_reset_counters ();
    send_messages (server, 64, 20, 'G');
    recv_messages (client, 64, 20, 'G');

    TEST_ASSERT_EQUAL_UINT64 (
      0, static_cast<uint64_t> (zmq_debug_get_batch_flush_count ()));

    test_context_socket_close (client);
    test_context_socket_close (server);
    teardown_test_context ();
    restore_env_value ("ZMQ_ASIO_WRITE_BATCHING", saved);
}

#else  // !ZMQ_IOTHREAD_POLLER_USE_ASIO || !ZMQ_ASIO_WRITE_BATCHING

void setUp ()
{
}

void tearDown ()
{
}

void test_asio_batching_not_enabled ()
{
    TEST_IGNORE_MESSAGE (
      "Asio batching not enabled, skipping batching tests");
}

#endif  // ZMQ_IOTHREAD_POLLER_USE_ASIO && ZMQ_ASIO_WRITE_BATCHING

int main ()
{
    setup_test_environment ();

    UNITY_BEGIN ();

#if defined ZMQ_IOTHREAD_POLLER_USE_ASIO && defined ZMQ_ASIO_WRITE_BATCHING
    RUN_TEST (test_batching_timeout_flush);
    RUN_TEST (test_batching_size_limit);
    RUN_TEST (test_batching_count_limit);
    RUN_TEST (test_batching_large_message_priority);
    RUN_TEST (test_batching_timer_cancellation);
    RUN_TEST (test_batching_strand_safety);
    RUN_TEST (test_batching_feature_flag_off);
#else
    RUN_TEST (test_asio_batching_not_enabled);
#endif

    return UNITY_END ();
}
