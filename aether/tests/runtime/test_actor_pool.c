/**
 * Actor Pool Tests
 * Validates type-specific actor pool allocation and deallocation
 */

#include "test_harness.h"
#include "../../runtime/actors/aether_actor_pool.h"
#include "../../runtime/actors/actor_state_machine.h"
#include <stdlib.h>
#include <string.h>

// Helper reset function for test_pool_reset_function
static int g_reset_called = 0;
void test_reset_helper(PooledActor* a) {
    (void)a;
    g_reset_called = 1;
}

void test_pool_init() {
    ActorPool* pool = malloc(sizeof(ActorPool));
    ASSERT_NOT_NULL(pool);
    actor_pool_init(pool);
    
    ASSERT_EQ(pool->initialized, 1);
    ASSERT_EQ(atomic_load(&pool->alloc_index), 0);
    ASSERT_EQ(atomic_load(&pool->free_index), 0);
    
    // All actors should be initialized
    for (int i = 0; i < ACTOR_POOL_SIZE; i++) {
        ASSERT_EQ(pool->actors[i].pool_index, i);
        ASSERT_EQ(atomic_load(&pool->actors[i].in_use), 0);
        ASSERT_EQ(pool->actors[i].active, 0);
    }
    free(pool);
}

void test_pool_acquire_release() {
    ActorPool* pool = malloc(sizeof(ActorPool));
    ASSERT_NOT_NULL(pool);
    actor_pool_init(pool);
    
    // Acquire an actor
    PooledActor* actor1 = actor_pool_acquire(pool);
    ASSERT_TRUE(actor1 != NULL);
    ASSERT_EQ(actor1->active, 1);
    ASSERT_EQ(atomic_load(&actor1->in_use), 1);
    
    // Acquire another
    PooledActor* actor2 = actor_pool_acquire(pool);
    ASSERT_TRUE(actor2 != NULL);
    ASSERT_TRUE(actor1 != actor2);
    ASSERT_EQ(atomic_load(&actor2->in_use), 1);
    
    // Release first actor
    actor_pool_release(pool, actor1);
    ASSERT_EQ(atomic_load(&actor1->in_use), 0);
    
    // Reacquire - might get same actor back
    PooledActor* actor3 = actor_pool_acquire(pool);
    ASSERT_TRUE(actor3 != NULL);
    ASSERT_EQ(atomic_load(&actor3->in_use), 1);
    
    // Clean up
    actor_pool_release(pool, actor2);
    actor_pool_release(pool, actor3);
    free(pool);
}

void test_pool_exhaustion() {
    ActorPool* pool = malloc(sizeof(ActorPool));
    ASSERT_NOT_NULL(pool);
    actor_pool_init(pool);
    
    PooledActor* actors[ACTOR_POOL_SIZE];
    
    // Acquire all actors
    for (int i = 0; i < ACTOR_POOL_SIZE; i++) {
        actors[i] = actor_pool_acquire(pool);
        ASSERT_TRUE(actors[i] != NULL);
        ASSERT_EQ(atomic_load(&actors[i]->in_use), 1);
    }
    
    // Next acquisition should fail
    PooledActor* overflow = actor_pool_acquire(pool);
    ASSERT_TRUE(overflow == NULL);
    
    // Release one
    actor_pool_release(pool, actors[0]);
    
    // Now we can acquire again
    overflow = actor_pool_acquire(pool);
    ASSERT_TRUE(overflow != NULL);
    
    // Clean up
    actor_pool_release(pool, overflow);
    for (int i = 1; i < ACTOR_POOL_SIZE; i++) {
        actor_pool_release(pool, actors[i]);
    }
    free(pool);
}

void test_pool_reset_function() {
    ActorPool* pool = malloc(sizeof(ActorPool));
    ASSERT_NOT_NULL(pool);
    actor_pool_init(pool);

    // Set up reset function on an actor
    PooledActor* actor = actor_pool_acquire(pool);
    ASSERT_TRUE(actor != NULL);

    g_reset_called = 0;
    actor->reset_fn = test_reset_helper;

    // Release and reacquire
    actor_pool_release(pool, actor);

    PooledActor* reacquired = actor_pool_acquire(pool);

    // Reset function should have been called if we got same actor
    if (reacquired == actor) {
        ASSERT_EQ(g_reset_called, 1);
    }
    
    actor_pool_release(pool, reacquired);
    free(pool);
}

void test_pool_mailbox_integration() {
    ActorPool* pool = malloc(sizeof(ActorPool));
    ASSERT_NOT_NULL(pool);
    actor_pool_init(pool);
    
    PooledActor* actor = actor_pool_acquire(pool);
    ASSERT_TRUE(actor != NULL);
    
    // Mailbox should be initialized
    ASSERT_EQ(actor->mailbox.count, 0);
    ASSERT_EQ(actor->mailbox.head, 0);
    ASSERT_EQ(actor->mailbox.tail, 0);
    
    // Send messages to mailbox
    for (int i = 0; i < 10; i++) {
        Message msg = message_create_simple(MSG_INCREMENT, 0, i);
        int result = mailbox_send(&actor->mailbox, msg);
        ASSERT_EQ(result, 1);
    }
    
    ASSERT_EQ(actor->mailbox.count, 10);
    
    // Release actor (mailbox will be reset on next acquire)
    actor_pool_release(pool, actor);
    
    // Reacquire
    PooledActor* reacquired = actor_pool_acquire(pool);
    ASSERT_TRUE(reacquired != NULL);
    
    // Mailbox should be reinitialized
    ASSERT_EQ(reacquired->mailbox.count, 0);
    
    actor_pool_release(pool, reacquired);
    free(pool);
}

void test_pool_concurrent_simulation() {
    ActorPool* pool = malloc(sizeof(ActorPool));
    ASSERT_NOT_NULL(pool);
    actor_pool_init(pool);
    
    // Simulate concurrent acquire/release pattern
    PooledActor* active[10];

    // Acquire 10 actors
    for (int i = 0; i < 10; i++) {
        active[i] = actor_pool_acquire(pool);
        ASSERT_TRUE(active[i] != NULL);
    }
    
    // Release 5, acquire 5 more
    for (int i = 0; i < 5; i++) {
        actor_pool_release(pool, active[i]);
    }
    
    for (int i = 0; i < 5; i++) {
        active[i] = actor_pool_acquire(pool);
        ASSERT_TRUE(active[i] != NULL);
    }
    
    // Clean up all
    for (int i = 0; i < 10; i++) {
        actor_pool_release(pool, active[i]);
    }
    free(pool);
}

void register_actor_pool_tests() {
    register_test_with_category("Actor pool initialization", test_pool_init, TEST_CATEGORY_RUNTIME);
    register_test_with_category("Actor pool acquire and release", test_pool_acquire_release, TEST_CATEGORY_RUNTIME);
    register_test_with_category("Actor pool exhaustion handling", test_pool_exhaustion, TEST_CATEGORY_RUNTIME);
    register_test_with_category("Actor pool reset function", test_pool_reset_function, TEST_CATEGORY_RUNTIME);
    register_test_with_category("Actor pool mailbox integration", test_pool_mailbox_integration, TEST_CATEGORY_RUNTIME);
    register_test_with_category("Actor pool concurrent simulation", test_pool_concurrent_simulation, TEST_CATEGORY_RUNTIME);
}
