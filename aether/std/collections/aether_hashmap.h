#ifndef AETHER_HASHMAP_H
#define AETHER_HASHMAP_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Generic HashMap using Robin Hood hashing for better cache locality
// Open addressing with linear probing

typedef struct {
    void* key;
    void* value;
    uint64_t hash;
    int32_t psl;  // Probe sequence length (for Robin Hood)
    bool occupied;
} HashMapEntry;

typedef struct {
    HashMapEntry* entries;
    size_t capacity;
    size_t size;
    float load_factor;
    
    // Function pointers for generic key/value operations
    uint64_t (*hash_func)(const void* key);
    bool (*key_equals)(const void* a, const void* b);
    void (*key_free)(void* key);
    void (*value_free)(void* value);
    void* (*key_clone)(const void* key);
    void* (*value_clone)(const void* value);
} HashMap;

// Iterator for traversing the hashmap
typedef struct {
    HashMap* map;
    size_t index;
} HashMapIterator;

// HashMap creation and destruction
HashMap* hashmap_create(size_t initial_capacity,
                       uint64_t (*hash_func)(const void*),
                       bool (*key_equals)(const void*, const void*),
                       void (*key_free)(void*),
                       void (*value_free)(void*),
                       void* (*key_clone)(const void*),
                       void* (*value_clone)(const void*));

void hashmap_free(HashMap* map);

// Core operations
bool hashmap_insert(HashMap* map, void* key, void* value);
void* hashmap_get(HashMap* map, const void* key);
bool hashmap_remove(HashMap* map, const void* key);
bool hashmap_contains(HashMap* map, const void* key);
void hashmap_clear(HashMap* map);

// Size operations
size_t hashmap_size(HashMap* map);
bool hashmap_is_empty(HashMap* map);

// Iterator operations
HashMapIterator hashmap_iterator(HashMap* map);
bool hashmap_iterator_next(HashMapIterator* iter, void** key, void** value);

// Utility functions for common types
uint64_t hashmap_hash_string(const void* key);
bool hashmap_equals_string(const void* a, const void* b);
void hashmap_free_string(void* str);
void* hashmap_clone_string(const void* str);

uint64_t hashmap_hash_int(const void* key);
bool hashmap_equals_int(const void* a, const void* b);
void hashmap_free_int(void* ptr);
void* hashmap_clone_int(const void* ptr);

// Convenience constructors
HashMap* hashmap_create_string_to_int(size_t initial_capacity);
HashMap* hashmap_create_string_to_string(size_t initial_capacity);
HashMap* hashmap_create_int_to_int(size_t initial_capacity);

#endif // AETHER_HASHMAP_H

