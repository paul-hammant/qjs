/**
 * Aether Runtime Profiling Implementation
 */

#include "aether_runtime_profile.h"
#include <string.h>

// Global profiling statistics
ProfileStats g_profile_stats[16] = {0};

void profile_init(void) {
    memset(g_profile_stats, 0, sizeof(g_profile_stats));
}

void profile_reset(void) {
    profile_init();
}

void profile_print_report(int num_cores) {
#ifdef AETHER_PROFILE
    printf("\n");
    printf("===============================================================\n");
    printf("                 Aether Runtime Profile Report\n");
    printf("===============================================================\n");
    
    for (int core = 0; core < num_cores; core++) {
        ProfileStats* stats = &g_profile_stats[core];
        
        uint64_t mailbox_send_count = atomic_load(&stats->mailbox_send_count);
        uint64_t mailbox_recv_count = atomic_load(&stats->mailbox_receive_count);
        uint64_t batch_send_count = atomic_load(&stats->batch_send_count);
        uint64_t batch_recv_count = atomic_load(&stats->batch_receive_count);
        uint64_t actor_step_count = atomic_load(&stats->actor_step_count);
        
        if (mailbox_send_count == 0 && mailbox_recv_count == 0 && 
            batch_send_count == 0 && batch_recv_count == 0) {
            continue;  // Skip inactive cores
        }
        
        printf("\n--- Core %d ---\n", core);
        
        // Mailbox operations
        if (mailbox_send_count > 0) {
            uint64_t cycles = atomic_load(&stats->mailbox_send_cycles);
            printf("  Mailbox Send:    %12llu ops, %8.2f cycles/op\n",
                   (unsigned long long)mailbox_send_count,
                   (double)cycles / mailbox_send_count);
        }
        
        if (mailbox_recv_count > 0) {
            uint64_t cycles = atomic_load(&stats->mailbox_receive_cycles);
            printf("  Mailbox Receive: %12llu ops, %8.2f cycles/op\n",
                   (unsigned long long)mailbox_recv_count,
                   (double)cycles / mailbox_recv_count);
        }
        
        // Batch operations
        if (batch_send_count > 0) {
            uint64_t cycles = atomic_load(&stats->batch_send_cycles);
            printf("  Batch Send:      %12llu msgs, %8.2f cycles/msg\n",
                   (unsigned long long)batch_send_count,
                   (double)cycles / batch_send_count);
        }
        
        if (batch_recv_count > 0) {
            uint64_t cycles = atomic_load(&stats->batch_receive_cycles);
            printf("  Batch Receive:   %12llu msgs, %8.2f cycles/msg\n",
                   (unsigned long long)batch_recv_count,
                   (double)cycles / batch_recv_count);
        }
        
        // SPSC queue
        uint64_t spsc_enq_count = atomic_load(&stats->spsc_enqueue_count);
        uint64_t spsc_deq_count = atomic_load(&stats->spsc_dequeue_count);
        
        if (spsc_enq_count > 0) {
            uint64_t cycles = atomic_load(&stats->spsc_enqueue_cycles);
            printf("  SPSC Enqueue:    %12llu ops, %8.2f cycles/op\n",
                   (unsigned long long)spsc_enq_count,
                   (double)cycles / spsc_enq_count);
        }
        
        if (spsc_deq_count > 0) {
            uint64_t cycles = atomic_load(&stats->spsc_dequeue_cycles);
            printf("  SPSC Dequeue:    %12llu ops, %8.2f cycles/op\n",
                   (unsigned long long)spsc_deq_count,
                   (double)cycles / spsc_deq_count);
        }
        
        // Cross-core queue
        uint64_t queue_enq_count = atomic_load(&stats->queue_enqueue_count);
        uint64_t queue_deq_count = atomic_load(&stats->queue_dequeue_count);
        
        if (queue_enq_count > 0) {
            uint64_t cycles = atomic_load(&stats->queue_enqueue_cycles);
            printf("  Queue Enqueue:   %12llu ops, %8.2f cycles/op\n",
                   (unsigned long long)queue_enq_count,
                   (double)cycles / queue_enq_count);
        }
        
        if (queue_deq_count > 0) {
            uint64_t cycles = atomic_load(&stats->queue_dequeue_cycles);
            printf("  Queue Dequeue:   %12llu ops, %8.2f cycles/op\n",
                   (unsigned long long)queue_deq_count,
                   (double)cycles / queue_deq_count);
        }
        
        // Actor processing
        if (actor_step_count > 0) {
            uint64_t cycles = atomic_load(&stats->actor_step_cycles);
            printf("  Actor Step:      %12llu ops, %8.2f cycles/op\n",
                   (unsigned long long)actor_step_count,
                   (double)cycles / actor_step_count);
        }
        
        // Atomic operations
        uint64_t atomic_ops = atomic_load(&stats->atomic_op_count);
        uint64_t contentions = atomic_load(&stats->lock_contention_count);
        if (atomic_ops > 0) {
            printf("  Atomic Ops:      %12llu\n", (unsigned long long)atomic_ops);
        }
        if (contentions > 0) {
            printf("  Lock Contentions:%12llu (%.2f%%)\n", 
                   (unsigned long long)contentions,
                   100.0 * contentions / atomic_ops);
        }
        
        uint64_t idle = atomic_load(&stats->idle_cycles_total);
        if (idle > 0) {
            printf("  Idle Cycles:     %12llu\n", (unsigned long long)idle);
        }
    }
    
    printf("\n===============================================================\n");
#else
    printf("\nProfiling not enabled. Compile with -DAETHER_PROFILE\n");
#endif
}

void profile_print_summary(int num_cores) {
#ifdef AETHER_PROFILE
    uint64_t total_msgs = 0;
    uint64_t total_cycles = 0;
    uint64_t total_atomic_ops = 0;
    uint64_t total_contentions = 0;
    
    for (int core = 0; core < num_cores; core++) {
        ProfileStats* stats = &g_profile_stats[core];
        total_msgs += atomic_load(&stats->mailbox_send_count);
        total_msgs += atomic_load(&stats->batch_send_count);
        total_cycles += atomic_load(&stats->mailbox_send_cycles);
        total_cycles += atomic_load(&stats->batch_send_cycles);
        total_atomic_ops += atomic_load(&stats->atomic_op_count);
        total_contentions += atomic_load(&stats->lock_contention_count);
    }
    
    printf("\n=== Performance Summary ===\n");
    if (total_msgs > 0) {
        printf("Total Messages:   %llu\n", (unsigned long long)total_msgs);
        printf("Avg Cycles/Msg:   %.2f\n", (double)total_cycles / total_msgs);
        printf("Throughput:       %.2f M msg/sec (at 3 GHz)\n",
               3000.0 * total_msgs / total_cycles);
    }
    if (total_atomic_ops > 0) {
        printf("Atomic Operations: %llu\n", (unsigned long long)total_atomic_ops);
        printf("Lock Contentions:  %llu (%.2f%%)\n",
               (unsigned long long)total_contentions,
               100.0 * total_contentions / total_atomic_ops);
    }
#endif
}

void profile_dump_csv(const char* filename, int num_cores) {
#ifdef AETHER_PROFILE
    FILE* f = fopen(filename, "w");
    if (!f) {
        printf("Failed to open %s for writing\n", filename);
        return;
    }
    
    fprintf(f, "core,operation,count,total_cycles,avg_cycles\n");
    
    for (int core = 0; core < num_cores; core++) {
        ProfileStats* stats = &g_profile_stats[core];
        
        #define DUMP_STAT(name, count_field, cycles_field) do { \
            uint64_t c = atomic_load(&stats->count_field); \
            if (c > 0) { \
                uint64_t cy = atomic_load(&stats->cycles_field); \
                fprintf(f, "%d,%s,%llu,%llu,%.2f\n", core, name, \
                        (unsigned long long)c, (unsigned long long)cy, \
                        (double)cy / c); \
            } \
        } while(0)
        
        DUMP_STAT("mailbox_send", mailbox_send_count, mailbox_send_cycles);
        DUMP_STAT("mailbox_receive", mailbox_receive_count, mailbox_receive_cycles);
        DUMP_STAT("batch_send", batch_send_count, batch_send_cycles);
        DUMP_STAT("batch_receive", batch_receive_count, batch_receive_cycles);
        DUMP_STAT("spsc_enqueue", spsc_enqueue_count, spsc_enqueue_cycles);
        DUMP_STAT("spsc_dequeue", spsc_dequeue_count, spsc_dequeue_cycles);
        DUMP_STAT("queue_enqueue", queue_enqueue_count, queue_enqueue_cycles);
        DUMP_STAT("queue_dequeue", queue_dequeue_count, queue_dequeue_cycles);
        DUMP_STAT("actor_step", actor_step_count, actor_step_cycles);
        
        #undef DUMP_STAT
    }
    
    fclose(f);
    printf("Profile data dumped to %s\n", filename);
#endif
}
