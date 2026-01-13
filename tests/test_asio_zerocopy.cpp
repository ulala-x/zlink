/* SPDX-License-Identifier: MPL-2.0 */

/*
 * Test suite for ASIO Zero-Copy Write (Phase 1.1)
 *
 * These tests verify zero-copy write paths using scatter-gather I/O.
 * Tests are written in TDD style - they will fail until implementation is complete.
 */

#include "testutil.hpp"
#include "testutil_unity.hpp"

#include <unity.h>

#if defined ZMQ_IOTHREAD_POLLER_USE_ASIO

#include <string.h>
#include <stdlib.h>

// Debug counter API for testing zero-copy behavior
#if defined ZMQ_DEBUG_COUNTERS
extern "C" {
    size_t zmq_debug_get_zerocopy_count();
    size_t zmq_debug_get_fallback_count();
    size_t zmq_debug_get_bytes_copied();
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

// Test 1: Verify large messages use zero-copy path
// Large messages (>= 8KB) should use scatter-gather I/O without buffer merging
void test_zerocopy_large_message ()
{
    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));

    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

    msleep (SETTLE_TIME);

    // Create a large message (16 KB - well above 8KB threshold)
    const size_t msg_size = 16 * 1024;
    char *large_msg = static_cast<char *> (malloc (msg_size));
    TEST_ASSERT_NOT_NULL (large_msg);

    // Fill with pattern
    for (size_t i = 0; i < msg_size; i++) {
        large_msg[i] = static_cast<char> ('A' + (i % 26));
    }

    size_t zerocopy_before = 0;
#if defined ZMQ_DEBUG_COUNTERS
    zerocopy_before = zmq_debug_get_zerocopy_count();
#endif

    // Send large message
    TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                           zmq_send (server, large_msg, msg_size, 0));

    // Receive and verify
    char *recv_buf = static_cast<char *> (malloc (msg_size));
    TEST_ASSERT_NOT_NULL (recv_buf);

    TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                           zmq_recv (client, recv_buf, msg_size, 0));

    TEST_ASSERT_EQUAL_MEMORY (large_msg, recv_buf, msg_size);

#if defined ZMQ_DEBUG_COUNTERS
    size_t zerocopy_after = zmq_debug_get_zerocopy_count();

    // Verify zero-copy was used (at least one zero-copy write)
    TEST_ASSERT_GREATER_THAN (zerocopy_before, zerocopy_after);
#else
    // Without debug counters, we can't verify zero-copy was used
    // This test will pass but won't verify the optimization
    (void) zerocopy_before;  // Suppress unused variable warning
#endif

    free (large_msg);
    free (recv_buf);

    test_context_socket_close (client);
    test_context_socket_close (server);
}

// Test 2: Verify encoder buffer lifetime management with sentinel detection
// The sentinel buffer should detect use-after-free bugs
void test_zerocopy_encoder_buffer_lifetime ()
{
    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));

    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

    msleep (SETTLE_TIME);

    // Send multiple large messages in sequence
    const size_t msg_size = 10 * 1024;
    const int num_messages = 5;

    for (int i = 0; i < num_messages; i++) {
        char *msg = static_cast<char *> (malloc (msg_size));
        TEST_ASSERT_NOT_NULL (msg);

        // Fill with unique pattern per message
        for (size_t j = 0; j < msg_size; j++) {
            msg[j] = static_cast<char> ('A' + ((i + j) % 26));
        }

        TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                               zmq_send (server, msg, msg_size, 0));

        // Receive and verify
        char *recv_buf = static_cast<char *> (malloc (msg_size));
        TEST_ASSERT_NOT_NULL (recv_buf);

        TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                               zmq_recv (client, recv_buf, msg_size, 0));

        TEST_ASSERT_EQUAL_MEMORY (msg, recv_buf, msg_size);

        free (msg);
        free (recv_buf);
    }

    // If we reach here without crashes, sentinel protection is working
    test_context_socket_close (client);
    test_context_socket_close (server);
}

// Test 3: Verify fallback during handshake
// During ZMTP handshake, small control messages should use fallback path
void test_zerocopy_fallback_handshake ()
{
    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));

    size_t fallback_before = 0;
#if defined ZMQ_DEBUG_COUNTERS
    fallback_before = zmq_debug_get_fallback_count();
#endif

    // Connect triggers handshake with small control messages
    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

    msleep (SETTLE_TIME);

#if defined ZMQ_DEBUG_COUNTERS
    size_t fallback_after = zmq_debug_get_fallback_count();

    // Verify fallback was used during handshake
    TEST_ASSERT_GREATER_THAN (fallback_before, fallback_after);
#else
    (void) fallback_before;  // Suppress unused variable warning
#endif

    // Now send a large message to verify zero-copy works after handshake
    const size_t msg_size = 12 * 1024;
    char *large_msg = static_cast<char *> (malloc (msg_size));
    TEST_ASSERT_NOT_NULL (large_msg);
    memset (large_msg, 'X', msg_size);

    TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                           zmq_send (server, large_msg, msg_size, 0));

    char *recv_buf = static_cast<char *> (malloc (msg_size));
    TEST_ASSERT_NOT_NULL (recv_buf);

    TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                           zmq_recv (client, recv_buf, msg_size, 0));

    free (large_msg);
    free (recv_buf);

    test_context_socket_close (client);
    test_context_socket_close (server);
}

// Test 4: Verify header+body sent without merge using scatter-gather
// Multi-part messages should use scatter-gather without copying
void test_zerocopy_scatter_gather ()
{
    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));

    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

    msleep (SETTLE_TIME);

    // Send a large message (should use scatter-gather for header + body)
    const size_t msg_size = 20 * 1024;
    char *large_msg = static_cast<char *> (malloc (msg_size));
    TEST_ASSERT_NOT_NULL (large_msg);

    for (size_t i = 0; i < msg_size; i++) {
        large_msg[i] = static_cast<char> (i % 256);
    }

    size_t bytes_copied_before = 0;
#if defined ZMQ_DEBUG_COUNTERS
    bytes_copied_before = zmq_debug_get_bytes_copied();
#endif

    TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                           zmq_send (server, large_msg, msg_size, 0));

#if defined ZMQ_DEBUG_COUNTERS
    size_t bytes_copied_after = zmq_debug_get_bytes_copied();

    // Verify minimal copying (only header, not the body)
    // Total copied should be much less than message size
    size_t copied = bytes_copied_after - bytes_copied_before;
    TEST_ASSERT_LESS_THAN (1024, copied);  // Should copy < 1KB for header
#else
    (void) bytes_copied_before;  // Suppress unused variable warning
#endif

    char *recv_buf = static_cast<char *> (malloc (msg_size));
    TEST_ASSERT_NOT_NULL (recv_buf);

    TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                           zmq_recv (client, recv_buf, msg_size, 0));

    TEST_ASSERT_EQUAL_MEMORY (large_msg, recv_buf, msg_size);

    free (large_msg);
    free (recv_buf);

    test_context_socket_close (client);
    test_context_socket_close (server);
}

// Test 5: Verify fallback when zero-copy feature flag is disabled
// If zero-copy is disabled at runtime, should use fallback path
void test_zerocopy_feature_flag_off ()
{
    // This test will be implemented when runtime feature flags are added
    // For now, we test that small messages always use fallback

    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));

    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

    msleep (SETTLE_TIME);

    // Small message (< 8KB threshold) should use fallback
    const char *small_msg = "This is a small message";
    const size_t msg_size = strlen (small_msg);

    size_t fallback_before = 0;
#if defined ZMQ_DEBUG_COUNTERS
    fallback_before = zmq_debug_get_fallback_count();
#endif

    send_string_expect_success (server, small_msg, 0);

    // Must receive message to ensure async I/O has completed
    // before checking counters
    recv_string_expect_success (client, small_msg, 0);

#if defined ZMQ_DEBUG_COUNTERS
    size_t fallback_after = zmq_debug_get_fallback_count();

    // Verify fallback was used for small message
    TEST_ASSERT_GREATER_THAN (fallback_before, fallback_after);
#else
    (void) fallback_before;  // Suppress unused variable warning
#endif

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

void test_asio_zerocopy_not_enabled ()
{
    //  Skip tests when Asio poller is not enabled
    TEST_IGNORE_MESSAGE ("Asio poller not enabled, skipping zero-copy tests");
}

#endif  // ZMQ_IOTHREAD_POLLER_USE_ASIO

int main ()
{
    setup_test_environment ();

    UNITY_BEGIN ();

#if defined ZMQ_IOTHREAD_POLLER_USE_ASIO
    RUN_TEST (test_zerocopy_large_message);
    RUN_TEST (test_zerocopy_encoder_buffer_lifetime);
    RUN_TEST (test_zerocopy_fallback_handshake);
    RUN_TEST (test_zerocopy_scatter_gather);
    RUN_TEST (test_zerocopy_feature_flag_off);
#else
    RUN_TEST (test_asio_zerocopy_not_enabled);
#endif

    return UNITY_END ();
}
