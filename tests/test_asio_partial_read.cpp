/* SPDX-License-Identifier: MPL-2.0 */

/*
 * Test suite for ASIO Partial Read Handling (Phase 2)
 *
 * These tests exercise large reads and fragmented delivery to ensure
 * the decoder path remains correct when reads split messages.
 */

#include "testutil.hpp"
#include "testutil_unity.hpp"

#include <unity.h>

#if defined ZMQ_IOTHREAD_POLLER_USE_ASIO

#include <stdlib.h>
#include <string.h>

void setUp ()
{
    setup_test_context ();
}

void tearDown ()
{
    teardown_test_context ();
}

static void fill_pattern (char *buf, size_t size, char seed)
{
    for (size_t i = 0; i < size; ++i)
        buf[i] = static_cast<char> (seed + (i % 23));
}

void test_partial_read_fragment ()
{
    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));
    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

    msleep (SETTLE_TIME);

    const size_t msg_size = 128 * 1024;
    char *payload = static_cast<char *> (malloc (msg_size));
    TEST_ASSERT_NOT_NULL (payload);
    fill_pattern (payload, msg_size, 'A');

    TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                           zmq_send (server, payload, msg_size, 0));

    char *recv_buf = static_cast<char *> (malloc (msg_size));
    TEST_ASSERT_NOT_NULL (recv_buf);

    TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                           zmq_recv (client, recv_buf, msg_size, 0));
    TEST_ASSERT_EQUAL_MEMORY (payload, recv_buf, msg_size);

    free (recv_buf);
    free (payload);

    test_context_socket_close (client);
    test_context_socket_close (server);
}

void test_partial_read_large_message ()
{
    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));
    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

    msleep (SETTLE_TIME);

    const size_t msg_size = 256 * 1024;
    char *payload = static_cast<char *> (malloc (msg_size));
    TEST_ASSERT_NOT_NULL (payload);
    fill_pattern (payload, msg_size, 'K');

    TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                           zmq_send (server, payload, msg_size, 0));

    char *recv_buf = static_cast<char *> (malloc (msg_size));
    TEST_ASSERT_NOT_NULL (recv_buf);

    TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                           zmq_recv (client, recv_buf, msg_size, 0));
    TEST_ASSERT_EQUAL_MEMORY (payload, recv_buf, msg_size);

    free (recv_buf);
    free (payload);

    test_context_socket_close (client);
    test_context_socket_close (server);
}

void test_partial_read_multiple_messages ()
{
    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));
    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

    msleep (SETTLE_TIME);

    const size_t msg_size = 4096;
    const int msg_count = 32;

    for (int i = 0; i < msg_count; ++i) {
        char *payload = static_cast<char *> (malloc (msg_size));
        TEST_ASSERT_NOT_NULL (payload);
        fill_pattern (payload, msg_size, static_cast<char> ('a' + (i % 7)));
        TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                               zmq_send (server, payload, msg_size, 0));
        free (payload);
    }

    for (int i = 0; i < msg_count; ++i) {
        char *recv_buf = static_cast<char *> (malloc (msg_size));
        TEST_ASSERT_NOT_NULL (recv_buf);
        TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                               zmq_recv (client, recv_buf, msg_size, 0));
        free (recv_buf);
    }

    test_context_socket_close (client);
    test_context_socket_close (server);
}

#else  // !ZMQ_IOTHREAD_POLLER_USE_ASIO

void setUp ()
{
}

void tearDown ()
{
}

void test_asio_partial_read_not_enabled ()
{
    TEST_IGNORE_MESSAGE ("Asio poller not enabled, skipping partial read tests");
}

#endif  // ZMQ_IOTHREAD_POLLER_USE_ASIO

int main ()
{
    setup_test_environment ();

    UNITY_BEGIN ();

#if defined ZMQ_IOTHREAD_POLLER_USE_ASIO
    RUN_TEST (test_partial_read_fragment);
    RUN_TEST (test_partial_read_large_message);
    RUN_TEST (test_partial_read_multiple_messages);
#else
    RUN_TEST (test_asio_partial_read_not_enabled);
#endif

    return UNITY_END ();
}
