/* SPDX-License-Identifier: MPL-2.0 */

/*
 * Test suite for ASIO Error Recovery (Phase 1.1)
 *
 * These tests verify proper cleanup and error handling in async I/O paths.
 * Tests are written in TDD style - they will fail until implementation is complete.
 */

#include "testutil.hpp"
#include "testutil_unity.hpp"

#include <unity.h>

#if defined ZMQ_IOTHREAD_POLLER_USE_ASIO

#include <string.h>
#include <stdlib.h>

void setUp ()
{
    setup_test_context ();
}

void tearDown ()
{
    teardown_test_context ();
}

// Test 1: Verify cleanup on connection reset (ECONNRESET)
// When peer resets connection, all buffers should be properly released
void test_error_econnreset ()
{
    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));

    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

    msleep (SETTLE_TIME);

    // Send a message to establish connection
    const char *msg = "Test message";
    send_string_expect_success (server, msg, 0);
    recv_string_expect_success (client, msg, 0);

    // Close client abruptly with zero linger (simulates connection reset)
    int linger = 0;
    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_setsockopt (client, ZMQ_LINGER, &linger, sizeof (int)));
    test_context_socket_close (client);

    // Try to send from server (should handle error gracefully)
    // The send might succeed (queued) or fail depending on timing
    // Either way, cleanup should be proper
    const size_t large_size = 64 * 1024;
    char *large_msg = static_cast<char *> (malloc (large_size));
    TEST_ASSERT_NOT_NULL (large_msg);
    memset (large_msg, 'A', large_size);

    // Send may fail or succeed, we just care that cleanup happens
    zmq_send (server, large_msg, large_size, ZMQ_DONTWAIT);

    free (large_msg);

    // Give time for error to propagate
    msleep (100);

    // Close server should not crash or leak
    test_context_socket_close (server);
}

// Test 2: Verify cleanup on broken pipe (EPIPE)
// Similar to ECONNRESET but for EPIPE errors
void test_error_epipe ()
{
    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));

    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

    msleep (SETTLE_TIME);

    // Close client immediately
    int linger = 0;
    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_setsockopt (client, ZMQ_LINGER, &linger, sizeof (int)));
    test_context_socket_close (client);

    msleep (50);

    // Send multiple messages from server
    // Some may succeed (in buffer), others may fail
    for (int i = 0; i < 10; i++) {
        const size_t msg_size = 8 * 1024;
        char *msg = static_cast<char *> (malloc (msg_size));
        TEST_ASSERT_NOT_NULL (msg);
        memset (msg, 'B', msg_size);

        // Non-blocking send - may succeed or fail
        zmq_send (server, msg, msg_size, ZMQ_DONTWAIT);

        free (msg);
    }

    // Give time for errors to be detected
    msleep (100);

    // Cleanup should be proper
    test_context_socket_close (server);
}

// Test 3: Verify state restoration on operation cancellation
// When async operations are cancelled, state should be properly restored
void test_error_cancellation ()
{
    void *server = test_context_socket (ZMQ_DEALER);
    void *client1 = test_context_socket (ZMQ_DEALER);

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));

    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client1, endpoint));

    msleep (SETTLE_TIME);

    // Send a large message
    const size_t msg_size = 32 * 1024;
    char *msg = static_cast<char *> (malloc (msg_size));
    TEST_ASSERT_NOT_NULL (msg);
    memset (msg, 'C', msg_size);

    TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                           zmq_send (server, msg, msg_size, ZMQ_DONTWAIT));

    // Close client1 while data might be in flight
    int linger = 0;
    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_setsockopt (client1, ZMQ_LINGER, &linger, sizeof (int)));
    test_context_socket_close (client1);

    // Create new client and verify server still works
    void *client2 = test_context_socket (ZMQ_DEALER);
    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client2, endpoint));

    msleep (SETTLE_TIME);

    // Server should be able to communicate with new client
    const char *test_msg = "Recovery test";
    send_string_expect_success (server, test_msg, 0);
    recv_string_expect_success (client2, test_msg, 0);

    free (msg);

    test_context_socket_close (client2);
    test_context_socket_close (server);
}

// Test 4: ASAN verification - no memory leaks during error conditions
// When running under ASAN, this verifies no leaks occur during errors
void test_error_no_leak ()
{
    // This test relies on ASAN being enabled during build
    // It creates error conditions and verifies no leaks via ASAN

    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));

    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

    msleep (SETTLE_TIME);

    // Send large messages
    const size_t msg_size = 128 * 1024;
    for (int i = 0; i < 5; i++) {
        char *msg = static_cast<char *> (malloc (msg_size));
        TEST_ASSERT_NOT_NULL (msg);

        for (size_t j = 0; j < msg_size; j++) {
            msg[j] = static_cast<char> ((i + j) % 256);
        }

        TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                               zmq_send (server, msg, msg_size, ZMQ_DONTWAIT));
        free (msg);
    }

    // Abrupt close
    int linger = 0;
    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_setsockopt (server, ZMQ_LINGER, &linger, sizeof (int)));
    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_setsockopt (client, ZMQ_LINGER, &linger, sizeof (int)));

    test_context_socket_close (client);
    test_context_socket_close (server);

    // If running under ASAN, any leaks will be reported at process exit
    // Run with: cmake -DCMAKE_CXX_FLAGS="-fsanitize=address" to enable ASAN
}

// Test 5: Verify no double-free of buffers during error paths
// Sentinel buffers should prevent double-free bugs
void test_error_no_double_free ()
{
    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));

    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

    msleep (SETTLE_TIME);

    // Send multiple large messages rapidly
    const size_t msg_size = 64 * 1024;
    const int num_messages = 20;

    for (int i = 0; i < num_messages; i++) {
        char *msg = static_cast<char *> (malloc (msg_size));
        TEST_ASSERT_NOT_NULL (msg);
        memset (msg, 'D', msg_size);

        zmq_send (server, msg, msg_size, ZMQ_DONTWAIT);
        free (msg);

        // Randomly close and recreate client to create error conditions
        if (i == 10) {
            int linger = 0;
            TEST_ASSERT_SUCCESS_ERRNO (
              zmq_setsockopt (client, ZMQ_LINGER, &linger, sizeof (int)));
            test_context_socket_close (client);

            client = test_context_socket (ZMQ_DEALER);
            TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));
            msleep (SETTLE_TIME);
        }
    }

    msleep (100);

    // Cleanup - if double-free occurs, this will likely crash
    test_context_socket_close (client);
    test_context_socket_close (server);

    // If we reach here without crash, no double-free occurred
    // Run with: cmake -DCMAKE_CXX_FLAGS="-fsanitize=address" to enable ASAN
}

#else  // !ZMQ_IOTHREAD_POLLER_USE_ASIO

void setUp ()
{
}

void tearDown ()
{
}

void test_asio_error_recovery_not_enabled ()
{
    //  Skip tests when Asio poller is not enabled
    TEST_IGNORE_MESSAGE ("Asio poller not enabled, skipping error recovery tests");
}

#endif  // ZMQ_IOTHREAD_POLLER_USE_ASIO

int main ()
{
    setup_test_environment ();

    UNITY_BEGIN ();

#if defined ZMQ_IOTHREAD_POLLER_USE_ASIO
    RUN_TEST (test_error_econnreset);
    RUN_TEST (test_error_epipe);
    RUN_TEST (test_error_cancellation);
    RUN_TEST (test_error_no_leak);
    RUN_TEST (test_error_no_double_free);
#else
    RUN_TEST (test_asio_error_recovery_not_enabled);
#endif

    return UNITY_END ();
}
