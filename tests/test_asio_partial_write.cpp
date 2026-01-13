/* SPDX-License-Identifier: MPL-2.0 */

/*
 * Test suite for ASIO Partial Write Handling (Phase 1.1)
 *
 * These tests verify correct handling of partial writes in async I/O.
 * Tests are written in TDD style - they will fail until implementation is complete.
 */

#include "testutil.hpp"
#include "testutil_unity.hpp"

#include <unity.h>

#if defined ZMQ_IOTHREAD_POLLER_USE_ASIO

#include <string.h>
#include <stdlib.h>

#ifdef ZMQ_HAVE_WINDOWS
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

void setUp ()
{
    setup_test_context ();
}

void tearDown ()
{
    teardown_test_context ();
}

// Test 1: Force partial writes with small SO_SNDBUF
// Verify that messages complete even when SO_SNDBUF forces partial writes
void test_partial_write_small_buffer ()
{
    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    // Set small send buffer to force partial writes
    // Note: This is OS-dependent - some systems may round up the buffer size
    int sndbuf = 4096;  // 4KB send buffer
    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_setsockopt (server, ZMQ_SNDBUF, &sndbuf, sizeof (int)));

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));

    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

    msleep (SETTLE_TIME);

    // Send message larger than send buffer (should trigger partial writes)
    const size_t msg_size = 32 * 1024;  // 32KB message
    char *large_msg = static_cast<char *> (malloc (msg_size));
    TEST_ASSERT_NOT_NULL (large_msg);

    // Fill with pattern
    for (size_t i = 0; i < msg_size; i++) {
        large_msg[i] = static_cast<char> ('A' + (i % 26));
    }

    // Send should succeed even with small buffer
    TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                           zmq_send (server, large_msg, msg_size, 0));

    // Receive and verify complete message
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

// Test 2: Verify write offset correctly advances during partial writes
// Multiple partial writes should correctly track offset
void test_partial_write_offset_advance ()
{
    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    // Set small send buffer
    int sndbuf = 8192;  // 8KB
    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_setsockopt (server, ZMQ_SNDBUF, &sndbuf, sizeof (int)));

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));

    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

    msleep (SETTLE_TIME);

    // Send multiple messages in quick succession
    // This increases chance of partial writes
    const size_t msg_size = 16 * 1024;  // 16KB
    const int num_messages = 10;

    for (int i = 0; i < num_messages; i++) {
        char *msg = static_cast<char *> (malloc (msg_size));
        TEST_ASSERT_NOT_NULL (msg);

        // Fill with unique pattern per message
        for (size_t j = 0; j < msg_size; j++) {
            msg[j] = static_cast<char> (i * 256 + (j % 256));
        }

        TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                               zmq_send (server, msg, msg_size, 0));

        free (msg);
    }

    // Receive all messages and verify correctness
    for (int i = 0; i < num_messages; i++) {
        char *recv_buf = static_cast<char *> (malloc (msg_size));
        TEST_ASSERT_NOT_NULL (recv_buf);

        TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                               zmq_recv (client, recv_buf, msg_size, 0));

        // Verify pattern
        for (size_t j = 0; j < msg_size; j++) {
            char expected = static_cast<char> (i * 256 + (j % 256));
            TEST_ASSERT_EQUAL_INT (expected, recv_buf[j]);
        }

        free (recv_buf);
    }

    test_context_socket_close (client);
    test_context_socket_close (server);
}

// Test 3: Verify completion only after all bytes sent
// Write completion callback should only fire after entire message is sent
void test_partial_write_completion ()
{
    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    // Set small send buffer
    int sndbuf = 4096;  // 4KB
    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_setsockopt (server, ZMQ_SNDBUF, &sndbuf, sizeof (int)));

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));

    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

    msleep (SETTLE_TIME);

    // Send a large message
    const size_t msg_size = 64 * 1024;  // 64KB
    char *large_msg = static_cast<char *> (malloc (msg_size));
    TEST_ASSERT_NOT_NULL (large_msg);
    memset (large_msg, 'Z', msg_size);

    // Non-blocking send
    TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                           zmq_send (server, large_msg, msg_size, ZMQ_DONTWAIT));

    // Give time for async write to complete
    msleep (100);

    // Receive complete message
    char *recv_buf = static_cast<char *> (malloc (msg_size));
    TEST_ASSERT_NOT_NULL (recv_buf);

    TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                           zmq_recv (client, recv_buf, msg_size, 0));

    // Verify all bytes received correctly
    for (size_t i = 0; i < msg_size; i++) {
        TEST_ASSERT_EQUAL_INT ('Z', recv_buf[i]);
    }

    free (large_msg);
    free (recv_buf);

    test_context_socket_close (client);
    test_context_socket_close (server);
}

// Test 4: Large message split into multiple write chunks
// Very large messages should handle multiple partial write cycles
void test_partial_write_multiple_chunks ()
{
    void *server = test_context_socket (ZMQ_DEALER);
    void *client = test_context_socket (ZMQ_DEALER);

    // Set very small send buffer to force many partial writes
    int sndbuf = 2048;  // 2KB
    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_setsockopt (server, ZMQ_SNDBUF, &sndbuf, sizeof (int)));

    char endpoint[MAX_SOCKET_STRING];
    bind_loopback_ipv4 (server, endpoint, sizeof (endpoint));

    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (client, endpoint));

    msleep (SETTLE_TIME);

    // Send very large message (should require many partial write cycles)
    const size_t msg_size = 256 * 1024;  // 256KB
    char *huge_msg = static_cast<char *> (malloc (msg_size));
    TEST_ASSERT_NOT_NULL (huge_msg);

    // Fill with verifiable pattern
    for (size_t i = 0; i < msg_size; i++) {
        huge_msg[i] = static_cast<char> (i & 0xFF);
    }

    TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                           zmq_send (server, huge_msg, msg_size, 0));

    // Receive and verify
    char *recv_buf = static_cast<char *> (malloc (msg_size));
    TEST_ASSERT_NOT_NULL (recv_buf);

    TEST_ASSERT_EQUAL_INT (static_cast<int> (msg_size),
                           zmq_recv (client, recv_buf, msg_size, 0));

    // Verify entire message
    TEST_ASSERT_EQUAL_MEMORY (huge_msg, recv_buf, msg_size);

    free (huge_msg);
    free (recv_buf);

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

void test_asio_partial_write_not_enabled ()
{
    //  Skip tests when Asio poller is not enabled
    TEST_IGNORE_MESSAGE ("Asio poller not enabled, skipping partial write tests");
}

#endif  // ZMQ_IOTHREAD_POLLER_USE_ASIO

int main ()
{
    setup_test_environment ();

    UNITY_BEGIN ();

#if defined ZMQ_IOTHREAD_POLLER_USE_ASIO
    RUN_TEST (test_partial_write_small_buffer);
    RUN_TEST (test_partial_write_offset_advance);
    RUN_TEST (test_partial_write_completion);
    RUN_TEST (test_partial_write_multiple_chunks);
#else
    RUN_TEST (test_asio_partial_write_not_enabled);
#endif

    return UNITY_END ();
}
