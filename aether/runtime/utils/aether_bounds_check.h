#ifndef AETHER_BOUNDS_CHECK_H
#define AETHER_BOUNDS_CHECK_H

#include <stddef.h>

// Bounds checking is controlled by compile-time flag
// -DAETHER_DEBUG enables bounds checking
// -DAETHER_RELEASE disables bounds checking (default)

#ifdef AETHER_DEBUG
#define AETHER_BOUNDS_CHECK_ENABLED 1
#else
#define AETHER_BOUNDS_CHECK_ENABLED 0
#endif

// Array bounds checking
void aether_check_array_access(int index, int length, const char* array_name, 
                                const char* file, int line);

// Macro for bounds-checked array access
#if AETHER_BOUNDS_CHECK_ENABLED
#define AETHER_ARRAY_ACCESS(array, index, length) \
    (aether_check_array_access((index), (length), #array, __FILE__, __LINE__), (array)[(index)])
#else
#define AETHER_ARRAY_ACCESS(array, index, length) ((array)[(index)])
#endif

// Null pointer checking
void aether_check_null_pointer(const void* ptr, const char* ptr_name,
                                const char* file, int line);

#if AETHER_BOUNDS_CHECK_ENABLED
#define AETHER_CHECK_NULL(ptr) \
    aether_check_null_pointer((ptr), #ptr, __FILE__, __LINE__)
#else
#define AETHER_CHECK_NULL(ptr) ((void)0)
#endif

// Division by zero checking
void aether_check_div_by_zero(int divisor, const char* file, int line);
void aether_check_div_by_zero_float(float divisor, const char* file, int line);

#if AETHER_BOUNDS_CHECK_ENABLED
#define AETHER_CHECK_DIV(divisor) \
    aether_check_div_by_zero((divisor), __FILE__, __LINE__)
#define AETHER_CHECK_DIV_FLOAT(divisor) \
    aether_check_div_by_zero_float((divisor), __FILE__, __LINE__)
#else
#define AETHER_CHECK_DIV(divisor) ((void)0)
#define AETHER_CHECK_DIV_FLOAT(divisor) ((void)0)
#endif

// Assertion
void aether_assert_impl(int condition, const char* condition_str,
                        const char* file, int line, const char* message);

#define AETHER_ASSERT(condition, message) \
    aether_assert_impl((condition), #condition, __FILE__, __LINE__, (message))

#endif // AETHER_BOUNDS_CHECK_H

