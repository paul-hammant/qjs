/**
 * Micro-Profiling Utilities for Aether Benchmarks
 * 
 * Provides cycle-accurate timing and performance counters
 * without needing external profiling tools.
 */

#ifndef MICRO_PROFILE_H
#define MICRO_PROFILE_H

#include <stdint.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#include <intrin.h>
#pragma intrinsic(__rdtsc)
#else
#include <x86intrin.h>
#endif

// ============================================================================
// High-Resolution Timing
// ============================================================================

typedef struct {
    uint64_t start_cycles;
    uint64_t end_cycles;
    uint64_t total_cycles;
    uint64_t count;
} CycleTimer;

// Read CPU cycle counter (RDTSC instruction)
static inline uint64_t read_cycles(void) {
    return __rdtsc();
}

// QueryPerformanceCounter for nanosecond precision
static inline uint64_t read_nanoseconds(void) {
#ifdef _WIN32
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    return (counter.QuadPart * 1000000000ULL) / freq.QuadPart;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000ULL + ts.tv_nsec;
#endif
}

// ============================================================================
// Profiling Regions
// ============================================================================

static inline void profile_start(CycleTimer* timer) {
    timer->start_cycles = read_cycles();
}

static inline void profile_end(CycleTimer* timer) {
    timer->end_cycles = read_cycles();
    uint64_t elapsed = timer->end_cycles - timer->start_cycles;
    timer->total_cycles += elapsed;
    timer->count++;
}

static inline void profile_reset(CycleTimer* timer) {
    timer->start_cycles = 0;
    timer->end_cycles = 0;
    timer->total_cycles = 0;
    timer->count = 0;
}

static inline double profile_avg_cycles(const CycleTimer* timer) {
    return timer->count > 0 ? (double)timer->total_cycles / timer->count : 0.0;
}

static inline void profile_print(const char* name, const CycleTimer* timer) {
    printf("  %-30s: %12llu cycles (avg: %.2f cycles/op over %llu ops)\n",
           name, 
           (unsigned long long)timer->total_cycles,
           profile_avg_cycles(timer),
           (unsigned long long)timer->count);
}

// ============================================================================
// Overhead Measurement
// ============================================================================

typedef struct {
    CycleTimer with_atomic;
    CycleTimer without_atomic;
    CycleTimer mailbox_send;
    CycleTimer mailbox_receive;
    CycleTimer message_copy;
} MicroBenchResults;

// Measure atomic operation overhead
static inline void bench_atomic_overhead(MicroBenchResults* results, int iterations) {
    volatile int counter = 0;
    _Atomic int atomic_counter = 0;
    
    // Measure non-atomic increment
    profile_reset(&results->without_atomic);
    profile_start(&results->without_atomic);
    for (int i = 0; i < iterations; i++) {
        counter++;
    }
    profile_end(&results->without_atomic);
    
    // Measure atomic increment
    profile_reset(&results->with_atomic);
    profile_start(&results->with_atomic);
    for (int i = 0; i < iterations; i++) {
        atomic_fetch_add_explicit(&atomic_counter, 1, memory_order_relaxed);
    }
    profile_end(&results->with_atomic);
}

// Print overhead comparison
static inline void print_micro_bench_results(const MicroBenchResults* results) {
    printf("\n=== Micro-Benchmark Results ===\n");
    
    if (results->with_atomic.count > 0 && results->without_atomic.count > 0) {
        double atomic_cycles = profile_avg_cycles(&results->with_atomic);
        double plain_cycles = profile_avg_cycles(&results->without_atomic);
        double overhead = atomic_cycles - plain_cycles;
        
        printf("Counter Increment:\n");
        printf("  Plain int:     %.2f cycles/op\n", plain_cycles);
        printf("  Atomic int:    %.2f cycles/op\n", atomic_cycles);
        printf("  Overhead:      %.2f cycles/op (%.1fx slower)\n", 
               overhead, atomic_cycles / plain_cycles);
    }
    
    if (results->mailbox_send.count > 0) {
        profile_print("Mailbox Send", &results->mailbox_send);
    }
    
    if (results->mailbox_receive.count > 0) {
        profile_print("Mailbox Receive", &results->mailbox_receive);
    }
    
    if (results->message_copy.count > 0) {
        profile_print("Message Copy", &results->message_copy);
    }
}

// ============================================================================
// Hot Path Markers (for manual instrumentation)
// ============================================================================

#define PROFILE_REGION_START(timer) profile_start(&timer)
#define PROFILE_REGION_END(timer) profile_end(&timer)

// Compile-time option to disable profiling in production
#ifdef AETHER_DISABLE_PROFILING
#undef PROFILE_REGION_START
#undef PROFILE_REGION_END
#define PROFILE_REGION_START(timer) ((void)0)
#define PROFILE_REGION_END(timer) ((void)0)
#endif

// ============================================================================
// Cache Miss Estimation (Windows only, requires admin)
// ============================================================================

#ifdef _WIN32
// Note: Requires enabling Performance Counters in Windows
// See: https://docs.microsoft.com/en-us/windows/win32/perfctrs/performance-counters-portal
static inline void print_cache_info(void) {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    printf("\nSystem Info:\n");
    printf("  Processors: %lu\n", si.dwNumberOfProcessors);
    printf("  Page Size: %lu bytes\n", si.dwPageSize);
    printf("  Cache Line: 64 bytes (assumed)\n");
}
#endif

#endif // MICRO_PROFILE_H
