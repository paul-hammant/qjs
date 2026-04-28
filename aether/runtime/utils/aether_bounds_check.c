#include "aether_bounds_check.h"
#include <stdio.h>
#include <stdlib.h>

// Array bounds checking
void aether_check_array_access(int index, int length, const char* array_name,
                                const char* file, int line) {
    if (index < 0 || index >= length) {
        fprintf(stderr, "\n%s:%d: ", file, line);
        fprintf(stderr, "\033[31m\033[1merror:\033[0m Array index out of bounds\n");
        fprintf(stderr, "  Array: %s\n", array_name);
        fprintf(stderr, "  Index: %d\n", index);
        fprintf(stderr, "  Length: %d\n", length);
        fprintf(stderr, "  Valid range: [0, %d)\n\n", length);
        abort();
    }
}

// Null pointer checking
void aether_check_null_pointer(const void* ptr, const char* ptr_name,
                                const char* file, int line) {
    if (!ptr) {
        fprintf(stderr, "\n%s:%d: ", file, line);
        fprintf(stderr, "\033[31m\033[1merror:\033[0m Null pointer dereference\n");
        fprintf(stderr, "  Pointer: %s\n\n", ptr_name);
        abort();
    }
}

// Division by zero checking
void aether_check_div_by_zero(int divisor, const char* file, int line) {
    if (divisor == 0) {
        fprintf(stderr, "\n%s:%d: ", file, line);
        fprintf(stderr, "\033[31m\033[1merror:\033[0m Division by zero\n\n");
        abort();
    }
}

void aether_check_div_by_zero_float(float divisor, const char* file, int line) {
    if (divisor == 0.0f) {
        fprintf(stderr, "\n%s:%d: ", file, line);
        fprintf(stderr, "\033[31m\033[1merror:\033[0m Division by zero (float)\n\n");
        abort();
    }
}

// Assertion
void aether_assert_impl(int condition, const char* condition_str,
                        const char* file, int line, const char* message) {
    if (!condition) {
        fprintf(stderr, "\n%s:%d: ", file, line);
        fprintf(stderr, "\033[31m\033[1massertion failed:\033[0m %s\n", condition_str);
        if (message && message[0] != '\0') {
            fprintf(stderr, "  Message: %s\n", message);
        }
        fprintf(stderr, "\n");
        abort();
    }
}

