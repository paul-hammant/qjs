#include "test_harness.h"
#include <stdint.h>
#include <limits.h>

TEST_CATEGORY(int64_size, TEST_CATEGORY_STDLIB) {
    ASSERT_EQ(8, sizeof(int64_t));
    ASSERT_EQ(8, sizeof(uint64_t));
}

TEST_CATEGORY(int64_max_min, TEST_CATEGORY_STDLIB) {
    int64_t max = INT64_MAX;
    int64_t min = INT64_MIN;
    
    ASSERT_TRUE(max > 0);
    ASSERT_TRUE(min < 0);
    ASSERT_TRUE(max > INT32_MAX);
    ASSERT_TRUE(min < INT32_MIN);
}

TEST_CATEGORY(uint64_max, TEST_CATEGORY_STDLIB) {
    uint64_t max = UINT64_MAX;
    ASSERT_TRUE(max > UINT32_MAX);
}

TEST_CATEGORY(int64_arithmetic, TEST_CATEGORY_STDLIB) {
    int64_t a = 1000000000000LL;
    int64_t b = 2000000000000LL;
    int64_t c = a + b;
    
    ASSERT_EQ(3000000000000LL, c);
}

TEST_CATEGORY(uint64_arithmetic, TEST_CATEGORY_STDLIB) {
    uint64_t a = 10000000000ULL;
    uint64_t b = 20000000000ULL;
    uint64_t c = a + b;
    
    ASSERT_EQ(30000000000ULL, c);
}

TEST_CATEGORY(int64_overflow_detection, TEST_CATEGORY_STDLIB) {
    int64_t max = INT64_MAX;
    int64_t near_max = max - 100;
    
    ASSERT_TRUE(near_max > 0);
    ASSERT_TRUE(near_max < max);
}

TEST_CATEGORY(pointer_size_64bit, TEST_CATEGORY_STDLIB) {
    #if defined(__LP64__) || defined(_WIN64)
        ASSERT_EQ(8, sizeof(void*));
        ASSERT_EQ(8, sizeof(size_t));
    #endif
}

TEST_CATEGORY(size_t_large_values, TEST_CATEGORY_STDLIB) {
    size_t large = SIZE_MAX;
    ASSERT_TRUE(large > 0);
    
    #if defined(__LP64__) || defined(_WIN64)
        ASSERT_TRUE(large > UINT32_MAX);
    #endif
}

TEST_CATEGORY(int64_array_indexing, TEST_CATEGORY_STDLIB) {
    int64_t arr[10];
    for (int64_t i = 0; i < 10; i++) {
        arr[i] = i * 1000000000LL;
    }
    
    ASSERT_EQ(0, arr[0]);
    ASSERT_EQ(9000000000LL, arr[9]);
}

TEST_CATEGORY(uint64_bitwise, TEST_CATEGORY_STDLIB) {
    uint64_t a = 0xFFFFFFFF00000000ULL;
    uint64_t b = 0x00000000FFFFFFFFULL;
    uint64_t c = a | b;
    
    ASSERT_EQ(0xFFFFFFFFFFFFFFFFULL, c);
}

TEST_CATEGORY(int64_negative_values, TEST_CATEGORY_STDLIB) {
    int64_t neg = -1000000000000LL;
    ASSERT_TRUE(neg < 0);
    ASSERT_TRUE(neg < INT32_MIN);
}

