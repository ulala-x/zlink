/* SPDX-License-Identifier: MPL-2.0 */

#ifndef __ZMQ_DEBUG_H_INCLUDED__
#define __ZMQ_DEBUG_H_INCLUDED__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Debug counters - only active when ZMQ_DEBUG_COUNTERS is defined
// These are for testing purposes only, not part of the public API

#if defined(ZMQ_DEBUG_COUNTERS)

// Get zero-copy write count
size_t zmq_debug_get_zerocopy_count();

// Get fallback (copy) write count
size_t zmq_debug_get_fallback_count();

// Get total bytes copied (should be 0 for large messages if zero-copy works)
size_t zmq_debug_get_bytes_copied();

// Get scatter-gather write count
size_t zmq_debug_get_scatter_gather_count();

// Get partial write count
size_t zmq_debug_get_partial_write_count();

// Get read buffer compaction count
size_t zmq_debug_get_compaction_count();

// Get read buffer compaction skipped count
size_t zmq_debug_get_compaction_skipped_count();

// Get bytes moved during compaction
size_t zmq_debug_get_compaction_bytes();

// Get handshake input copies from external buffer
size_t zmq_debug_get_handshake_copy_count();

// Get bytes copied during handshake input handling
size_t zmq_debug_get_handshake_copy_bytes();

// Get batch flush count
size_t zmq_debug_get_batch_flush_count();

// Get batch flushes triggered by timer
size_t zmq_debug_get_batch_timeout_flush_count();

// Get batch flushes triggered by size threshold
size_t zmq_debug_get_batch_size_flush_count();

// Get batch flushes triggered by message count threshold
size_t zmq_debug_get_batch_count_flush_count();

// Get total messages flushed in batches
size_t zmq_debug_get_batch_messages_total();

// Get total bytes flushed in batches
size_t zmq_debug_get_batch_bytes_total();

// Reset all counters
void zmq_debug_reset_counters();

// Increment functions (internal use)
void zmq_debug_inc_zerocopy_count();
void zmq_debug_inc_fallback_count();
void zmq_debug_add_bytes_copied(size_t bytes);
void zmq_debug_inc_scatter_gather_count();
void zmq_debug_inc_partial_write_count();
void zmq_debug_inc_compaction_count();
void zmq_debug_inc_compaction_skipped_count();
void zmq_debug_add_compaction_bytes(size_t bytes);
void zmq_debug_inc_handshake_copy_count();
void zmq_debug_add_handshake_copy_bytes(size_t bytes);
void zmq_debug_inc_batch_flush_count();
void zmq_debug_inc_batch_timeout_flush_count();
void zmq_debug_inc_batch_size_flush_count();
void zmq_debug_inc_batch_count_flush_count();
void zmq_debug_add_batch_messages(size_t messages);
void zmq_debug_add_batch_bytes(size_t bytes);

#else

// Stub macros when counters are disabled
#define zmq_debug_get_zerocopy_count() 0
#define zmq_debug_get_fallback_count() 0
#define zmq_debug_get_bytes_copied() 0
#define zmq_debug_get_scatter_gather_count() 0
#define zmq_debug_get_partial_write_count() 0
#define zmq_debug_get_compaction_count() 0
#define zmq_debug_get_compaction_skipped_count() 0
#define zmq_debug_get_compaction_bytes() 0
#define zmq_debug_get_handshake_copy_count() 0
#define zmq_debug_get_handshake_copy_bytes() 0
#define zmq_debug_get_batch_flush_count() 0
#define zmq_debug_get_batch_timeout_flush_count() 0
#define zmq_debug_get_batch_size_flush_count() 0
#define zmq_debug_get_batch_count_flush_count() 0
#define zmq_debug_get_batch_messages_total() 0
#define zmq_debug_get_batch_bytes_total() 0
#define zmq_debug_reset_counters() ((void)0)
#define zmq_debug_inc_zerocopy_count() ((void)0)
#define zmq_debug_inc_fallback_count() ((void)0)
#define zmq_debug_add_bytes_copied(x) ((void)0)
#define zmq_debug_inc_scatter_gather_count() ((void)0)
#define zmq_debug_inc_partial_write_count() ((void)0)
#define zmq_debug_inc_compaction_count() ((void)0)
#define zmq_debug_inc_compaction_skipped_count() ((void)0)
#define zmq_debug_add_compaction_bytes(x) ((void)0)
#define zmq_debug_inc_handshake_copy_count() ((void)0)
#define zmq_debug_add_handshake_copy_bytes(x) ((void)0)
#define zmq_debug_inc_batch_flush_count() ((void)0)
#define zmq_debug_inc_batch_timeout_flush_count() ((void)0)
#define zmq_debug_inc_batch_size_flush_count() ((void)0)
#define zmq_debug_inc_batch_count_flush_count() ((void)0)
#define zmq_debug_add_batch_messages(x) ((void)0)
#define zmq_debug_add_batch_bytes(x) ((void)0)

#endif // ZMQ_DEBUG_COUNTERS

#ifdef __cplusplus
}
#endif

#endif // __ZMQ_DEBUG_H_INCLUDED__
