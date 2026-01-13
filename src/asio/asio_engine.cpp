/* SPDX-License-Identifier: MPL-2.0 */

#include "../precompiled.hpp"
#if defined ZMQ_IOTHREAD_POLLER_USE_ASIO

#include "asio_engine.hpp"
#include "asio_poller.hpp"
#include "i_asio_transport.hpp"
#include "tcp_transport.hpp"
#include "../io_thread.hpp"
#include "../session_base.hpp"
#include "../socket_base.hpp"
#include "../null_mechanism.hpp"
#include "../plain_client.hpp"
#include "../plain_server.hpp"
#include "../config.hpp"
#include "../err.hpp"
#include "../ip.hpp"
#include "../tcp.hpp"
#include "../likely.hpp"
#include "../zmq_debug.h"

#include <cstdlib>
#include <cstring>
#ifndef ZMQ_HAVE_WINDOWS
#include <unistd.h>
#endif

#include <sstream>

// Debug logging for ASIO engine - enable with -DZMQ_ASIO_DEBUG=1
#if defined(ZMQ_ASIO_DEBUG)
#define ASIO_ENGINE_DEBUG 1
#else
#define ASIO_ENGINE_DEBUG 0
#endif

#if ASIO_ENGINE_DEBUG
#include <cstdio>
#define ENGINE_DBG(fmt, ...)                                                   \
    fprintf (stderr, "[ASIO_ENGINE] " fmt "\n", ##__VA_ARGS__)
#else
#define ENGINE_DBG(fmt, ...)
#endif

namespace
{
bool get_env_flag (const char *name_, bool default_value_)
{
    const char *value = std::getenv (name_);
    if (!value || *value == '\0')
        return default_value_;

    if (strcmp (value, "0") == 0 || strcmp (value, "false") == 0
        || strcmp (value, "off") == 0)
        return false;

    if (strcmp (value, "1") == 0 || strcmp (value, "true") == 0
        || strcmp (value, "on") == 0)
        return true;

    return default_value_;
}

bool is_tcp_endpoint (const std::string &endpoint_)
{
    static const char prefix[] = "tcp://";
    return endpoint_.compare (0, sizeof (prefix) - 1, prefix) == 0;
}
} // namespace

//  Draft API message property names (kept for internal use)
#define ZMQ_MSG_PROPERTY_ROUTING_ID "Routing-Id"
#define ZMQ_MSG_PROPERTY_SOCKET_TYPE "Socket-Type"
#define ZMQ_MSG_PROPERTY_USER_ID "User-Id"
#define ZMQ_MSG_PROPERTY_PEER_ADDRESS "Peer-Address"

static std::string get_peer_address (zmq::fd_t s_)
{
    std::string peer_address;

    const int family = zmq::get_peer_ip_address (s_, peer_address);
    if (family == 0)
        peer_address.clear ();
#if defined ZMQ_HAVE_SO_PEERCRED
    else if (family == PF_UNIX) {
        struct ucred cred;
        socklen_t size = sizeof (cred);
        if (!getsockopt (s_, SOL_SOCKET, SO_PEERCRED, &cred, &size)) {
            std::ostringstream buf;
            buf << ":" << cred.uid << ":" << cred.gid << ":" << cred.pid;
            peer_address += buf.str ();
        }
    }
#elif defined ZMQ_HAVE_LOCAL_PEERCRED
    else if (family == PF_UNIX) {
        struct xucred cred;
        socklen_t size = sizeof (cred);
        if (!getsockopt (s_, 0, LOCAL_PEERCRED, &cred, &size)
            && cred.cr_version == XUCRED_VERSION) {
            std::ostringstream buf;
            buf << ":" << cred.cr_uid << ":";
            if (cred.cr_ngroups > 0)
                buf << cred.cr_groups[0];
            buf << ":";
            peer_address += buf.str ();
        }
    }
#endif

    return peer_address;
}

zmq::asio_engine_t::asio_engine_t (
  fd_t fd_,
  const options_t &options_,
  const endpoint_uri_pair_t &endpoint_uri_pair_,
  std::unique_ptr<i_asio_transport> transport_) :
    _options (options_),
    _inpos (NULL),
    _insize (0),
    _decoder (NULL),
    _input_in_decoder_buffer (false),
    _outpos (NULL),
    _outsize (0),
    _encoder (NULL),
    _mechanism (NULL),
    _next_msg (NULL),
    _process_msg (NULL),
    _metadata (NULL),
    _input_stopped (false),
    _output_stopped (false),
    _endpoint_uri_pair (endpoint_uri_pair_),
    _has_handshake_timer (false),
    _has_ttl_timer (false),
    _has_timeout_timer (false),
    _has_heartbeat_timer (false),
    _peer_address (get_peer_address (fd_)),
    _has_handshake_stage (true),
    _io_context (NULL),
    _transport (std::move (transport_)),
    _current_timer_id (-1),
    _read_buffer (read_buffer_size),
    _fd (fd_),
    _plugged (false),
    _handshaking (true),
    _io_error (false),
    _read_pending (false),
    _write_pending (false),
    _terminating (false),
    _read_buffer_ptr (NULL),
    _session (NULL),
    _socket (NULL)
#ifdef ZMQ_ASIO_WRITE_BATCHING
    , _batch_timer_active (false)
    , _write_batch_max_bytes (64 * 1024)
    , _write_batch_max_messages (100)
    , _write_batch_timeout_us (100)
    , _write_batching_enabled (
        get_env_flag ("ZMQ_ASIO_WRITE_BATCHING", true))
#endif
#ifdef ZMQ_ASIO_LAZY_COMPACT
    , _lazy_compact_enabled (get_env_flag ("ZMQ_ASIO_LAZY_COMPACT", true))
#endif
#ifdef ZMQ_ASIO_ZEROCOPY_WRITE
    , _using_zerocopy (false)
    , _zerocopy_header_size (0)
    , _zerocopy_body (NULL)
    , _zerocopy_body_size (0)
    , _zerocopy_write_enabled (
        get_env_flag ("ZMQ_ASIO_ZEROCOPY_WRITE", true))
#endif
#ifdef ZMQ_ASIO_HANDSHAKE_ZEROCOPY
    , _handshake_zerocopy_enabled (
        get_env_flag ("ZMQ_ASIO_HANDSHAKE_ZEROCOPY", true))
#endif
{
    ENGINE_DBG ("Constructor called, fd=%d", fd_);

    const int rc = _tx_msg.init ();
    errno_assert (rc == 0);

    if (!_transport) {
        _transport = std::unique_ptr<i_asio_transport> (
          new (std::nothrow) tcp_transport_t ());
        alloc_assert (_transport);
    }

    //  Put the socket into non-blocking mode.
    unblock_socket (_fd);
}

zmq::asio_engine_t::~asio_engine_t ()
{
    ENGINE_DBG ("Destructor called");

    zmq_assert (!_plugged);

    if (_transport) {
        _transport->close ();
    } else if (_fd != retired_fd) {
#ifdef ZMQ_HAVE_WINDOWS
        const int rc = closesocket (_fd);
        wsa_assert (rc != SOCKET_ERROR);
#else
        int rc = close (_fd);
#if defined(__FreeBSD_kernel__) || defined(__FreeBSD__)
        // FreeBSD may return ECONNRESET on close() under load but this is not
        // an error.
        if (rc == -1 && errno == ECONNRESET)
            rc = 0;
#endif
        errno_assert (rc == 0);
#endif
        _fd = retired_fd;
    }

    const int rc = _tx_msg.close ();
    errno_assert (rc == 0);

    //  Drop reference to metadata and destroy it if we are
    //  the only user.
    if (_metadata != NULL) {
        if (_metadata->drop_ref ()) {
            LIBZMQ_DELETE (_metadata);
        }
    }

    LIBZMQ_DELETE (_encoder);
    LIBZMQ_DELETE (_decoder);
    LIBZMQ_DELETE (_mechanism);

    //  Smart pointers will automatically clean up ASIO objects
    //  (_timer, _socket_handle/_stream_descriptor)
}

void zmq::asio_engine_t::plug (io_thread_t *io_thread_,
                               session_base_t *session_)
{
    ENGINE_DBG ("plug called");

    zmq_assert (!_plugged);
    _plugged = true;

    //  Connect to session object.
    zmq_assert (!_session);
    zmq_assert (session_);
    _session = session_;
    _socket = _session->get_socket ();

    //  Get reference to io_context from the io_thread's poller
    asio_poller_t *poller =
      static_cast<asio_poller_t *> (io_thread_->get_poller ());
    _io_context = &poller->get_io_context ();

    //  Allocate timer with correct io_context
    _timer = std::unique_ptr<boost::asio::steady_timer> (
      new boost::asio::steady_timer (*_io_context));
#ifdef ZMQ_ASIO_WRITE_BATCHING
    _batch_timer = std::unique_ptr<boost::asio::steady_timer> (
      new boost::asio::steady_timer (*_io_context));
    _batch_timer_active = false;
#endif

    _io_error = false;

    if (!_transport || !_transport->open (*_io_context, _fd)) {
        error (connection_error);
        return;
    }

    if (_transport->requires_handshake ()) {
        start_transport_handshake ();
        return;
    }

    plug_internal ();
}

void zmq::asio_engine_t::start_transport_handshake ()
{
    ENGINE_DBG ("start_transport_handshake");

    if (_options.handshake_ivl > 0)
        set_handshake_timer ();

    const int handshake_type =
      _endpoint_uri_pair.local_type == endpoint_type_connect ? 0 : 1;

    _transport->async_handshake (
      handshake_type,
      [this] (const boost::system::error_code &ec, std::size_t) {
          on_transport_handshake (ec);
      });
}

void zmq::asio_engine_t::on_transport_handshake (
  const boost::system::error_code &ec)
{
    ENGINE_DBG ("on_transport_handshake: ec=%s, terminating=%d",
                ec.message ().c_str (), _terminating);

    if (_terminating)
        return;

    if (!_plugged)
        return;

    if (_has_handshake_timer) {
        cancel_timer (handshake_timer_id);
        _has_handshake_timer = false;
    }

    if (ec) {
        error (connection_error);
        return;
    }

    plug_internal ();
}

void zmq::asio_engine_t::unplug ()
{
    ENGINE_DBG ("unplug called");

    zmq_assert (_plugged);
    _plugged = false;

    //  Cancel all timers.
    if (_has_handshake_timer) {
        cancel_timer (handshake_timer_id);
        _has_handshake_timer = false;
    }

    if (_has_ttl_timer) {
        cancel_timer (heartbeat_ttl_timer_id);
        _has_ttl_timer = false;
    }

    if (_has_timeout_timer) {
        cancel_timer (heartbeat_timeout_timer_id);
        _has_timeout_timer = false;
    }

    if (_has_heartbeat_timer) {
        cancel_timer (heartbeat_ivl_timer_id);
        _has_heartbeat_timer = false;
    }

    //  Cancel pending async operations by closing the transport
    if (_transport)
        _transport->close ();
    if (_timer)
        _timer->cancel ();
#ifdef ZMQ_ASIO_WRITE_BATCHING
    if (_batch_timer) {
        _batch_timer->cancel ();
        _batch_timer_active = false;
    }
#endif

    _session = NULL;
}

void zmq::asio_engine_t::terminate ()
{
    ENGINE_DBG ("terminate called");

    //  Mark as terminating to prevent callbacks from processing
    _terminating = true;

    unplug ();

    //  Drain any pending async handlers while the object is still alive.
    //  The _terminating flag ensures callbacks are no-ops.
    if (_io_context) {
        while (_io_context->poll () > 0) {
        }
    }

    delete this;
}

void zmq::asio_engine_t::start_async_read ()
{
    if (_read_pending || _input_stopped || _io_error)
        return;

    ENGINE_DBG ("start_async_read: insize=%zu", _insize);

    _read_pending = true;

    //  Get buffer from decoder if available
    size_t read_size;

    if (_decoder) {
        _decoder->get_buffer (&_read_buffer_ptr, &read_size);

        if (_insize > 0) {
            unsigned char *const buffer_start = _read_buffer_ptr;
            unsigned char *const buffer_end = buffer_start + read_size;
            unsigned char *const data_end = _inpos + _insize;
            const size_t remaining =
              data_end <= buffer_end ? buffer_end - data_end : 0;
            const size_t consumed = _inpos - buffer_start;

#ifdef ZMQ_ASIO_LAZY_COMPACT
            if (_lazy_compact_enabled) {
                static const size_t min_consumed = 2048;
                const size_t min_space =
                  std::max<size_t> (2048, read_size / 5);
                bool need_compaction = remaining == 0;
                if (!need_compaction) {
                    need_compaction =
                      consumed >= min_consumed && remaining < min_space;
                }

                if (need_compaction) {
                    ENGINE_DBG ("start_async_read: compacting %zu partial bytes "
                                "(consumed=%zu, remaining=%zu)",
                                _insize, consumed, remaining);
                    memmove (buffer_start, _inpos, _insize);
                    _inpos = buffer_start;
                    _read_buffer_ptr = buffer_start + _insize;
                    read_size -= _insize;
                    zmq_debug_inc_compaction_count ();
                    zmq_debug_add_compaction_bytes (_insize);
                } else {
                    _read_buffer_ptr = data_end;
                    read_size = remaining;
                    zmq_debug_inc_compaction_skipped_count ();
                }
            } else {
                if (_inpos != _read_buffer_ptr) {
                    ENGINE_DBG ("start_async_read: moving %zu partial bytes to buffer start",
                                _insize);
                    memmove (_read_buffer_ptr, _inpos, _insize);
                    _inpos = _read_buffer_ptr;
                }

                if (read_size > _insize) {
                    _read_buffer_ptr += _insize;
                    read_size -= _insize;
                }
            }
#else
            //  If we have partial data from previous read, move it to buffer start.
            //  The decoder expects data to start from the beginning of its buffer.
            if (_inpos != _read_buffer_ptr) {
                ENGINE_DBG ("start_async_read: moving %zu partial bytes to buffer start",
                            _insize);
                memmove (_read_buffer_ptr, _inpos, _insize);
                _inpos = _read_buffer_ptr;
            }

            //  Read into buffer after any existing partial data
            if (read_size > _insize) {
                _read_buffer_ptr += _insize;
                read_size -= _insize;
            }
#endif
        }
    } else {
        //  During handshake, use internal buffer
        _read_buffer_ptr = _read_buffer.data ();
        read_size = _read_buffer.size ();
    }

    ENGINE_DBG ("start_async_read: reading up to %zu bytes", read_size);

    if (_transport) {
        _transport->async_read_some (
          _read_buffer_ptr, read_size,
          [this] (const boost::system::error_code &ec, std::size_t bytes) {
              on_read_complete (ec, bytes);
          });
    }
}

void zmq::asio_engine_t::start_async_write ()
{
    if (_write_pending || _io_error)
        return;

    ENGINE_DBG ("start_async_write");

#ifdef ZMQ_ASIO_WRITE_BATCHING
    if (_write_batching_enabled && _batch_timer_active && _batch_timer) {
        _batch_timer->cancel ();
        _batch_timer_active = false;
    }
#endif

    //  Prepare output buffer
    process_output ();

#ifdef ZMQ_ASIO_ZEROCOPY_WRITE
    //  Check if we're using zero-copy path
    if (_zerocopy_write_enabled && _using_zerocopy) {
        ENGINE_DBG ("start_async_write: scatter-gather, header=%zu, body=%zu",
                    _zerocopy_header_size, _zerocopy_body_size);

        _write_pending = true;
        zmq_debug_inc_scatter_gather_count ();

        std::vector<boost::asio::const_buffer> buffers;
        if (!_write_buffer.empty ()) {
            buffers.push_back (
              boost::asio::buffer (_write_buffer.data (), _write_buffer.size ()));
        }
        if (_zerocopy_body_size > 0 && _zerocopy_body) {
            buffers.push_back (
              boost::asio::buffer (_zerocopy_body, _zerocopy_body_size));
        }

        if (_transport) {
            _transport->async_write_scatter (
              buffers,
              [this] (const boost::system::error_code &ec, std::size_t bytes) {
                  on_zerocopy_write_complete (ec, bytes);
              });
        }
        return;
    }
#endif

    if (_write_buffer.empty ()) {
        _output_stopped = true;
        return;
    }

    _write_pending = true;

    if (_transport) {
        _transport->async_write_some (
          _write_buffer.data (), _write_buffer.size (),
          [this] (const boost::system::error_code &ec, std::size_t bytes) {
              on_write_complete (ec, bytes);
          });
    }
}

#ifdef ZMQ_ASIO_WRITE_BATCHING
void zmq::asio_engine_t::start_batch_timer ()
{
    if (!_write_batching_enabled || !_batch_timer || _batch_timer_active)
        return;

    _batch_timer_active = true;
    _batch_timer->expires_after (
      std::chrono::microseconds (_write_batch_timeout_us));
    _batch_timer->async_wait ([this] (const boost::system::error_code &ec) {
        on_batch_timer (ec);
    });
}

void zmq::asio_engine_t::on_batch_timer (const boost::system::error_code &ec)
{
    _batch_timer_active = false;

    if (!_write_batching_enabled)
        return;

    if (ec == boost::asio::error::operation_aborted)
        return;

    if (_terminating || _io_error || _write_pending || _output_stopped)
        return;

    zmq_debug_inc_batch_timeout_flush_count ();
    start_async_write ();
}
#endif

void zmq::asio_engine_t::on_read_complete (const boost::system::error_code &ec,
                                           std::size_t bytes_transferred)
{
    _read_pending = false;
    ENGINE_DBG ("on_read_complete: ec=%s, bytes=%zu, terminating=%d",
                ec.message ().c_str (), bytes_transferred, _terminating);

    //  If terminating, just return - terminate() is draining handlers
    if (_terminating)
        return;

    if (!_plugged)
        return;

    if (ec) {
        if (ec == boost::asio::error::operation_aborted)
            return;
        error (connection_error);
        return;
    }

    if (bytes_transferred == 0) {
        //  Connection closed by peer
        error (connection_error);
        return;
    }

    //  Handle buffer pointers based on whether we have partial data
    if (_decoder && _insize > 0) {
        //  We have partial data from previous read.
        //  start_async_read() moved partial data to buffer start and set _inpos there.
        //  New data was read right after the partial data.
        const size_t partial_size = _insize;
        _insize = partial_size + bytes_transferred;
        ENGINE_DBG ("on_read_complete: total %zu bytes (partial=%zu + new=%zu)",
                    _insize, partial_size, bytes_transferred);
        //  Note: Do NOT call resize_buffer() here - it interferes with decoder's
        //  internal state management. The decoder tracks its own buffer state.
    } else {
        //  Fresh read - set buffer pointers
        _inpos = _read_buffer_ptr;
        _insize = bytes_transferred;
        //  Note: Do NOT call resize_buffer() here - let process_input() handle
        //  decoder buffer management when data is copied to decoder buffer.
    }
    _input_in_decoder_buffer = (_decoder != NULL);

    //  Process received data
    if (!process_input ()) {
        //  If handshaking and need more data (EAGAIN), continue I/O
        if (_handshaking && errno == EAGAIN) {
            //  Send any output that was built during greeting processing
            if (_outsize > 0)
                start_async_write ();
            start_async_read ();
        }
        return;
    }

    //  Continue reading if not stopped.
    //  If _insize > 0, there's still unprocessed data (partial message) that
    //  needs more data to complete. The new read will append to existing data.
    if (!_input_stopped)
        start_async_read ();
}

void zmq::asio_engine_t::on_write_complete (const boost::system::error_code &ec,
                                            std::size_t bytes_transferred)
{
    _write_pending = false;
    ENGINE_DBG ("on_write_complete: ec=%s, bytes=%zu, terminating=%d",
                ec.message ().c_str (), bytes_transferred, _terminating);

    //  If terminating, just return - terminate() is draining handlers
    if (_terminating)
        return;

    if (!_plugged)
        return;

    if (ec) {
        if (ec == boost::asio::error::operation_aborted)
            return;
        //  IO error - stop writing but continue reading to detect connection close
        _io_error = true;
        return;
    }

    //  Check for partial write
    if (bytes_transferred < _write_buffer.size ()) {
        zmq_debug_inc_partial_write_count ();
    }

    _write_buffer.clear ();

    //  If still handshaking and there's nothing more to write, just return
    if (_handshaking && _outsize == 0)
        return;

    //  Continue writing if more data available
    if (!_output_stopped)
        start_async_write ();
}

#ifdef ZMQ_ASIO_ZEROCOPY_WRITE
void zmq::asio_engine_t::on_zerocopy_write_complete (
  const boost::system::error_code &ec, std::size_t bytes_transferred)
{
    _write_pending = false;
    ENGINE_DBG ("on_zerocopy_write_complete: ec=%s, bytes=%zu, terminating=%d",
                ec.message ().c_str (), bytes_transferred, _terminating);

    const size_t total_size = _zerocopy_header_size + _zerocopy_body_size;
    size_t body_written = 0;
    if (bytes_transferred > _zerocopy_header_size)
        body_written = bytes_transferred - _zerocopy_header_size;

    //  If terminating, just return - terminate() is draining handlers
    if (_terminating) {
        //  Release encoder buffer before returning
        if (_encoder) {
            _encoder->release_output_buffer_ref (_zerocopy_body_size);
        }
        _write_buffer.clear ();
        _zerocopy_header_size = 0;
        _zerocopy_body = NULL;
        _zerocopy_body_size = 0;
        _outpos = NULL;
        _outsize = 0;
        return;
    }

    if (!_plugged) {
        //  Release encoder buffer before returning
        if (_encoder) {
            _encoder->release_output_buffer_ref (_zerocopy_body_size);
        }
        _write_buffer.clear ();
        _zerocopy_header_size = 0;
        _zerocopy_body = NULL;
        _zerocopy_body_size = 0;
        _outpos = NULL;
        _outsize = 0;
        return;
    }

    if (ec) {
        //  Release encoder buffer on error
        if (_encoder) {
            _encoder->release_output_buffer_ref (_zerocopy_body_size);
        }
        _write_buffer.clear ();
        _zerocopy_header_size = 0;
        _zerocopy_body = NULL;
        _zerocopy_body_size = 0;
        _outpos = NULL;
        _outsize = 0;

        if (ec == boost::asio::error::operation_aborted)
            return;
        //  IO error - stop writing but continue reading to detect connection close
        _io_error = true;
        return;
    }

    //  Check for partial write
    if (bytes_transferred < total_size) {
        zmq_debug_inc_partial_write_count ();
    }

    //  Release encoder buffer after successful write
    if (_encoder) {
        if (body_written > _zerocopy_body_size)
            body_written = _zerocopy_body_size;
        _encoder->release_output_buffer_ref (body_written);
    }

    //  Reset output position and size
    _write_buffer.clear ();
    _zerocopy_header_size = 0;
    _zerocopy_body = NULL;
    _zerocopy_body_size = 0;
    _outpos = NULL;
    _outsize = 0;
    _using_zerocopy = false;

    //  Continue writing if more data available
    if (!_output_stopped)
        start_async_write ();
}
#endif

bool zmq::asio_engine_t::process_input ()
{
    ENGINE_DBG ("process_input: handshaking=%d, insize=%zu", _handshaking,
                _insize);

    //  If still handshaking, receive and process the greeting message.
    if (unlikely (_handshaking)) {
        if (handshake ()) {
            //  Handshaking was successful.
            //  Switch into the normal message flow.
            _handshaking = false;

            if (_mechanism == NULL && _has_handshake_stage) {
                _session->engine_ready ();

                if (_has_handshake_timer) {
                    cancel_timer (handshake_timer_id);
                    _has_handshake_timer = false;
                }
            }

            //  After greeting exchange completes, trigger output to send
            //  mechanism handshake (e.g., READY command)
            if (_mechanism != NULL) {
                ENGINE_DBG ("process_input: greeting done, triggering write");
                start_async_write ();
            }
        } else
            return false;
    }

    zmq_assert (_decoder);

    //  If there has been an I/O error, stop.
    if (_input_stopped) {
        _io_error = true;
        return true;
    }

    const bool input_in_decoder_buffer = _input_in_decoder_buffer;

    ENGINE_DBG (
      "process_input: after handshake, insize=%zu, input_in_decoder_buffer=%d",
      _insize, input_in_decoder_buffer);

    int rc = 0;
    size_t processed = 0;

    while (_insize > 0) {
        unsigned char *decode_buf;
        size_t decode_size;

        decode_buf = _inpos;
        decode_size = _insize;

        if (!input_in_decoder_buffer) {
#ifdef ZMQ_ASIO_HANDSHAKE_ZEROCOPY
            const bool handshake_zerocopy =
              _handshake_zerocopy_enabled
              && is_tcp_endpoint (_endpoint_uri_pair.identifier ());
            if (handshake_zerocopy) {
                decode_buf = _inpos;
                decode_size = _insize;
            } else {
                _decoder->get_buffer (&decode_buf, &decode_size);
                decode_size = std::min (_insize, decode_size);
                memcpy (decode_buf, _inpos, decode_size);
                _decoder->resize_buffer (decode_size);
                zmq_debug_inc_handshake_copy_count ();
                zmq_debug_add_handshake_copy_bytes (decode_size);
                ENGINE_DBG ("process_input: copied %zu bytes from external input "
                            "buffer to decoder buffer",
                            decode_size);
            }
#else
            //  Data came from an external buffer (handshake/greeting); copy
            //  into decoder buffer to preserve decoder buffer invariants.
            _decoder->get_buffer (&decode_buf, &decode_size);
            decode_size = std::min (_insize, decode_size);
            memcpy (decode_buf, _inpos, decode_size);
            _decoder->resize_buffer (decode_size);
            zmq_debug_inc_handshake_copy_count ();
            zmq_debug_add_handshake_copy_bytes (decode_size);
            ENGINE_DBG ("process_input: copied %zu bytes from external input "
                        "buffer to decoder buffer",
                        decode_size);
#endif
        }

        ENGINE_DBG ("process_input: decode loop decode_size=%zu", decode_size);
        rc = _decoder->decode (decode_buf, decode_size, processed);
        ENGINE_DBG ("process_input: decode returned rc=%d, processed=%zu", rc,
                    processed);
        zmq_assert (processed <= decode_size);
        _inpos += processed;
        _insize -= processed;

        if (rc == 0 || rc == -1)
            break;
        rc = (this->*_process_msg) (_decoder->msg ());
        ENGINE_DBG ("process_input: process_msg returned rc=%d", rc);
        if (rc == -1)
            break;
    }

    //  Tear down the connection if we have failed to decode input data
    //  or the session has rejected the message.
    if (rc == -1) {
        if (errno != EAGAIN) {
            error (protocol_error);
            return false;
        }
        _input_stopped = true;
    }

    _session->flush ();
    return true;
}

void zmq::asio_engine_t::process_output ()
{
    ENGINE_DBG ("process_output: outsize=%zu", _outsize);

    //  If write buffer is empty, try to read new data from the encoder.
    if (_outsize == 0) {
        //  Even when we stop as soon as there is no data to send,
        //  there may be a pending async_write.
        if (unlikely (_encoder == NULL)) {
            zmq_assert (_handshaking);
            return;
        }

        _outpos = NULL;
        _outsize = _encoder->encode (&_outpos, 0);

#ifdef ZMQ_ASIO_WRITE_BATCHING
        const bool batching_enabled =
          _write_batching_enabled && !_handshaking;
        size_t msg_count = 0;
        const size_t target_batch = batching_enabled
          ? std::min<size_t> (static_cast<size_t> (_options.out_batch_size),
                              _write_batch_max_bytes)
          : static_cast<size_t> (_options.out_batch_size);
#else
        const size_t target_batch =
          static_cast<size_t> (_options.out_batch_size);
#endif

        while (_outsize < target_batch) {
            if ((this->*_next_msg) (&_tx_msg) == -1) {
                if (errno == ECONNRESET)
                    return;
                else
                    break;
            }
            _encoder->load_msg (&_tx_msg);
#ifdef ZMQ_ASIO_WRITE_BATCHING
            if (batching_enabled)
                ++msg_count;
#endif

#ifdef ZMQ_ASIO_ZEROCOPY_WRITE
            _using_zerocopy = false;
            _zerocopy_header_size = 0;
            _zerocopy_body = NULL;
            _zerocopy_body_size = 0;

            static const size_t ZEROCOPY_THRESHOLD = 8192;
            if (_zerocopy_write_enabled && !_handshaking
                && _tx_msg.size () >= ZEROCOPY_THRESHOLD) {
                encoder_buffer_ref header_ref;
                if (_encoder->get_output_buffer_ref (header_ref)
                    && header_ref.size > 0) {
                    const size_t header_size = header_ref.size;
                    const size_t pending_size = _outsize;
                    const size_t combined_size = pending_size + header_size;
                    if (_write_buffer.capacity () < combined_size)
                        _write_buffer.reserve (combined_size);
                    if (pending_size > 0) {
                        _write_buffer.resize (pending_size);
                        memcpy (_write_buffer.data (), _outpos, pending_size);
                        zmq_debug_add_bytes_copied (pending_size);
#ifdef ZMQ_ASIO_WRITE_BATCHING
                        if (batching_enabled) {
                            size_t batch_messages = msg_count > 1 ? msg_count - 1 : 1;
                            zmq_debug_inc_batch_flush_count ();
                            if (pending_size >= target_batch)
                                zmq_debug_inc_batch_size_flush_count ();
                            if (batch_messages >= _write_batch_max_messages)
                                zmq_debug_inc_batch_count_flush_count ();
                            zmq_debug_add_batch_messages (batch_messages);
                            zmq_debug_add_batch_bytes (pending_size);
                        }
#endif
                    }
                    _write_buffer.resize (combined_size);
                    memcpy (_write_buffer.data () + pending_size, header_ref.data,
                            header_size);
                    zmq_debug_add_bytes_copied (header_size);

                    _encoder->release_output_buffer_ref (header_size);

                    _using_zerocopy = true;
                    _zerocopy_header_size = header_size;
                    _zerocopy_body = static_cast<unsigned char *> (_tx_msg.data ());
                    _zerocopy_body_size = _tx_msg.size ();
                    _outpos = NULL;
                    _outsize = 0;
                    zmq_debug_inc_zerocopy_count ();
                    return;
                }
            }
#endif

            unsigned char *bufptr = _outpos + _outsize;
            const size_t n = _encoder->encode (&bufptr, target_batch - _outsize);
            zmq_assert (n > 0);
            if (_outpos == NULL)
                _outpos = bufptr;
            _outsize += n;

#ifdef ZMQ_ASIO_WRITE_BATCHING
            if (batching_enabled && msg_count >= _write_batch_max_messages)
                break;
#endif
        }

#ifdef ZMQ_ASIO_WRITE_BATCHING
        if (batching_enabled && _outsize > 0) {
            size_t batch_messages = msg_count;
            if (batch_messages == 0)
                batch_messages = 1;
            zmq_debug_inc_batch_flush_count ();
            if (_outsize >= target_batch)
                zmq_debug_inc_batch_size_flush_count ();
            if (msg_count >= _write_batch_max_messages)
                zmq_debug_inc_batch_count_flush_count ();
            zmq_debug_add_batch_messages (batch_messages);
            zmq_debug_add_batch_bytes (_outsize);
        }
#endif

        //  If there is no data to send, mark output as stopped.
        if (_outsize == 0) {
            _output_stopped = true;
            return;
        }
    }

#ifdef ZMQ_ASIO_ZEROCOPY_WRITE
    _using_zerocopy = false;
    if (_zerocopy_write_enabled) {
        zmq_debug_inc_fallback_count ();
        zmq_debug_add_bytes_copied (_outsize);
    }
#endif

    //  Copy data to write buffer (reuse capacity to avoid reallocations)
    const size_t out_batch_size =
      static_cast<size_t> (_options.out_batch_size);
    const size_t target =
      _outsize > out_batch_size ? _outsize : out_batch_size;
    if (_write_buffer.capacity () < target)
        _write_buffer.reserve (target);
    _write_buffer.resize (_outsize);
    memcpy (_write_buffer.data (), _outpos, _outsize);

    //  During handshake, advance _outpos but don't reset it to NULL
    //  so that receive_greeting_versioned() can check position correctly
    if (_handshaking) {
        _outpos += _outsize;
        _outsize = 0;
    } else {
        _outpos = NULL;
        _outsize = 0;
    }
}

void zmq::asio_engine_t::restart_output ()
{
    if (unlikely (_io_error))
        return;

    if (likely (_output_stopped)) {
        _output_stopped = false;
    }

    //  Start async write
#ifdef ZMQ_ASIO_WRITE_BATCHING
    if (!_write_batching_enabled || _handshaking || _write_pending)
        start_async_write ();
    else
        start_batch_timer ();
#else
    start_async_write ();
#endif
}

bool zmq::asio_engine_t::restart_input ()
{
    zmq_assert (_input_stopped);
    zmq_assert (_session != NULL);
    zmq_assert (_decoder != NULL);

    int rc = (this->*_process_msg) (_decoder->msg ());
    if (rc == -1) {
        if (errno == EAGAIN)
            _session->flush ();
        else {
            error (protocol_error);
            return false;
        }
        return true;
    }

    while (_insize > 0) {
        size_t processed = 0;
        unsigned char *decode_buf = _inpos;
        size_t decode_size = _insize;

        if (!_input_in_decoder_buffer) {
            _decoder->get_buffer (&decode_buf, &decode_size);
            decode_size = std::min (_insize, decode_size);
            memcpy (decode_buf, _inpos, decode_size);
            _decoder->resize_buffer (decode_size);
        }

        rc = _decoder->decode (decode_buf, decode_size, processed);
        zmq_assert (processed <= _insize);
        _inpos += processed;
        _insize -= processed;
        if (rc == 0 || rc == -1)
            break;
        rc = (this->*_process_msg) (_decoder->msg ());
        if (rc == -1)
            break;
    }

    if (rc == -1 && errno == EAGAIN)
        _session->flush ();
    else if (_io_error) {
        error (connection_error);
        return false;
    } else if (rc == -1) {
        error (protocol_error);
        return false;
    } else {
        _input_stopped = false;
        _session->flush ();

        //  Continue reading
        start_async_read ();
    }

    return true;
}

int zmq::asio_engine_t::next_handshake_command (msg_t *msg_)
{
    zmq_assert (_mechanism != NULL);

    if (_mechanism->status () == mechanism_t::ready) {
        mechanism_ready ();
        return pull_and_encode (msg_);
    }
    if (_mechanism->status () == mechanism_t::error) {
        errno = EPROTO;
        return -1;
    }
    const int rc = _mechanism->next_handshake_command (msg_);

    if (rc == 0)
        msg_->set_flags (msg_t::command);

    return rc;
}

int zmq::asio_engine_t::process_handshake_command (msg_t *msg_)
{
    zmq_assert (_mechanism != NULL);
    const int rc = _mechanism->process_handshake_command (msg_);
    if (rc == 0) {
        if (_mechanism->status () == mechanism_t::ready)
            mechanism_ready ();
        else if (_mechanism->status () == mechanism_t::error) {
            errno = EPROTO;
            return -1;
        }
        if (_output_stopped)
            restart_output ();
    }

    return rc;
}

void zmq::asio_engine_t::zap_msg_available ()
{
    zmq_assert (_mechanism != NULL);

    const int rc = _mechanism->zap_msg_available ();
    if (rc == -1) {
        error (protocol_error);
        return;
    }
    if (_input_stopped)
        if (!restart_input ())
            return;
    if (_output_stopped)
        restart_output ();
}

const zmq::endpoint_uri_pair_t &zmq::asio_engine_t::get_endpoint () const
{
    return _endpoint_uri_pair;
}

void zmq::asio_engine_t::mechanism_ready ()
{
    if (_options.heartbeat_interval > 0 && !_has_heartbeat_timer) {
        add_timer (_options.heartbeat_interval, heartbeat_ivl_timer_id);
        _has_heartbeat_timer = true;
    }

    if (_has_handshake_stage)
        _session->engine_ready ();

    bool flush_session = false;

    if (_options.recv_routing_id) {
        msg_t routing_id;
        _mechanism->peer_routing_id (&routing_id);
        const int rc = _session->push_msg (&routing_id);
        if (rc == -1 && errno == EAGAIN) {
            return;
        }
        errno_assert (rc == 0);
        flush_session = true;
    }

    if (flush_session)
        _session->flush ();

    _next_msg = &asio_engine_t::pull_and_encode;
    _process_msg = &asio_engine_t::write_credential;

    //  Compile metadata.
    properties_t properties;
    init_properties (properties);

    //  Add ZAP properties.
    const properties_t &zap_properties = _mechanism->get_zap_properties ();
    properties.insert (zap_properties.begin (), zap_properties.end ());

    //  Add ZMTP properties.
    const properties_t &zmtp_properties = _mechanism->get_zmtp_properties ();
    properties.insert (zmtp_properties.begin (), zmtp_properties.end ());

    zmq_assert (_metadata == NULL);
    if (!properties.empty ()) {
        _metadata = new (std::nothrow) metadata_t (properties);
        alloc_assert (_metadata);
    }

    if (_has_handshake_timer) {
        cancel_timer (handshake_timer_id);
        _has_handshake_timer = false;
    }

    _socket->event_handshake_succeeded (_endpoint_uri_pair, 0);
}

int zmq::asio_engine_t::write_credential (msg_t *msg_)
{
    zmq_assert (_mechanism != NULL);
    zmq_assert (_session != NULL);

    const blob_t &credential = _mechanism->get_user_id ();
    if (credential.size () > 0) {
        msg_t msg;
        int rc = msg.init_size (credential.size ());
        zmq_assert (rc == 0);
        memcpy (msg.data (), credential.data (), credential.size ());
        msg.set_flags (msg_t::credential);
        rc = _session->push_msg (&msg);
        if (rc == -1) {
            rc = msg.close ();
            errno_assert (rc == 0);
            return -1;
        }
    }
    _process_msg = &asio_engine_t::decode_and_push;
    return decode_and_push (msg_);
}

int zmq::asio_engine_t::pull_and_encode (msg_t *msg_)
{
    zmq_assert (_mechanism != NULL);

    if (_session->pull_msg (msg_) == -1)
        return -1;
    if (_mechanism->encode (msg_) == -1)
        return -1;
    return 0;
}

int zmq::asio_engine_t::decode_and_push (msg_t *msg_)
{
    zmq_assert (_mechanism != NULL);

    if (_mechanism->decode (msg_) == -1)
        return -1;

    if (_has_timeout_timer) {
        _has_timeout_timer = false;
        cancel_timer (heartbeat_timeout_timer_id);
    }

    if (_has_ttl_timer) {
        _has_ttl_timer = false;
        cancel_timer (heartbeat_ttl_timer_id);
    }

    if (msg_->flags () & msg_t::command) {
        process_command_message (msg_);
    }

    if (_metadata)
        msg_->set_metadata (_metadata);
    if (_session->push_msg (msg_) == -1) {
        if (errno == EAGAIN)
            _process_msg = &asio_engine_t::push_one_then_decode_and_push;
        return -1;
    }
    return 0;
}

int zmq::asio_engine_t::push_one_then_decode_and_push (msg_t *msg_)
{
    const int rc = _session->push_msg (msg_);
    if (rc == 0)
        _process_msg = &asio_engine_t::decode_and_push;
    return rc;
}

int zmq::asio_engine_t::pull_msg_from_session (msg_t *msg_)
{
    return _session->pull_msg (msg_);
}

int zmq::asio_engine_t::push_msg_to_session (msg_t *msg_)
{
    return _session->push_msg (msg_);
}

void zmq::asio_engine_t::error (error_reason_t reason_)
{
    ENGINE_DBG ("error: reason=%d", static_cast<int> (reason_));

    //  Mark as terminating to prevent callbacks from processing
    _terminating = true;

    zmq_assert (_session);

    // protocol errors have been signaled already at the point where they occurred
    if (reason_ != protocol_error
        && (_mechanism == NULL
            || _mechanism->status () == mechanism_t::handshaking)) {
        const int err = errno;
        _socket->event_handshake_failed_no_detail (_endpoint_uri_pair, err);
    }

    _socket->event_disconnected (_endpoint_uri_pair, _fd);
    _session->flush ();
    _session->engine_error (
      !_handshaking
        && (_mechanism == NULL
            || _mechanism->status () != mechanism_t::handshaking),
      reason_);
    unplug ();

    //  Drain any pending async handlers while the object is still alive.
    //  The _terminating flag ensures callbacks are no-ops.
    if (_io_context) {
        while (_io_context->poll () > 0) {
        }
    }

    delete this;
}

void zmq::asio_engine_t::set_handshake_timer ()
{
    zmq_assert (!_has_handshake_timer);

    if (_options.handshake_ivl > 0) {
        add_timer (_options.handshake_ivl, handshake_timer_id);
        _has_handshake_timer = true;
    }
}

void zmq::asio_engine_t::cancel_handshake_timer ()
{
    if (_has_handshake_timer) {
        cancel_timer (handshake_timer_id);
        _has_handshake_timer = false;
    }
}

void zmq::asio_engine_t::add_timer (int timeout_, int id_)
{
    ENGINE_DBG ("add_timer: timeout=%d, id=%d", timeout_, id_);

    zmq_assert (_timer);
    _current_timer_id = id_;
    _timer->expires_after (std::chrono::milliseconds (timeout_));
    _timer->async_wait ([this, id_] (const boost::system::error_code &ec) {
        on_timer (id_, ec);
    });
}

void zmq::asio_engine_t::cancel_timer (int id_)
{
    ENGINE_DBG ("cancel_timer: id=%d", id_);

    if (_current_timer_id == id_ && _timer) {
        _timer->cancel ();
        _current_timer_id = -1;
    }
}

void zmq::asio_engine_t::on_timer (int id_, const boost::system::error_code &ec)
{
    ENGINE_DBG ("on_timer: id=%d, ec=%s, terminating=%d", id_,
                ec.message ().c_str (), _terminating);

    if (ec == boost::asio::error::operation_aborted)
        return;

    //  If terminating, just return - terminate() is draining handlers
    if (_terminating)
        return;

    if (!_plugged)
        return;

    if (id_ == handshake_timer_id) {
        _has_handshake_timer = false;
        //  handshake timer expired before handshake completed, so engine fail
        error (timeout_error);
    } else if (id_ == heartbeat_ivl_timer_id) {
        _next_msg = &asio_engine_t::produce_ping_message;
        restart_output ();
        add_timer (_options.heartbeat_interval, heartbeat_ivl_timer_id);
    } else if (id_ == heartbeat_ttl_timer_id) {
        _has_ttl_timer = false;
        error (timeout_error);
    } else if (id_ == heartbeat_timeout_timer_id) {
        _has_timeout_timer = false;
        error (timeout_error);
    }
}

bool zmq::asio_engine_t::init_properties (properties_t &properties_)
{
    if (_peer_address.empty ())
        return false;
    properties_.ZMQ_MAP_INSERT_OR_EMPLACE (
      std::string (ZMQ_MSG_PROPERTY_PEER_ADDRESS), _peer_address);

    //  Private property to support deprecated SRCFD
    std::ostringstream stream;
    stream << static_cast<int> (_fd);
    std::string fd_string = stream.str ();
    properties_.ZMQ_MAP_INSERT_OR_EMPLACE (std::string ("__fd"),
                                           ZMQ_MOVE (fd_string));
    return true;
}

#endif  // ZMQ_IOTHREAD_POLLER_USE_ASIO
