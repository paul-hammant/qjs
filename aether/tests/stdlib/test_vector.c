/**
 * Aether Vector Test Suite
 * Tests for std/collections/aether_vector
 */

#include "test_framework.h"
#include "../../std/collections/aether_vector.h"

int main(void) {
    TEST_SUITE_BEGIN("Vector Tests");
    
    // Test 1: Create
    Vector* v = vector_create(10, free, test_int_clone);
    TEST("Create", v != NULL && vector_size(v) == 0 && vector_capacity(v) >= 10);
    
    // Test 2: Push
    int *v1 = test_make_int(10);
    int *v2 = test_make_int(20);
    int *v3 = test_make_int(30);
    vector_push(v, v1);
    vector_push(v, v2);
    vector_push(v, v3);
    TEST("Push", vector_size(v) == 3);
    
    // Test 3: Get
    TEST("Get", *(int*)vector_get(v, 0) == 10 && *(int*)vector_get(v, 2) == 30);
    
    // Test 4: Pop
    void* p = vector_pop(v);
    bool pop_ok = (*(int*)p == 30 && vector_size(v) == 2);
    free(p);
    TEST("Pop", pop_ok);
    
    // Test 5: Insert
    int *val = test_make_int(15);
    vector_insert(v, 1, val);
    TEST("Insert", vector_size(v) == 3 && *(int*)vector_get(v, 1) == 15);
    
    // Test 6: Remove
    vector_remove(v, 1);
    TEST("Remove", vector_size(v) == 2);
    
    // Test 7: Find
    int search = 20;
    TEST("Find", vector_find(v, &search, test_int_equals) != -1);
    
    // Test 8: Clear
    vector_clear(v);
    TEST("Clear", vector_is_empty(v));
    
    // Test 9: Auto-grow
    for (int i = 0; i < 20; i++) {
        int *val = test_make_int(i);
        vector_push(v, val);
    }
    TEST("Auto-grow", vector_size(v) == 20 && vector_capacity(v) >= 20);
    
    // Cleanup
    vector_free(v);
    
    TEST_SUITE_END();
}
