// CRAZY OPTIMIZATION #5: Adaptive Batch Size
// Dynamically adjust batch size based on load

#ifndef AETHER_ADAPTIVE_BATCH_H
#define AETHER_ADAPTIVE_BATCH_H

#include <stdatomic.h>
#include "actor_state_machine.h"

#define MIN_BATCH_SIZE 64
#define MAX_BATCH_SIZE 1024

// Per-actor adaptive batching state
typedef struct {
    int current_batch_size;
    int consecutive_full_batches;
    int consecutive_partial_batches;
    uint64_t total_messages_processed;
    uint64_t last_adjustment_time;
} AdaptiveBatchState;

// Initialize adaptive state
static inline void adaptive_batch_init(AdaptiveBatchState* state) {
    state->current_batch_size = 128;  // Start high for multicore
    state->consecutive_full_batches = 0;
    state->consecutive_partial_batches = 0;
    state->total_messages_processed = 0;
    state->last_adjustment_time = 0;
}

// Adjust batch size based on utilization
static inline void adaptive_batch_adjust(
    AdaptiveBatchState* state, 
    int messages_received
) {
    state->total_messages_processed += messages_received;
    
    // Check if batch was full or partial
    if (messages_received >= state->current_batch_size) {
        state->consecutive_full_batches++;
        state->consecutive_partial_batches = 0;
        
        // If consistently full, increase batch size
        if (state->consecutive_full_batches >= 5 && 
            state->current_batch_size < MAX_BATCH_SIZE) {
            state->current_batch_size *= 2;
            if (state->current_batch_size > MAX_BATCH_SIZE) {
                state->current_batch_size = MAX_BATCH_SIZE;
            }
            state->consecutive_full_batches = 0;
        }
    } else {
        state->consecutive_partial_batches++;
        state->consecutive_full_batches = 0;
        
        // If consistently partial, decrease batch size
        if (state->consecutive_partial_batches >= 10 && 
            state->current_batch_size > MIN_BATCH_SIZE) {
            state->current_batch_size /= 2;
            if (state->current_batch_size < MIN_BATCH_SIZE) {
                state->current_batch_size = MIN_BATCH_SIZE;
            }
            state->consecutive_partial_batches = 0;
        }
    }
}

// Receive with adaptive batching
static inline int adaptive_batch_receive(
    AdaptiveBatchState* state,
    Mailbox* mbox,
    Message* out_msgs,
    int max_msgs
) {
    // Use current adaptive batch size
    int batch_size = state->current_batch_size;
    if (batch_size > max_msgs) {
        batch_size = max_msgs;
    }
    
    // Receive batch (or single messages if no batch function)
    int received = 0;
    while (received < batch_size && atomic_load_explicit(&mbox->count, memory_order_relaxed) > 0) {
        if (!mailbox_receive(mbox, &out_msgs[received])) {
            break;
        }
        received++;
    }
    
    // Adjust batch size based on result
    adaptive_batch_adjust(state, received);
    
    return received;
}

#endif // AETHER_ADAPTIVE_BATCH_H
