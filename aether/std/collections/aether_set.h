#ifndef AETHER_SET_H
#define AETHER_SET_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "aether_hashmap.h"

// Set implementation using HashMap (value is dummy)
typedef struct {
    HashMap* map;
} Set;

// Set creation and destruction
Set* set_create(size_t initial_capacity,
               uint64_t (*hash_func)(const void*),
               bool (*key_equals)(const void*, const void*),
               void (*key_free)(void*),
               void* (*key_clone)(const void*));

void set_free(Set* set);

// Core operations
bool set_add(Set* set, void* element);
bool set_remove(Set* set, const void* element);
bool set_contains(Set* set, const void* element);
void set_clear(Set* set);

// Size operations
size_t set_size(Set* set);
bool set_is_empty(Set* set);

// Set operations
Set* set_union(Set* a, Set* b);
Set* set_intersection(Set* a, Set* b);
Set* set_difference(Set* a, Set* b);
bool set_is_subset(Set* a, Set* b);
bool set_is_superset(Set* a, Set* b);

// Iterator (reuses HashMap iterator)
typedef HashMapIterator SetIterator;
SetIterator set_iterator(Set* set);
bool set_iterator_next(SetIterator* iter, void** element);

// Convenience constructors
Set* set_create_string(size_t initial_capacity);
Set* set_create_int(size_t initial_capacity);

#endif // AETHER_SET_H

