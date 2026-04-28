/**
 * Aether Standard Library Test Framework
 * Common utilities and macros for testing
 */

#ifndef AETHER_TEST_FRAMEWORK_H
#define AETHER_TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// ANSI color codes for cross-platform colored output
#ifdef _WIN32
    #define COLOR_RESET   ""
    #define COLOR_GREEN   ""
    #define COLOR_RED     ""
    #define COLOR_YELLOW  ""
    #define COLOR_CYAN    ""
#else
    #define COLOR_RESET   "\033[0m"
    #define COLOR_GREEN   "\033[32m"
    #define COLOR_RED     "\033[31m"
    #define COLOR_YELLOW  "\033[33m"
    #define COLOR_CYAN    "\033[36m"
#endif

// Test result tracking
typedef struct {
    int total;
    int passed;
    int failed;
    const char* suite_name;
} TestResults;

// Global test results
static TestResults g_test_results = {0, 0, 0, NULL};

// Initialize test suite
#define TEST_SUITE_BEGIN(name) \
    do { \
        g_test_results.suite_name = name; \
        g_test_results.total = 0; \
        g_test_results.passed = 0; \
        g_test_results.failed = 0; \
        printf("\n%s=== %s ===%s\n\n", COLOR_CYAN, name, COLOR_RESET); \
    } while(0)

// Test assertion with automatic pass/fail tracking
#define TEST(name, condition) \
    do { \
        g_test_results.total++; \
        printf("%s... ", name); \
        fflush(stdout); \
        if (condition) { \
            g_test_results.passed++; \
            printf("%sOK%s\n", COLOR_GREEN, COLOR_RESET); \
        } else { \
            g_test_results.failed++; \
            printf("%sFAILED%s\n", COLOR_RED, COLOR_RESET); \
            return 1; \
        } \
    } while(0)

// Test with custom error message
#define TEST_MSG(name, condition, msg) \
    do { \
        g_test_results.total++; \
        printf("%s... ", name); \
        fflush(stdout); \
        if (condition) { \
            g_test_results.passed++; \
            printf("%sOK%s\n", COLOR_GREEN, COLOR_RESET); \
        } else { \
            g_test_results.failed++; \
            printf("%sFAILED%s: %s\n", COLOR_RED, COLOR_RESET, msg); \
            return 1; \
        } \
    } while(0)

// End test suite and print results
#define TEST_SUITE_END() \
    do { \
        printf("\n%s====================", COLOR_CYAN); \
        if (g_test_results.failed == 0) { \
            printf("\n%sAll %d tests passed!%s\n", COLOR_GREEN, \
                   g_test_results.passed, COLOR_RESET); \
        } else { \
            printf("\n%s%d/%d tests passed, %d failed%s\n", COLOR_YELLOW, \
                   g_test_results.passed, g_test_results.total, \
                   g_test_results.failed, COLOR_RESET); \
        } \
        printf("%s====================%s\n\n", COLOR_CYAN, COLOR_RESET); \
        return g_test_results.failed > 0 ? 1 : 0; \
    } while(0)

// Common test helper functions

// Integer comparison for generic collections
static inline bool test_int_equals(const void* a, const void* b) {
    return *(int*)a == *(int*)b;
}

// Integer hash function
static inline uint64_t test_int_hash(const void* key) {
    return (uint64_t)(*(int*)key);
}

// Integer clone function
static inline void* test_int_clone(const void* val) {
    int* c = malloc(sizeof(int));
    if (c) *c = *(int*)val;
    return c;
}

// String hash function (djb2)
static inline uint64_t test_str_hash(const void* key) {
    const char* str = (const char*)key;
    uint64_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

// String comparison
static inline bool test_str_equals(const void* a, const void* b) {
    return strcmp((const char*)a, (const char*)b) == 0;
}

// Helper to create heap-allocated integer
static inline int* test_make_int(int value) {
    int* ptr = malloc(sizeof(int));
    if (ptr) *ptr = value;
    return ptr;
}

#endif // AETHER_TEST_FRAMEWORK_H
