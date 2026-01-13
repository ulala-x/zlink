/* SPDX-License-Identifier: MPL-2.0 */

#include "zmq_debug.h"

#if defined(ZMQ_DEBUG_COUNTERS)

#include <atomic>

static std::atomic<size_t> s_zerocopy_count{0};
static std::atomic<size_t> s_fallback_count{0};
static std::atomic<size_t> s_bytes_copied{0};
static std::atomic<size_t> s_scatter_gather_count{0};
static std::atomic<size_t> s_partial_write_count{0};
static std::atomic<size_t> s_compaction_count{0};
static std::atomic<size_t> s_compaction_skipped_count{0};
static std::atomic<size_t> s_compaction_bytes{0};
static std::atomic<size_t> s_handshake_copy_count{0};
static std::atomic<size_t> s_handshake_copy_bytes{0};
static std::atomic<size_t> s_batch_flush_count{0};
static std::atomic<size_t> s_batch_timeout_flush_count{0};
static std::atomic<size_t> s_batch_size_flush_count{0};
static std::atomic<size_t> s_batch_count_flush_count{0};
static std::atomic<size_t> s_batch_messages_total{0};
static std::atomic<size_t> s_batch_bytes_total{0};

extern "C" {

size_t zmq_debug_get_zerocopy_count()
{
    return s_zerocopy_count.load(std::memory_order_relaxed);
}

size_t zmq_debug_get_fallback_count()
{
    return s_fallback_count.load(std::memory_order_relaxed);
}

size_t zmq_debug_get_bytes_copied()
{
    return s_bytes_copied.load(std::memory_order_relaxed);
}

size_t zmq_debug_get_scatter_gather_count()
{
    return s_scatter_gather_count.load(std::memory_order_relaxed);
}

size_t zmq_debug_get_partial_write_count()
{
    return s_partial_write_count.load(std::memory_order_relaxed);
}

size_t zmq_debug_get_compaction_count()
{
    return s_compaction_count.load(std::memory_order_relaxed);
}

size_t zmq_debug_get_compaction_skipped_count()
{
    return s_compaction_skipped_count.load(std::memory_order_relaxed);
}

size_t zmq_debug_get_compaction_bytes()
{
    return s_compaction_bytes.load(std::memory_order_relaxed);
}

size_t zmq_debug_get_handshake_copy_count()
{
    return s_handshake_copy_count.load(std::memory_order_relaxed);
}

size_t zmq_debug_get_handshake_copy_bytes()
{
    return s_handshake_copy_bytes.load(std::memory_order_relaxed);
}

size_t zmq_debug_get_batch_flush_count()
{
    return s_batch_flush_count.load(std::memory_order_relaxed);
}

size_t zmq_debug_get_batch_timeout_flush_count()
{
    return s_batch_timeout_flush_count.load(std::memory_order_relaxed);
}

size_t zmq_debug_get_batch_size_flush_count()
{
    return s_batch_size_flush_count.load(std::memory_order_relaxed);
}

size_t zmq_debug_get_batch_count_flush_count()
{
    return s_batch_count_flush_count.load(std::memory_order_relaxed);
}

size_t zmq_debug_get_batch_messages_total()
{
    return s_batch_messages_total.load(std::memory_order_relaxed);
}

size_t zmq_debug_get_batch_bytes_total()
{
    return s_batch_bytes_total.load(std::memory_order_relaxed);
}

void zmq_debug_reset_counters()
{
    s_zerocopy_count.store(0, std::memory_order_relaxed);
    s_fallback_count.store(0, std::memory_order_relaxed);
    s_bytes_copied.store(0, std::memory_order_relaxed);
    s_scatter_gather_count.store(0, std::memory_order_relaxed);
    s_partial_write_count.store(0, std::memory_order_relaxed);
    s_compaction_count.store(0, std::memory_order_relaxed);
    s_compaction_skipped_count.store(0, std::memory_order_relaxed);
    s_compaction_bytes.store(0, std::memory_order_relaxed);
    s_handshake_copy_count.store(0, std::memory_order_relaxed);
    s_handshake_copy_bytes.store(0, std::memory_order_relaxed);
    s_batch_flush_count.store(0, std::memory_order_relaxed);
    s_batch_timeout_flush_count.store(0, std::memory_order_relaxed);
    s_batch_size_flush_count.store(0, std::memory_order_relaxed);
    s_batch_count_flush_count.store(0, std::memory_order_relaxed);
    s_batch_messages_total.store(0, std::memory_order_relaxed);
    s_batch_bytes_total.store(0, std::memory_order_relaxed);
}

void zmq_debug_inc_zerocopy_count()
{
    s_zerocopy_count.fetch_add(1, std::memory_order_relaxed);
}

void zmq_debug_inc_fallback_count()
{
    s_fallback_count.fetch_add(1, std::memory_order_relaxed);
}

void zmq_debug_add_bytes_copied(size_t bytes)
{
    s_bytes_copied.fetch_add(bytes, std::memory_order_relaxed);
}

void zmq_debug_inc_scatter_gather_count()
{
    s_scatter_gather_count.fetch_add(1, std::memory_order_relaxed);
}

void zmq_debug_inc_partial_write_count()
{
    s_partial_write_count.fetch_add(1, std::memory_order_relaxed);
}

void zmq_debug_inc_compaction_count()
{
    s_compaction_count.fetch_add(1, std::memory_order_relaxed);
}

void zmq_debug_inc_compaction_skipped_count()
{
    s_compaction_skipped_count.fetch_add(1, std::memory_order_relaxed);
}

void zmq_debug_add_compaction_bytes(size_t bytes)
{
    s_compaction_bytes.fetch_add(bytes, std::memory_order_relaxed);
}

void zmq_debug_inc_handshake_copy_count()
{
    s_handshake_copy_count.fetch_add(1, std::memory_order_relaxed);
}

void zmq_debug_add_handshake_copy_bytes(size_t bytes)
{
    s_handshake_copy_bytes.fetch_add(bytes, std::memory_order_relaxed);
}

void zmq_debug_inc_batch_flush_count()
{
    s_batch_flush_count.fetch_add(1, std::memory_order_relaxed);
}

void zmq_debug_inc_batch_timeout_flush_count()
{
    s_batch_timeout_flush_count.fetch_add(1, std::memory_order_relaxed);
}

void zmq_debug_inc_batch_size_flush_count()
{
    s_batch_size_flush_count.fetch_add(1, std::memory_order_relaxed);
}

void zmq_debug_inc_batch_count_flush_count()
{
    s_batch_count_flush_count.fetch_add(1, std::memory_order_relaxed);
}

void zmq_debug_add_batch_messages(size_t messages)
{
    s_batch_messages_total.fetch_add(messages, std::memory_order_relaxed);
}

void zmq_debug_add_batch_bytes(size_t bytes)
{
    s_batch_bytes_total.fetch_add(bytes, std::memory_order_relaxed);
}

} // extern "C"

#endif // ZMQ_DEBUG_COUNTERS
