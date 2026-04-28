// Unit tests for actor optimizations

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../../runtime/actors/actor_state_machine.h"
#include "../../runtime/actors/aether_actor_pool.h"
#include "../../runtime/actors/aether_direct_send.h"
#include "../../runtime/actors/aether_message_dedup.h"
#include "../../runtime/actors/aether_message_specialize.h"
#include "../../runtime/actors/aether_adaptive_batch.h"

int tests_passed = 0;
int tests_failed = 0;

#define TEST(name) \
    printf("  Testing %s... ", name); \
    fflush(stdout);

#define PASS() \
    printf("PASS\n"); \
    tests_passed++;

#define FAIL(msg) \
    printf("FAIL: %s\n", msg); \
    tests_failed++;

// Test 1: Actor Pool
void test_actor_pool() {
    TEST("actor pool acquire/release");
    
    ActorPool pool;
    actor_pool_init(&pool);
    
    // Acquire actors
    PooledActor* actors[10];
    for (int i = 0; i < 10; i++) {
        actors[i] = actor_pool_acquire(&pool);
        if (!actors[i]) {
            FAIL("Failed to acquire actor from pool");
            return;
        }
    }
    
    // Release actors
    for (int i = 0; i < 10; i++) {
        actor_pool_release(&pool, actors[i]);
    }
    
    // Reacquire - should get same actors
    PooledActor* reacquired = actor_pool_acquire(&pool);
    if (!reacquired) {
        FAIL("Failed to reacquire actor from pool");
        return;
    }
    
    PASS();
}

void test_actor_pool_exhaustion() {
    TEST("actor pool exhaustion");
    
    ActorPool pool;
    actor_pool_init(&pool);
    
    // Acquire all actors
    PooledActor* actors[ACTOR_POOL_SIZE];
    for (int i = 0; i < ACTOR_POOL_SIZE; i++) {
        actors[i] = actor_pool_acquire(&pool);
        if (!actors[i]) {
            FAIL("Pool exhausted prematurely");
            return;
        }
    }
    
    // Next acquisition should fail
    PooledActor* overflow = actor_pool_acquire(&pool);
    if (overflow != NULL) {
        FAIL("Pool should be exhausted");
        return;
    }
    
    // Release one
    actor_pool_release(&pool, actors[0]);
    
    // Should be able to acquire again
    overflow = actor_pool_acquire(&pool);
    if (!overflow) {
        FAIL("Failed to acquire after release");
        return;
    }
    
    PASS();
}

// Test 2: Direct Send
void test_direct_send() {
    TEST("direct send same core");
    
    current_scheduler_id = 0;
    
    // Create two actors on same core
    Mailbox mbox1, mbox2;
    mailbox_init(&mbox1);
    mailbox_init(&mbox2);
    
    ActorMetadata meta1 = {&mbox1, 0, 0};
    ActorMetadata meta2 = {&mbox2, 0, 0};
    
    // Should succeed for same core
    if (!actors_same_core(&meta1, &meta2)) {
        FAIL("Actors should be on same core");
        return;
    }
    
    PASS();
}

void test_direct_send_different_cores() {
    TEST("direct send different cores");
    
    Mailbox mbox1, mbox2;
    mailbox_init(&mbox1);
    mailbox_init(&mbox2);
    
    ActorMetadata meta1 = {&mbox1, 0, 0};
    ActorMetadata meta2 = {&mbox2, 1, 0};  // Different core
    
    if (actors_same_core(&meta1, &meta2)) {
        FAIL("Actors should be on different cores");
        return;
    }
    
    PASS();
}

// Test 3: Message Deduplication
void test_message_dedup() {
    TEST("message deduplication");
    
    DedupWindow window = {0};
    Message msg1 = {1, 0, 42, NULL};
    Message msg2 = {1, 0, 42, NULL};  // Duplicate
    Message msg3 = {1, 0, 43, NULL};  // Different
    
    // First message not duplicate
    if (is_duplicate(&window, &msg1)) {
        FAIL("First message should not be duplicate");
        return;
    }
    record_message(&window, &msg1);
    
    // Second message is duplicate
    if (!is_duplicate(&window, &msg2)) {
        FAIL("Second message should be duplicate");
        return;
    }
    
    // Third message not duplicate
    if (is_duplicate(&window, &msg3)) {
        FAIL("Different message should not be duplicate");
        return;
    }
    
    PASS();
}

void test_dedup_window_overflow() {
    TEST("deduplication window overflow");
    
    DedupWindow window = {0};
    
    // Fill window with unique messages
    for (int i = 0; i < DEDUP_WINDOW_SIZE; i++) {
        Message msg = {1, 0, i, NULL};
        record_message(&window, &msg);
    }
    
    // Add one more - should evict oldest
    Message new_msg = {1, 0, DEDUP_WINDOW_SIZE, NULL};
    record_message(&window, &new_msg);
    
    // First message should no longer be detected as duplicate
    Message old_msg = {1, 0, 0, NULL};
    if (is_duplicate(&window, &old_msg)) {
        FAIL("Old message should have been evicted");
        return;
    }
    
    PASS();
}

// Test 4: Message Specialization
void test_specialized_send() {
    TEST("specialized message send");
    
    Mailbox mbox;
    mailbox_init(&mbox);
    
    // Use specialized send
    if (!send_increment(&mbox, 123)) {
        FAIL("Specialized send failed");
        return;
    }
    
    // Verify message
    Message received;
    if (!mailbox_receive(&mbox, &received)) {
        FAIL("Failed to receive message");
        return;
    }
    
    if (received.type != MSG_INCREMENT) {
        FAIL("Wrong message type");
        return;
    }
    
    if (received.sender_id != 123) {
        FAIL("Wrong sender_id");
        return;
    }
    
    PASS();
}

void test_specialized_receive() {
    TEST("specialized message receive");
    
    Mailbox mbox;
    mailbox_init(&mbox);
    
    // Send increment message
    send_increment(&mbox, 0);
    
    // Use specialized receive
    Message msg;
    if (!receive_increment(&mbox, &msg)) {
        FAIL("Specialized receive failed");
        return;
    }
    
    if (msg.type != MSG_INCREMENT) {
        FAIL("Wrong message type");
        return;
    }
    
    PASS();
}

// Test 5: Adaptive Batching
void test_adaptive_batch_init() {
    TEST("adaptive batch initialization");
    
    AdaptiveBatchState state;
    adaptive_batch_init(&state);
    
    if (state.current_batch_size < MIN_BATCH_SIZE || 
        state.current_batch_size > MAX_BATCH_SIZE) {
        FAIL("Invalid initial batch size");
        return;
    }
    
    if (state.total_messages_processed != 0) {
        FAIL("Message count should be zero");
        return;
    }
    
    PASS();
}

void test_adaptive_batch_increase() {
    TEST("adaptive batch size increase");
    
    AdaptiveBatchState state;
    adaptive_batch_init(&state);
    
    int initial_size = state.current_batch_size;
    
    // Simulate consistently full batches
    for (int i = 0; i < 10; i++) {
        adaptive_batch_adjust(&state, state.current_batch_size);
    }
    
    if (state.current_batch_size <= initial_size) {
        FAIL("Batch size should have increased");
        return;
    }
    
    PASS();
}

void test_adaptive_batch_decrease() {
    TEST("adaptive batch size decrease");
    
    AdaptiveBatchState state;
    adaptive_batch_init(&state);
    state.current_batch_size = 32;  // Start high
    
    // Simulate consistently partial batches
    for (int i = 0; i < 20; i++) {
        adaptive_batch_adjust(&state, 2);  // Only 2 messages
    }
    
    if (state.current_batch_size >= 32) {
        FAIL("Batch size should have decreased");
        return;
    }
    
    if (state.current_batch_size < MIN_BATCH_SIZE) {
        FAIL("Batch size should not go below minimum");
        return;
    }
    
    PASS();
}

// Integration test
void test_combined_optimizations() {
    TEST("combined optimizations");
    
    ActorPool pool;
    actor_pool_init(&pool);
    current_scheduler_id = 0;
    
    // Acquire pooled actor
    PooledActor* actor = actor_pool_acquire(&pool);
    if (!actor) {
        FAIL("Failed to acquire actor");
        return;
    }
    
    mailbox_init(&actor->mailbox);
    
    // Use specialized send
    DedupWindow dedup = {0};
    for (int i = 0; i < 10; i++) {
        Message msg = {MSG_INCREMENT, 0, i, NULL};
        if (!is_duplicate(&dedup, &msg)) {
            send_increment(&actor->mailbox, 0);
            record_message(&dedup, &msg);
        }
    }
    
    // Verify messages received
    if (actor->mailbox.count != 10) {
        FAIL("Wrong number of messages");
        actor_pool_release(&pool, actor);
        return;
    }
    
    // Release actor
    actor_pool_release(&pool, actor);
    
    PASS();
}

int main() {
    printf("=== Actor Optimization Tests ===\n\n");
    
    printf("Actor Pool Tests:\n");
    test_actor_pool();
    test_actor_pool_exhaustion();
    
    printf("\nDirect Send Tests:\n");
    test_direct_send();
    test_direct_send_different_cores();
    
    printf("\nMessage Deduplication Tests:\n");
    test_message_dedup();
    test_dedup_window_overflow();
    
    printf("\nMessage Specialization Tests:\n");
    test_specialized_send();
    test_specialized_receive();
    
    printf("\nAdaptive Batching Tests:\n");
    test_adaptive_batch_init();
    test_adaptive_batch_increase();
    test_adaptive_batch_decrease();
    
    printf("\nIntegration Tests:\n");
    test_combined_optimizations();
    
    printf("\n=== Test Summary ===\n");
    printf("Passed: %d\n", tests_passed);
    printf("Failed: %d\n", tests_failed);
    
    return tests_failed > 0 ? 1 : 0;
}
