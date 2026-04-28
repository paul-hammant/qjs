// Runtime Type Checking Functions for Aether
// Provides dynamic type inspection and checking

#ifndef AETHER_RUNTIME_TYPES_H
#define AETHER_RUNTIME_TYPES_H

#include <stdint.h>

// Type codes for runtime type checking
typedef enum {
    RUNTIME_TYPE_INT = 0,
    RUNTIME_TYPE_FLOAT = 1,
    RUNTIME_TYPE_BOOL = 2,
    RUNTIME_TYPE_STRING = 3,
    RUNTIME_TYPE_ARRAY = 4,
    RUNTIME_TYPE_STRUCT = 5,
    RUNTIME_TYPE_ACTOR_REF = 6,
    RUNTIME_TYPE_VOID = 7,
    RUNTIME_TYPE_UNKNOWN = 8
} RuntimeTypeCode;

// Runtime value wrapper with type tag
typedef struct {
    RuntimeTypeCode type;
    union {
        int64_t int_val;
        double float_val;
        int bool_val;
        char* string_val;
        void* ptr_val;  // For arrays, structs, actor refs
    } value;
} RuntimeValue;

// Runtime type checking functions
const char* aether_typeof(RuntimeValue* val);
int aether_is_type(RuntimeValue* val, const char* type_name);
RuntimeValue* aether_convert_type(RuntimeValue* val, const char* target_type);

// Helper functions
RuntimeTypeCode string_to_type_code(const char* type_name);
const char* type_code_to_string(RuntimeTypeCode code);

#endif // AETHER_RUNTIME_TYPES_H
