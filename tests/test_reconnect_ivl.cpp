/* SPDX-License-Identifier: MPL-2.0 */

#include "testutil.hpp"
#include "testutil_unity.hpp"

void setUp ()
{
    setup_test_context ();
    int blocky = 0;
    zmq_ctx_set (get_test_context (), ZMQ_BLOCKY, blocky);
}

void tearDown ()
{
    teardown_test_context ();
}

void *test_reconnect_ivl_against_pair_socket (const char *my_endpoint_,
                                              void *sb_)
{
    void *sc = test_context_socket (ZMQ_PAIR);
    int interval = -1;
    TEST_ASSERT_SUCCESS_ERRNO (
      zmq_setsockopt (sc, ZMQ_RECONNECT_IVL, &interval, sizeof (int)));
    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (sc, my_endpoint_));

    bounce (sb_, sc);

    TEST_ASSERT_SUCCESS_ERRNO (zmq_unbind (sb_, my_endpoint_));

    expect_bounce_fail (sb_, sc);

    TEST_ASSERT_SUCCESS_ERRNO (zmq_bind (sb_, my_endpoint_));

    expect_bounce_fail (sb_, sc);

    {
        int rc = zmq_disconnect (sc, my_endpoint_);
        if (rc != 0)
            TEST_ASSERT_EQUAL_INT (ENOENT, errno);
    }

    TEST_ASSERT_SUCCESS_ERRNO (zmq_connect (sc, my_endpoint_));

    bounce (sb_, sc);

    return sc;
}

#if defined(ZMQ_HAVE_IPC) && !defined(ZMQ_HAVE_GNU)
void test_reconnect_ivl_ipc (void)
{
    char my_endpoint[256];
    make_random_ipc_endpoint (my_endpoint);

    void *sb = test_context_socket (ZMQ_PAIR);
    TEST_ASSERT_SUCCESS_ERRNO (zmq_bind (sb, my_endpoint));

    void *sc = test_reconnect_ivl_against_pair_socket (my_endpoint, sb);
    test_context_socket_close_zero_linger (sb);
    test_context_socket_close_zero_linger (sc);
}
#endif

void test_reconnect_ivl_tcp (bind_function_t bind_function_)
{
    char my_endpoint[MAX_SOCKET_STRING];

    void *sb = test_context_socket (ZMQ_PAIR);
    bind_function_ (sb, my_endpoint, sizeof my_endpoint);

    void *sc = test_reconnect_ivl_against_pair_socket (my_endpoint, sb);
    {
        int rc = zmq_unbind (sb, my_endpoint);
        if (rc != 0)
            TEST_ASSERT_EQUAL_INT (ENOENT, errno);
    }
    test_context_socket_close_zero_linger (sb);
    test_context_socket_close_zero_linger (sc);
}

void test_reconnect_ivl_tcp_ipv4 ()
{
    test_reconnect_ivl_tcp (bind_loopback_ipv4);
}

void test_reconnect_ivl_tcp_ipv6 ()
{
    if (is_ipv6_available ()) {
        zmq_ctx_set (get_test_context (), ZMQ_IPV6, 1);
        test_reconnect_ivl_tcp (bind_loopback_ipv6);
    }
}

int main (void)
{
    setup_test_environment ();

    UNITY_BEGIN ();
#if defined(ZMQ_HAVE_IPC) && !defined(ZMQ_HAVE_GNU)
    RUN_TEST (test_reconnect_ivl_ipc);
#endif
    RUN_TEST (test_reconnect_ivl_tcp_ipv4);
    RUN_TEST (test_reconnect_ivl_tcp_ipv6);

    return UNITY_END ();
}
