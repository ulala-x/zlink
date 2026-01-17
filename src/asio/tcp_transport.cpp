/* SPDX-License-Identifier: MPL-2.0 */

#include "precompiled.hpp"
#include "tcp_transport.hpp"

#if defined ZMQ_IOTHREAD_POLLER_USE_ASIO

#include "asio_debug.hpp"
#include "../address.hpp"
#include <atomic>
#include <cstdlib>
#include <cstdio>

namespace zmq
{
namespace
{
std::atomic<uint64_t> tcp_async_read_calls (0);
std::atomic<uint64_t> tcp_async_read_bytes (0);
std::atomic<uint64_t> tcp_async_read_errors (0);
std::atomic<uint64_t> tcp_read_some_calls (0);
std::atomic<uint64_t> tcp_read_some_bytes (0);
std::atomic<uint64_t> tcp_read_some_eagain (0);
std::atomic<uint64_t> tcp_read_some_errors (0);
std::atomic<uint64_t> tcp_async_write_calls (0);
std::atomic<uint64_t> tcp_async_write_bytes (0);
std::atomic<uint64_t> tcp_async_write_errors (0);
std::atomic<uint64_t> tcp_write_some_calls (0);
std::atomic<uint64_t> tcp_write_some_bytes (0);
std::atomic<uint64_t> tcp_write_some_eagain (0);
std::atomic<uint64_t> tcp_write_some_errors (0);
std::atomic<bool> tcp_stats_registered (false);

boost::asio::ip::tcp protocol_for_fd (fd_t fd_)
{
    sockaddr_storage ss;
    const zmq_socklen_t sl = get_socket_address (fd_, socket_end_local, &ss);
    if (sl != 0 && ss.ss_family == AF_INET6)
        return boost::asio::ip::tcp::v6 ();
    return boost::asio::ip::tcp::v4 ();
}

bool tcp_allow_sync_write ()
{
    static int enabled = -1;
    if (enabled == -1) {
        const char *env = std::getenv ("ZMQ_ASIO_TCP_SYNC_WRITE");
        enabled = (env && *env && *env != '0') ? 1 : 0;
    }
    return enabled == 1;
}

std::size_t tcp_max_transfer_size ()
{
    static std::size_t size = static_cast<std::size_t> (-1);
    if (size == static_cast<std::size_t> (-1)) {
        const char *env = std::getenv ("ZMQ_ASIO_TCP_MAX_TRANSFER");
        if (env && *env) {
            char *end = NULL;
            const unsigned long val = std::strtoul (env, &end, 10);
            if (end != env && val > 0)
                size = static_cast<std::size_t> (val);
            else
                size = 0;
        } else {
            size = 0;
        }
    }
    return size;
}

struct transfer_all_with_max_t
{
    explicit transfer_all_with_max_t (std::size_t total_,
                                      std::size_t max_)
        : total (total_), max_transfer (max_)
    {
    }

    template <typename Error>
    std::size_t operator()(const Error &err, std::size_t bytes_transferred)
    {
        if (err)
            return 0;
        const std::size_t remaining = total - bytes_transferred;
        return remaining < max_transfer ? remaining : max_transfer;
    }

    std::size_t total;
    std::size_t max_transfer;
};

bool tcp_stats_enabled ()
{
    static int enabled = -1;
    if (enabled == -1) {
        const char *env = std::getenv ("ZMQ_ASIO_TCP_STATS");
        enabled = (env && *env && *env != '0') ? 1 : 0;
    }
    return enabled == 1;
}

void tcp_stats_dump ()
{
    std::fprintf (stderr,
                  "[ASIO_TCP_STATS] async_read calls=%llu bytes=%llu errors=%llu\n"
                  "[ASIO_TCP_STATS] read_some calls=%llu bytes=%llu eagain=%llu "
                  "errors=%llu\n"
                  "[ASIO_TCP_STATS] async_write calls=%llu bytes=%llu errors=%llu\n"
                  "[ASIO_TCP_STATS] write_some calls=%llu bytes=%llu eagain=%llu errors=%llu\n",
                  static_cast<unsigned long long> (tcp_async_read_calls.load ()),
                  static_cast<unsigned long long> (tcp_async_read_bytes.load ()),
                  static_cast<unsigned long long> (tcp_async_read_errors.load ()),
                  static_cast<unsigned long long> (tcp_read_some_calls.load ()),
                  static_cast<unsigned long long> (tcp_read_some_bytes.load ()),
                  static_cast<unsigned long long> (tcp_read_some_eagain.load ()),
                  static_cast<unsigned long long> (tcp_read_some_errors.load ()),
                  static_cast<unsigned long long> (tcp_async_write_calls.load ()),
                  static_cast<unsigned long long> (tcp_async_write_bytes.load ()),
                  static_cast<unsigned long long> (tcp_async_write_errors.load ()),
                  static_cast<unsigned long long> (tcp_write_some_calls.load ()),
                  static_cast<unsigned long long> (tcp_write_some_bytes.load ()),
                  static_cast<unsigned long long> (tcp_write_some_eagain.load ()),
                  static_cast<unsigned long long> (tcp_write_some_errors.load ()));
}

void tcp_stats_maybe_register ()
{
    if (!tcp_stats_enabled ())
        return;
    bool expected = false;
    if (tcp_stats_registered.compare_exchange_strong (expected, true)) {
        std::atexit (tcp_stats_dump);
    }
}
}

tcp_transport_t::tcp_transport_t ()
{
}

tcp_transport_t::~tcp_transport_t ()
{
    close ();
}

bool tcp_transport_t::open (boost::asio::io_context &io_context, fd_t fd)
{
    try {
        _socket = std::unique_ptr<boost::asio::ip::tcp::socket> (
          new boost::asio::ip::tcp::socket (io_context));
    } catch (const std::bad_alloc &) {
        return false;
    }

    boost::system::error_code ec;
    _socket->assign (protocol_for_fd (fd), fd, ec);
    if (ec) {
        ASIO_GLOBAL_ERROR ("tcp_transport open failed: %s", ec.message ().c_str ());
        _socket.reset ();
        return false;
    }

    return true;
}

bool tcp_transport_t::is_open () const
{
    return _socket && _socket->is_open ();
}

void tcp_transport_t::close ()
{
    if (_socket) {
        boost::system::error_code ec;
        _socket->close (ec);
        //  Ignore close errors
        _socket.reset ();
    }
}

void tcp_transport_t::async_read_some (unsigned char *buffer,
                                       std::size_t buffer_size,
                                       completion_handler_t handler)
{
    if (tcp_stats_enabled ()) {
        tcp_stats_maybe_register ();
        ++tcp_async_read_calls;
    }

    if (_socket) {
        if (tcp_stats_enabled ()) {
            _socket->async_read_some (
              boost::asio::buffer (buffer, buffer_size),
              [handler](const boost::system::error_code &ec,
                        std::size_t bytes) {
                  if (ec)
                      ++tcp_async_read_errors;
                  else
                      tcp_async_read_bytes += bytes;
                  if (handler)
                      handler (ec, bytes);
              });
        } else {
            _socket->async_read_some (boost::asio::buffer (buffer, buffer_size),
                                      handler);
        }
    } else if (handler) {
        handler (boost::asio::error::bad_descriptor, 0);
    }
}

std::size_t tcp_transport_t::read_some (std::uint8_t *buffer, std::size_t len)
{
    if (len == 0) {
        errno = 0;
        return 0;
    }

    if (!_socket || !_socket->is_open ()) {
        errno = EBADF;
        return 0;
    }

    if (tcp_stats_enabled ()) {
        tcp_stats_maybe_register ();
        ++tcp_read_some_calls;
    }

    boost::system::error_code ec;
    const std::size_t bytes_read =
      _socket->read_some (boost::asio::buffer (buffer, len), ec);

    if (ec) {
        if (ec == boost::asio::error::would_block
            || ec == boost::asio::error::try_again) {
            errno = EAGAIN;
            if (tcp_stats_enabled ())
                ++tcp_read_some_eagain;
            return 0;
        }
        if (ec == boost::asio::error::eof
            || ec == boost::asio::error::connection_reset
            || ec == boost::asio::error::broken_pipe) {
            errno = EPIPE;
        } else if (ec == boost::asio::error::not_connected) {
            errno = ENOTCONN;
        } else if (ec == boost::asio::error::bad_descriptor) {
            errno = EBADF;
        } else {
            errno = EIO;
        }
        if (tcp_stats_enabled ())
            ++tcp_read_some_errors;
        return 0;
    }

    errno = 0;
    if (tcp_stats_enabled ())
        tcp_read_some_bytes += bytes_read;
    return bytes_read;
}

void tcp_transport_t::async_write_some (const unsigned char *buffer,
                                        std::size_t buffer_size,
                                        completion_handler_t handler)
{
    if (tcp_stats_enabled ()) {
        tcp_stats_maybe_register ();
        ++tcp_async_write_calls;
    }

    if (_socket) {
        const std::size_t max_transfer = tcp_max_transfer_size ();
        if (tcp_stats_enabled ()) {
            if (max_transfer > 0) {
                transfer_all_with_max_t completion (buffer_size, max_transfer);
                boost::asio::async_write (
                  *_socket, boost::asio::buffer (buffer, buffer_size),
                  completion,
                  [handler](const boost::system::error_code &ec,
                            std::size_t bytes) {
                      if (ec)
                          ++tcp_async_write_errors;
                      else
                          tcp_async_write_bytes += bytes;
                      if (handler)
                          handler (ec, bytes);
                  });
            } else {
                boost::asio::async_write (
                  *_socket, boost::asio::buffer (buffer, buffer_size),
                  [handler](const boost::system::error_code &ec,
                            std::size_t bytes) {
                      if (ec)
                          ++tcp_async_write_errors;
                      else
                          tcp_async_write_bytes += bytes;
                      if (handler)
                          handler (ec, bytes);
                  });
            }
        } else {
            if (max_transfer > 0) {
                transfer_all_with_max_t completion (buffer_size, max_transfer);
                boost::asio::async_write (
                  *_socket, boost::asio::buffer (buffer, buffer_size),
                  completion, handler);
            } else {
                boost::asio::async_write (
                  *_socket, boost::asio::buffer (buffer, buffer_size), handler);
            }
        }
    } else if (handler) {
        handler (boost::asio::error::bad_descriptor, 0);
    }
}

std::size_t tcp_transport_t::write_some (const std::uint8_t *data,
                                         std::size_t len)
{
    if (len == 0) {
        return 0;
    }

    boost::system::error_code ec;
    std::size_t bytes_written = 0;

    if (!_socket || !_socket->is_open ()) {
        errno = EBADF;
        return 0;
    }

    if (tcp_stats_enabled ()) {
        tcp_stats_maybe_register ();
        ++tcp_write_some_calls;
    }

    //  Perform synchronous write_some on TCP socket
    bytes_written =
      _socket->write_some (boost::asio::buffer (data, len), ec);

    if (ec) {
        //  Handle would_block case
        if (ec == boost::asio::error::would_block
            || ec == boost::asio::error::try_again) {
            errno = EAGAIN;
            if (tcp_stats_enabled ())
                ++tcp_write_some_eagain;
            return 0;
        }

        //  Map boost error codes to errno
        if (ec == boost::asio::error::broken_pipe
            || ec == boost::asio::error::connection_reset) {
            errno = EPIPE;
        } else if (ec == boost::asio::error::not_connected) {
            errno = ENOTCONN;
        } else if (ec == boost::asio::error::bad_descriptor) {
            errno = EBADF;
        } else {
            errno = EIO;
        }
        if (tcp_stats_enabled ())
            ++tcp_write_some_errors;
        return 0;
    }

    errno = 0;
    if (tcp_stats_enabled ())
        tcp_write_some_bytes += bytes_written;
    return bytes_written;
}

bool tcp_transport_t::supports_speculative_write () const
{
    return tcp_allow_sync_write ();
}

}  // namespace zmq

#endif  // ZMQ_IOTHREAD_POLLER_USE_ASIO
