/**
 * Aether Set Test Suite
 * Tests for std/collections/aether_set
 */

#include "test_framework.h"
#include "../../std/collections/aether_set.h"

int main(void) {
    TEST_SUITE_BEGIN("Set Tests");
    
    // Test 1: Create
    Set* set = set_create(16, test_int_hash, test_int_equals, NULL, test_int_clone);
    TEST("Create", set != NULL && set_size(set) == 0);
    
    // Test 2: Add
    int v1 = 10, v2 = 20, v3 = 30;
    set_add(set, &v1);
    set_add(set, &v2);
    set_add(set, &v3);
    TEST("Add", set_size(set) == 3);
    
    // Test 3: Contains
    TEST("Contains", set_contains(set, &v2));
    
    // Test 4: Remove
    set_remove(set, &v1);
    TEST("Remove", set_size(set) == 2 && !set_contains(set, &v1));
    
    // Test 5: Union
    Set* set2 = set_create(16, test_int_hash, test_int_equals, NULL, test_int_clone);
    int v4 = 40, v5 = 50;
    set_add(set2, &v4);
    set_add(set2, &v5);
    Set* union_set = set_union(set, set2);
    bool union_ok = (set_size(union_set) == 4);
    set_free(union_set);
    TEST("Union", union_ok);
    
    // Test 6: Intersection
    set_add(set2, &v2);
    Set* inter_set = set_intersection(set, set2);
    bool inter_ok = (set_size(inter_set) == 1);
    set_free(inter_set);
    TEST("Intersection", inter_ok);
    
    // Cleanup
    set_free(set);
    set_free(set2);
    
    TEST_SUITE_END();
}
