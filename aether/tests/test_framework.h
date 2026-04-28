/**
 * Aether Unified Test Framework
 * Professional testing infrastructure for all test categories
 * 
 * Usage:
 *   #include "test_framework.h"
 *   
 *   int main(void) {
 *       TEST_SUITE_BEGIN("My Tests");
 *       
 *       TEST("test_name", condition);
 *       ASSERT_EQ(expected, actual);
 *       ASSERT_NOT_NULL(ptr);
 *       
 *       TEST_SUITE_END();
 *   }
 */

#ifndef AETHER_TEST_FRAMEWORK_H
#define AETHER_TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

// ============================================================================
// Cross-Platform Color Support
// ============================================================================

#ifdef _WIN32
    #define COLOR_RESET   ""
    #define COLOR_GREEN   ""
    #define COLOR_RED     ""
    #define COLOR_YELLOW  ""
    #define COLOR_CYAN    ""
    #define COLOR_BLUE    ""
    #define COLOR_MAGENTA ""
#else
    #define COLOR_RESET   "\033[0m"
    #define COLOR_GREEN   "\033[32m"
    #define COLOR_RED     "\033[31m"
    #define COLOR_YELLOW  "\033[33m"
    #define COLOR_CYAN    "\033[36m"
    #define COLOR_BLUE    "\033[34m"
    #define COLOR_MAGENTA "\033[35m"
#endif

// ============================================================================
// Test Result Tracking
// ============================================================================

typedef struct {
    int total;
    int passed;
    int failed;
    const char* suite_name;
    bool verbose;
} TestResults;

static TestResults g_test_results = {0, 0, 0, NULL, false};

// ============================================================================
// Test Suite Macros
// ============================================================================

#define TEST_SUITE_BEGIN(name) \
    do { \
        g_test_results.suite_name = name; \
        g_test_results.total = 0; \
        g_test_results.passed = 0; \
        g_test_results.failed = 0; \
        printf("\n%s=== %s ===%s\n\n", COLOR_CYAN, name, COLOR_RESET); \
    } while(0)

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

// ============================================================================
// Basic Test Macros
// ============================================================================

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
            if (g_test_results.verbose) { \
                fprintf(stderr, "  at %s:%d\n", __FILE__, __LINE__); \
            } \
        } \
    } while(0)

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
            if (g_test_results.verbose) { \
                fprintf(stderr, "  at %s:%d\n", __FILE__, __LINE__); \
            } \
        } \
    } while(0)

// ============================================================================
// Assertion Macros
// ============================================================================

#define ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            g_test_results.failed++; \
            fprintf(stderr, "%sASSERT_TRUE failed%s at %s:%d: %s\n", \
                    COLOR_RED, COLOR_RESET, __FILE__, __LINE__, #condition); \
        } \
    } while (0)

#define ASSERT_FALSE(condition) \
    do { \
        if (condition) { \
            g_test_results.failed++; \
            fprintf(stderr, "%sASSERT_FALSE failed%s at %s:%d: %s\n", \
                    COLOR_RED, COLOR_RESET, __FILE__, __LINE__, #condition); \
        } \
    } while (0)

#define ASSERT_EQ(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            g_test_results.failed++; \
            fprintf(stderr, "%sASSERT_EQ failed%s at %s:%d: expected %ld, got %ld\n", \
                    COLOR_RED, COLOR_RESET, __FILE__, __LINE__, \
                    (long)(expected), (long)(actual)); \
        } \
    } while (0)

#define ASSERT_NE(expected, actual) \
    do { \
        if ((expected) == (actual)) { \
            g_test_results.failed++; \
            fprintf(stderr, "%sASSERT_NE failed%s at %s:%d: both values are %ld\n", \
                    COLOR_RED, COLOR_RESET, __FILE__, __LINE__, (long)(expected)); \
        } \
    } while (0)

#define ASSERT_LT(a, b) \
    do { \
        if ((a) >= (b)) { \
            g_test_results.failed++; \
            fprintf(stderr, "%sASSERT_LT failed%s at %s:%d: %ld >= %ld\n", \
                    COLOR_RED, COLOR_RESET, __FILE__, __LINE__, (long)(a), (long)(b)); \
        } \
    } while (0)

#define ASSERT_LE(a, b) \
    do { \
        if ((a) > (b)) { \
            g_test_results.failed++; \
            fprintf(stderr, "%sASSERT_LE failed%s at %s:%d: %ld > %ld\n", \
                    COLOR_RED, COLOR_RESET, __FILE__, __LINE__, (long)(a), (long)(b)); \
        } \
    } while (0)

#define ASSERT_GT(a, b) \
    do { \
        if ((a) <= (b)) { \
            g_test_results.failed++; \
            fprintf(stderr, "%sASSERT_GT failed%s at %s:%d: %ld <= %ld\n", \
                    COLOR_RED, COLOR_RESET, __FILE__, __LINE__, (long)(a), (long)(b)); \
        } \
    } while (0)

#define ASSERT_GE(a, b) \
    do { \
        if ((a) < (b)) { \
            g_test_results.failed++; \
            fprintf(stderr, "%sASSERT_GE failed%s at %s:%d: %ld < %ld\n", \
                    COLOR_RED, COLOR_RESET, __FILE__, __LINE__, (long)(a), (long)(b)); \
        } \
    } while (0)

#define ASSERT_STREQ(expected, actual) \
    do { \
        if (strcmp((expected), (actual)) != 0) { \
            g_test_results.failed++; \
            fprintf(stderr, "%sASSERT_STREQ failed%s at %s:%d: expected '%s', got '%s'\n", \
                    COLOR_RED, COLOR_RESET, __FILE__, __LINE__, (expected), (actual)); \
        } \
    } while (0)

#define ASSERT_STRNE(expected, actual) \
    do { \
        if (strcmp((expected), (actual)) == 0) { \
            g_test_results.failed++; \
            fprintf(stderr, "%sASSERT_STRNE failed%s at %s:%d: both strings are '%s'\n", \
                    COLOR_RED, COLOR_RESET, __FILE__, __LINE__, (expected)); \
        } \
    } while (0)

#define ASSERT_NULL(ptr) \
    do { \
        if ((ptr) != NULL) { \
            g_test_results.failed++; \
            fprintf(stderr, "%sASSERT_NULL failed%s at %s:%d: pointer is not NULL\n", \
                    COLOR_RED, COLOR_RESET, __FILE__, __LINE__); \
        } \
    } while (0)

#define ASSERT_NOT_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            g_test_results.failed++; \
            fprintf(stderr, "%sASSERT_NOT_NULL failed%s at %s:%d: pointer is NULL\n", \
                    COLOR_RED, COLOR_RESET, __FILE__, __LINE__); \
        } \
    } while (0)

#define ASSERT_FLOAT_EQ(expected, actual, epsilon) \
    do { \
        double _diff = fabs((double)(expected) - (double)(actual)); \
        if (_diff > (epsilon)) { \
            g_test_results.failed++; \
            fprintf(stderr, "%sASSERT_FLOAT_EQ failed%s at %s:%d: expected %f, got %f (diff %f > %f)\n", \
                    COLOR_RED, COLOR_RESET, __FILE__, __LINE__, \
                    (double)(expected), (double)(actual), _diff, (double)(epsilon)); \
        } \
    } while (0)

// ============================================================================
// Memory Testing Macros
// ============================================================================

// Note: Memory leak detection requires runtime/memory.c functions
// This is a placeholder until memory tracking is integrated
#define ASSERT_NO_MEMORY_LEAK(block) \
    do { \
        block; \
        /* Memory leak tracking not yet implemented */ \
    } while (0)

// ============================================================================
// Common Test Helper Functions
// ============================================================================

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

// Helper to create heap-allocated string copy
static inline char* test_make_str(const char* str) {
    char* ptr = malloc(strlen(str) + 1);
    if (ptr) strcpy(ptr, str);
    return ptr;
}

#endif // AETHER_TEST_FRAMEWORK_H
