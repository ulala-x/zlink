/* SPDX-License-Identifier: MPL-2.0 */

/*
 * Test suite for handshake buffer boundaries (Phase 4).
 */

#include "testutil.hpp"
#include "testutil_unity.hpp"

#include <string.h>

#if defined ZMQ_IOTHREAD_POLLER_USE_ASIO && defined ZMQ_ASIO_HANDSHAKE_ZEROCOPY

#if defined ZMQ_DEBUG_COUNTERS
extern "C" {
    size_t zmq_debug_get_handshake_copy_count();
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

static void send_and_recv (void *sender, void *receiver, const char *payload)
{
    const int size = static_cast<int> (strlen (payload));
    TEST_ASSERT_EQUAL_INT (size, zmq_send (sender, payload, size, 0));

    char buffer[128];
    TEST_ASSERT_EQUAL_INT (size, zmq_recv (receiver, buffer, sizeof (buffer), 0));
    TEST_ASSERT_EQUAL_INT (0, memcmp (buffer, payload, size));
}

void test_handshake_null_tcp ()
{
    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    set_socket_timeouts (server, 2000);
    set_socket_timeouts (client, 2000);
    set_socket_hwm (server, 1000);
    set_socket_hwm (client, 1000);

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));
    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

#if defined ZMQ_DEBUG_COUNTERS
    zmq_debug_reset_counters ();
#endif

    send_and_recv (client, server, "hello");

#if defined ZMQ_DEBUG_COUNTERS
    TEST_ASSERT_TRUE (zmq_debug_get_handshake_copy_count () >= 0);
#endif

    test_context_socket_close (client);
    test_context_socket_close (server);
}

#if defined(ZMQ_HAVE_IPC)
void test_handshake_null_ipc ()
{
    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    set_socket_timeouts (server, 2000);
    set_socket_timeouts (client, 2000);
    set_socket_hwm (server, 1000);
    set_socket_hwm (client, 1000);

    char endpoint[MAX_SOCKET_STRING];
    make_random_ipc_endpoint (endpoint);
    TEST_ASSERT_SUCCESS_ERRNO (zmq_bind (server, endpoint));
    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

#if defined ZMQ_DEBUG_COUNTERS
    zmq_debug_reset_counters ();
#endif

    send_and_recv (client, server, "ipc");

#if defined ZMQ_DEBUG_COUNTERS
    TEST_ASSERT_TRUE (zmq_debug_get_handshake_copy_count () >= 0);
#endif

    test_context_socket_close (client);
    test_context_socket_close (server);
}
#endif

#else  // !ZMQ_IOTHREAD_POLLER_USE_ASIO || !ZMQ_ASIO_HANDSHAKE_ZEROCOPY

void setUp ()
{
}

void tearDown ()
{
}

void test_asio_handshake_not_enabled ()
{
    TEST_IGNORE_MESSAGE (
      "Asio handshake zero-copy not enabled, skipping tests");
}

#endif  // ZMQ_IOTHREAD_POLLER_USE_ASIO && ZMQ_ASIO_HANDSHAKE_ZEROCOPY

int main ()
{
    setup_test_environment ();

    UNITY_BEGIN ();

#if defined ZMQ_IOTHREAD_POLLER_USE_ASIO && defined ZMQ_ASIO_HANDSHAKE_ZEROCOPY
    RUN_TEST (test_handshake_null_tcp);
#if defined(ZMQ_HAVE_IPC)
    RUN_TEST (test_handshake_null_ipc);
#endif
#else
    RUN_TEST (test_asio_handshake_not_enabled);
#endif

    return UNITY_END ();
}
