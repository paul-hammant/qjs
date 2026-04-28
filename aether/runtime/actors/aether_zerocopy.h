// Zero-Copy Message Passing System
// Eliminates memcpy() overhead by transferring ownership

#ifndef AETHER_ZEROCOPY_H
#define AETHER_ZEROCOPY_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

// Message ownership flags
#define MSG_OWNED    0x01  // Message owns its data (must free)
#define MSG_BORROWED 0x02  // Message borrows data (don't free)
#define MSG_INLINE   0x04  // Data is inline (no allocation)

// Zero-copy message envelope
typedef struct {
    void* data;          // Pointer to message data
    size_t size;         // Size of message data
    uint8_t flags;       // Ownership flags
    uint8_t _padding[7]; // Align to 16 bytes
} ZeroCopyMessage;

// Initialize zero-copy message with owned data
static inline void zerocopy_init_owned(ZeroCopyMessage* msg, void* data, size_t size) {
    msg->data = data;
    msg->size = size;
    msg->flags = MSG_OWNED;
}

// Initialize zero-copy message with borrowed data
static inline void zerocopy_init_borrowed(ZeroCopyMessage* msg, void* data, size_t size) {
    msg->data = data;
    msg->size = size;
    msg->flags = MSG_BORROWED;
}

// Transfer ownership (move semantics)
static inline void zerocopy_transfer(ZeroCopyMessage* dest, ZeroCopyMessage* src) {
    dest->data = src->data;
    dest->size = src->size;
    dest->flags = src->flags;
    
    // Source no longer owns the data
    src->data = NULL;
    src->size = 0;
    src->flags = 0;
}

// Clone message (allocates new buffer)
static inline ZeroCopyMessage zerocopy_clone(const ZeroCopyMessage* msg) {
    ZeroCopyMessage cloned;
    cloned.data = malloc(msg->size);
    cloned.size = msg->size;
    cloned.flags = MSG_OWNED;
    
    if (cloned.data) {
        memcpy(cloned.data, msg->data, msg->size);
    }
    
    return cloned;
}

// Free message if owned
static inline void zerocopy_free(ZeroCopyMessage* msg) {
    if (msg->flags & MSG_OWNED) {
        free(msg->data);
    }
    msg->data = NULL;
    msg->size = 0;
    msg->flags = 0;
}

// Check if message is valid
static inline int zerocopy_is_valid(const ZeroCopyMessage* msg) {
    return msg->data != NULL && msg->size > 0;
}

// Get message type (assumes first field is message_id)
static inline int zerocopy_get_type(const ZeroCopyMessage* msg) {
    if (!zerocopy_is_valid(msg)) return -1;
    return *(int*)msg->data;
}

#endif // AETHER_ZEROCOPY_H
