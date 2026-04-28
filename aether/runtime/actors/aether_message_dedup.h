// CRAZY OPTIMIZATION #3: Message Deduplication
// Skip redundant messages = huge win for certain patterns

#ifndef AETHER_MESSAGE_DEDUP_H
#define AETHER_MESSAGE_DEDUP_H

#include <stdint.h>
#include <string.h>
#include "actor_state_machine.h"

#define DEDUP_WINDOW_SIZE 16
#define DEDUP_WINDOW_MASK (DEDUP_WINDOW_SIZE - 1)

// Message fingerprint for deduplication
typedef struct {
    uint32_t type_hash;
    uint32_t payload_hash;
} MessageFingerprint;

// Deduplication window (last N messages)
typedef struct {
    MessageFingerprint window[DEDUP_WINDOW_SIZE];
    int write_index;
} DedupWindow;

// Fast hash for message type + payload
static inline uint32_t hash_message(Message* msg) {
    uint32_t hash = msg->type * 2654435761u;  // Knuth's multiplicative hash
    hash ^= msg->payload_int * 0x9e3779b1u;
    return hash;
}

// Create fingerprint
static inline MessageFingerprint message_fingerprint(Message* msg) {
    MessageFingerprint fp;
    fp.type_hash = msg->type * 2654435761u;
    fp.payload_hash = hash_message(msg);
    return fp;
}

// Check if message is duplicate
static inline int is_duplicate(DedupWindow* window, Message* msg) {
    MessageFingerprint fp = message_fingerprint(msg);
    
    // Check last N messages
    for (int i = 0; i < DEDUP_WINDOW_SIZE; i++) {
        if (window->window[i].type_hash == fp.type_hash &&
            window->window[i].payload_hash == fp.payload_hash) {
            return 1;  // Duplicate found
        }
    }
    
    return 0;  // Not duplicate
}

// Record message in dedup window
static inline void record_message(DedupWindow* window, Message* msg) {
    MessageFingerprint fp = message_fingerprint(msg);
    int idx = window->write_index & DEDUP_WINDOW_MASK;
    window->window[idx] = fp;
    window->write_index++;
}

// Send with deduplication
static inline int send_deduplicated(
    Mailbox* mailbox, 
    DedupWindow* window, 
    Message msg
) {
    // Check for duplicate
    if (is_duplicate(window, &msg)) {
        return 1;  // Skip duplicate, report success
    }
    
    // Record and send
    record_message(window, &msg);
    return mailbox_send(mailbox, msg);
}

#endif // AETHER_MESSAGE_DEDUP_H
