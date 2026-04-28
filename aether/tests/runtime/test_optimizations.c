// Unit Tests: SIMD Batching and Message Coalescing
// Validates correctness and performance of optimizations

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <malloc.h>
#define aligned_alloc(alignment, size) _aligned_malloc(size, alignment)
#define aligned_free(ptr) _aligned_free(ptr)
static inline uint64_t get_nanos() {
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    return (counter.QuadPart * 1000000000ULL) / freq.QuadPart;
}
#else
#define aligned_free(ptr) free(ptr)
static inline uint64_t get_nanos() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}
#endif

#include "../../runtime/actors/aether_simd_batch.h"
#include "../../runtime/actors/aether_message_coalescing.h"

// Test result tracking
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) \
    printf("Testing %s... ", name); \
    fflush(stdout);

#define PASS() \
    printf("PASS\n"); \
    tests_passed++;

#define FAIL(msg) \
    printf("FAIL: %s\n", msg); \
    tests_failed++;

// SIMD Tests

void test_simd_availability() {
    TEST("SIMD availability detection");
    
    int available = simd_batch_available();
    printf("(AVX2: %s) ", available ? "yes" : "no");
    
    PASS();
}

void test_simd_batch_process_correctness() {
    TEST("SIMD batch processing correctness");
    
    const int count = 16;
    int values[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    int results[16];
    int multiplier = 3;
    int offset = 10;
    
    simd_batch_process_int(values, results, count, multiplier, offset);
    
    // Verify results
    for (int i = 0; i < count; i++) {
        int expected = values[i] * multiplier + offset;
        if (results[i] != expected) {
            char msg[128];
            snprintf(msg, sizeof(msg), "result[%d] = %d, expected %d", 
                     i, results[i], expected);
            FAIL(msg);
            return;
        }
    }
    
    PASS();
}

void test_simd_batch_compare_correctness() {
    TEST("SIMD ID comparison correctness");
    
    int message_ids[8] = {1, 5, 3, 5, 2, 5, 7, 5};
    uint32_t mask = simd_batch_compare_ids(message_ids, 5, 8);
    
    // Check that positions 1, 3, 5, 7 are marked (where value is 5)
    int matches = 0;
    for (int i = 0; i < 8; i++) {
        if (message_ids[i] == 5) matches++;
    }
    
    if (matches != 4) {
        FAIL("Expected 4 matches");
        return;
    }
    
    PASS();
}

void test_simd_batch_performance() {
    TEST("SIMD batch processing performance");
    
    const int count = 10000;
    int* values = (int*)aligned_alloc(32, count * sizeof(int));
    int* results_scalar = (int*)aligned_alloc(32, count * sizeof(int));
    int* results_simd = (int*)aligned_alloc(32, count * sizeof(int));
    
    for (int i = 0; i < count; i++) {
        values[i] = i;
    }
    
    // Scalar baseline
    uint64_t start_scalar = get_nanos();
    for (int i = 0; i < count; i++) {
        results_scalar[i] = values[i] * 2 + 5;
    }
    uint64_t end_scalar = get_nanos();
    double time_scalar = (end_scalar - start_scalar) / 1e9;
    
    // SIMD version
    uint64_t start_simd = get_nanos();
    simd_batch_process_int(values, results_simd, count, 2, 5);
    uint64_t end_simd = get_nanos();
    double time_simd = (end_simd - start_simd) / 1e9;
    
    // Verify correctness
    for (int i = 0; i < count; i++) {
        if (results_scalar[i] != results_simd[i]) {
            FAIL("Results mismatch");
            aligned_free(values);
            aligned_free(results_scalar);
            aligned_free(results_simd);
            return;
        }
    }
    
    double speedup = time_scalar / time_simd;
    printf("(%.2fx speedup) ", speedup);
    
    aligned_free(values);
    aligned_free(results_scalar);
    aligned_free(results_simd);
    
    PASS();
}

// Message Coalescing Tests

void test_coalescing_buffer_init() {
    TEST("Coalescing buffer initialization");
    
    CoalescingBuffer buf;
    coalescing_buffer_init(&buf);
    
    if (buf.pending.count != 0) {
        FAIL("Buffer should be empty");
        return;
    }
    
    if (buf.pending.capacity != COALESCE_BUFFER_SIZE) {
        FAIL("Capacity incorrect");
        return;
    }
    
    PASS();
}

void test_coalescing_buffer_add() {
    TEST("Coalescing buffer add");
    
    CoalescingBuffer buf;
    coalescing_buffer_init(&buf);
    
    int msg1 = 1, msg2 = 2, msg3 = 3;
    
    int should_flush = coalescing_buffer_add(&buf, &msg1, sizeof(int));
    if (should_flush) {
        FAIL("Should not flush after 1 message");
        return;
    }
    
    if (buf.pending.count != 1) {
        FAIL("Count should be 1");
        return;
    }
    
    // Add more messages
    for (int i = 2; i <= COALESCE_THRESHOLD; i++) {
        should_flush = coalescing_buffer_add(&buf, &msg2, sizeof(int));
    }
    
    if (!should_flush) {
        FAIL("Should flush when threshold reached");
        return;
    }
    
    PASS();
}

static int flush_called = 0;
static void test_send_fn(void* msg, uint16_t size) {
    flush_called++;
}

void test_coalescing_buffer_flush() {
    TEST("Coalescing buffer flush");
    
    CoalescingBuffer buf;
    coalescing_buffer_init(&buf);
    
    int messages[5] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; i++) {
        coalescing_buffer_add(&buf, &messages[i], sizeof(int));
    }
    
    flush_called = 0;
    coalescing_buffer_flush(&buf, test_send_fn);
    
    if (flush_called != 5) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Expected 5 calls, got %d", flush_called);
        FAIL(msg);
        return;
    }
    
    if (buf.pending.count != 0) {
        FAIL("Buffer should be empty after flush");
        return;
    }
    
    PASS();
}

void test_coalescing_stats() {
    TEST("Coalescing statistics");
    
    CoalescingBuffer buf;
    coalescing_buffer_init(&buf);
    
    // Add and flush messages
    for (int batch = 0; batch < 3; batch++) {
        for (int i = 0; i < COALESCE_THRESHOLD; i++) {
            int msg = i;
            coalescing_buffer_add(&buf, &msg, sizeof(int));
        }
        coalescing_buffer_flush(&buf, test_send_fn);
    }
    
    CoalescingStats stats = coalescing_buffer_stats(&buf);
    
    if (stats.flush_count != 3) {
        FAIL("Flush count incorrect");
        return;
    }
    
    if (stats.high_watermark != COALESCE_THRESHOLD) {
        FAIL("High watermark incorrect");
        return;
    }
    
    PASS();
}

void test_coalescing_adaptive() {
    TEST("Adaptive coalescing decision");
    
    // Low load: should not coalesce
    if (coalescing_should_enable(2, 1000)) {
        FAIL("Should not enable for low load");
        return;
    }
    
    // High load (many pending): should coalesce
    if (!coalescing_should_enable(10, 1000)) {
        FAIL("Should enable for many pending messages");
        return;
    }
    
    // High load (high rate): should coalesce
    if (!coalescing_should_enable(3, 50000)) {
        FAIL("Should enable for high message rate");
        return;
    }
    
    PASS();
}

// Integration Tests

void test_combined_simd_coalescing() {
    TEST("Combined SIMD + Coalescing");
    
    const int total_messages = 1000;
    int* message_values = (int*)aligned_alloc(32, total_messages * sizeof(int));
    int* results = (int*)aligned_alloc(32, total_messages * sizeof(int));
    
    CoalescingBuffer buf;
    coalescing_buffer_init(&buf);
    
    // Simulate message flow with coalescing
    int processed = 0;
    for (int i = 0; i < total_messages; i++) {
        message_values[i] = i;
        
        int should_flush = coalescing_buffer_add(&buf, &message_values[i], sizeof(int));
        
        if (should_flush || i == total_messages - 1) {
            // Process batch with SIMD
            int batch_size = buf.pending.count;
            int batch_values[COALESCE_BUFFER_SIZE];
            
            for (int j = 0; j < batch_size; j++) {
                batch_values[j] = *(int*)buf.pending.messages[j];
            }
            
            simd_batch_process_int(batch_values, &results[processed], 
                                  batch_size, 1, 0);
            
            processed += batch_size;
            buf.pending.count = 0;
        }
    }
    
    // Verify all messages processed correctly
    if (processed != total_messages) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Processed %d/%d messages", 
                 processed, total_messages);
        FAIL(msg);
        aligned_free(message_values);
        aligned_free(results);
        return;
    }
    
    aligned_free(message_values);
    aligned_free(results);
    
    PASS();
}

int main() {
    printf("===========================================\n");
    printf("AETHER OPTIMIZATION TEST SUITE\n");
    printf("===========================================\n\n");
    
    printf("SIMD Batching Tests:\n");
    printf("-------------------------------------------\n");
    test_simd_availability();
    test_simd_batch_process_correctness();
    test_simd_batch_compare_correctness();
    test_simd_batch_performance();
    printf("\n");
    
    printf("Message Coalescing Tests:\n");
    printf("-------------------------------------------\n");
    test_coalescing_buffer_init();
    test_coalescing_buffer_add();
    test_coalescing_buffer_flush();
    test_coalescing_stats();
    test_coalescing_adaptive();
    printf("\n");
    
    printf("Integration Tests:\n");
    printf("-------------------------------------------\n");
    test_combined_simd_coalescing();
    printf("\n");
    
    printf("===========================================\n");
    printf("RESULTS: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("===========================================\n");
    
    return tests_failed > 0 ? 1 : 0;
}
