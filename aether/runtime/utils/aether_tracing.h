#ifndef AETHER_TRACING_H
#define AETHER_TRACING_H

#include "../actors/actor_state_machine.h"
#include <stdio.h>

// Message trace entry
typedef struct {
    int actor_id;
    int message_type;
    int message_payload;
    int from_actor_id;
    int to_actor_id;
    double timestamp;
    const char* event_type;  // "received", "sent", "processed"
} TraceEntry;

// Tracing configuration
typedef struct {
    FILE* log_file;
    int enabled;
    int trace_all;
    int* traced_actor_ids;
    int traced_actor_count;
    double start_time;
} TracingConfig;

// Global tracing config
extern TracingConfig global_tracing;

// Tracing control
void tracing_init(const char* log_filename);
void tracing_shutdown();
void tracing_enable();
void tracing_disable();

// Actor-specific tracing
void tracing_add_actor(int actor_id);
void tracing_remove_actor(int actor_id);
void tracing_set_trace_all(int enabled);

// Event logging
void tracing_log_received(int actor_id, int msg_type, int msg_payload);
void tracing_log_sent(int from_actor_id, int to_actor_id, int msg_type, int msg_payload);
void tracing_log_processed(int actor_id, int msg_type, int msg_payload);
void tracing_log_custom(int actor_id, const char* event);

// Helper
int tracing_should_trace(int actor_id);
double tracing_get_elapsed_time();

#endif // AETHER_TRACING_H

