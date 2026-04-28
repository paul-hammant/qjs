// CRAZY OPTIMIZATION #4: Compile-Time Message Specialization
// Generate optimized send/receive for each message type

#ifndef AETHER_MESSAGE_SPECIALIZE_H
#define AETHER_MESSAGE_SPECIALIZE_H

#include "actor_state_machine.h"

// Template for specialized message send (no payload copy)
#define DEFINE_SPECIALIZED_SEND(msg_type, msg_name) \
static inline int send_##msg_name(Mailbox* mbox, int sender_id) { \
    if (unlikely(atomic_load_explicit(&mbox->count, memory_order_relaxed) >= MAILBOX_SIZE)) return 0; \
    Message* slot = &mbox->messages[mbox->tail]; \
    slot->type = msg_type; \
    slot->sender_id = sender_id; \
    slot->payload_int = 0; \
    slot->payload_ptr = NULL; \
    mbox->tail = (mbox->tail + 1) & MAILBOX_MASK; \
    atomic_fetch_add_explicit(&mbox->count, 1, memory_order_release); \
    return 1; \
}

// Template for specialized receive with type check
#define DEFINE_SPECIALIZED_RECEIVE(msg_type, msg_name) \
static inline int receive_##msg_name(Mailbox* mbox, Message* out) { \
    if (unlikely(atomic_load_explicit(&mbox->count, memory_order_acquire) == 0)) return 0; \
    Message* msg = &mbox->messages[mbox->head]; \
    if (msg->type == msg_type) { \
        *out = *msg; \
        mbox->head = (mbox->head + 1) & MAILBOX_MASK; \
        atomic_fetch_sub_explicit(&mbox->count, 1, memory_order_relaxed); \
        return 1; \
    } \
    return 0; \
}

// Template for specialized send with int payload
#define DEFINE_SPECIALIZED_SEND_INT(msg_type, msg_name) \
static inline int send_##msg_name(Mailbox* mbox, int sender_id, int payload) { \
    if (unlikely(atomic_load_explicit(&mbox->count, memory_order_relaxed) >= MAILBOX_SIZE)) return 0; \
    Message* slot = &mbox->messages[mbox->tail]; \
    slot->type = msg_type; \
    slot->sender_id = sender_id; \
    slot->payload_int = payload; \
    slot->payload_ptr = NULL; \
    mbox->tail = (mbox->tail + 1) & MAILBOX_MASK; \
    atomic_fetch_add_explicit(&mbox->count, 1, memory_order_release); \
    return 1; \
}

// Example usage (compiler would generate these):
DEFINE_SPECIALIZED_SEND(MSG_INCREMENT, increment)
DEFINE_SPECIALIZED_SEND(MSG_DECREMENT, decrement)
DEFINE_SPECIALIZED_SEND_INT(MSG_SET_VALUE, set_value)
DEFINE_SPECIALIZED_RECEIVE(MSG_INCREMENT, increment)

// Benefit: Eliminates generic message construction overhead
// - No memset of unused fields
// - No branching on payload type
// - Compiler can inline and optimize
// - Type-safe at compile time

#endif // AETHER_MESSAGE_SPECIALIZE_H
