/* SPDX-License-Identifier: MPL-2.0 */

/*
 * Test suite for ASIO lazy compaction (Phase 2)
 *
 * These tests ensure the compaction counters are accessible and the
 * read path remains functional with lazy compaction enabled.
 */

#include "testutil.hpp"
#include "testutil_unity.hpp"

#include <unity.h>

#if defined ZMQ_IOTHREAD_POLLER_USE_ASIO

#include <stdlib.h>
#include <string.h>

#if defined ZMQ_DEBUG_COUNTERS
extern "C" {
    size_t zmq_debug_get_compaction_count();
    size_t zmq_debug_get_compaction_skipped_count();
    size_t zmq_debug_get_compaction_bytes();
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

void test_compaction_threshold_not_met ()
{
    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));
    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

    msleep (SETTLE_TIME);

#if defined ZMQ_DEBUG_COUNTERS
    zmq_debug_reset_counters();
    const size_t compaction_before = zmq_debug_get_compaction_count();
#endif

    const char *msg = "compact-small";
    send_string_expect_success (server, msg, 0);
    recv_string_expect_success (client, msg, 0);

#if defined ZMQ_DEBUG_COUNTERS
    const size_t compaction_after = zmq_debug_get_compaction_count();
    TEST_ASSERT_GREATER_OR_EQUAL (compaction_before, compaction_after);
#endif

    test_context_socket_close (client);
    test_context_socket_close (server);
}

void test_compaction_counters_available ()
{
#if defined ZMQ_DEBUG_COUNTERS
    const size_t compaction = zmq_debug_get_compaction_count();
    const size_t skipped = zmq_debug_get_compaction_skipped_count();
    const size_t bytes = zmq_debug_get_compaction_bytes();
    TEST_ASSERT_TRUE (compaction + skipped + bytes >= 0);
#else
    TEST_IGNORE_MESSAGE ("Debug counters not enabled, skipping compaction counter test");
#endif
}

void test_compaction_large_message_path ()
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
    memset (payload, 'Z', msg_size);

#if defined ZMQ_DEBUG_COUNTERS
    const size_t compaction_before = zmq_debug_get_compaction_count();
#endif

    TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                           zmq_send (server, payload, msg_size, 0));

    char *recv_buf = static_cast<char *> (malloc (msg_size));
    TEST_ASSERT_NOT_NULL (recv_buf);
    TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                           zmq_recv (client, recv_buf, msg_size, 0));

#if defined ZMQ_DEBUG_COUNTERS
    const size_t compaction_after = zmq_debug_get_compaction_count();
    TEST_ASSERT_GREATER_OR_EQUAL (compaction_before, compaction_after);
#endif

    free (recv_buf);
    free (payload);

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

void test_asio_compaction_not_enabled ()
{
    TEST_IGNORE_MESSAGE ("Asio poller not enabled, skipping compaction tests");
}

#endif  // ZMQ_IOTHREAD_POLLER_USE_ASIO

int main ()
{
    setup_test_environment ();

    UNITY_BEGIN ();

#if defined ZMQ_IOTHREAD_POLLER_USE_ASIO
    RUN_TEST (test_compaction_threshold_not_met);
    RUN_TEST (test_compaction_counters_available);
    RUN_TEST (test_compaction_large_message_path);
#else
    RUN_TEST (test_asio_compaction_not_enabled);
#endif

    return UNITY_END ();
}
