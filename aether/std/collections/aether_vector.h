#ifndef AETHER_VECTOR_H
#define AETHER_VECTOR_H

#include <stddef.h>
#include <stdbool.h>

// Generic dynamic array with 2x growth factor
typedef struct {
    void** data;
    size_t size;
    size_t capacity;
    void (*element_free)(void*);
    void* (*element_clone)(const void*);
} Vector;

// Vector creation and destruction
Vector* vector_create(size_t initial_capacity,
                     void (*element_free)(void*),
                     void* (*element_clone)(const void*));
void vector_free(Vector* vec);

// Core operations (amortized O(1) for push)
bool vector_push(Vector* vec, void* element);
void* vector_pop(Vector* vec);
void* vector_get(Vector* vec, size_t index);
bool vector_set(Vector* vec, size_t index, void* element);
bool vector_insert(Vector* vec, size_t index, void* element);
bool vector_remove(Vector* vec, size_t index);
void vector_clear(Vector* vec);

// Size operations
size_t vector_size(Vector* vec);
bool vector_is_empty(Vector* vec);
size_t vector_capacity(Vector* vec);
bool vector_reserve(Vector* vec, size_t new_capacity);
void vector_shrink_to_fit(Vector* vec);

// Search operations
int vector_find(Vector* vec, const void* element, bool (*equals)(const void*, const void*));
bool vector_contains(Vector* vec, const void* element, bool (*equals)(const void*, const void*));

// Utility operations
void vector_reverse(Vector* vec);
void vector_sort(Vector* vec, int (*compare)(const void*, const void*));

#endif // AETHER_VECTOR_H

