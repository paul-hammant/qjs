#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <pthread.h>

typedef struct Increment {
    int _message_id;
    int amount;
} Increment;

typedef struct {
    int id;
    atomic_int active;
    pthread_t thread;
    int auto_process;
    int count;
} Counter;

void* counter_thread(void* arg) {
    Counter* self = (Counter*)arg;
    printf("Counter thread started, id=%d\n", self->id);
    return NULL;
}

Counter* spawn_Counter() {
    Counter* actor = malloc(sizeof(Counter));
    actor->id = 1;
    atomic_init(&actor->active, 1);
    actor->auto_process = 1;
    actor->count = 0;
    
    if (actor->auto_process) {
        pthread_create(&actor->thread, NULL, counter_thread, actor);
    }
    
    return actor;
}

int main() {
    printf("Testing Actor V2 threading...\n");
    
    Counter* c = spawn_Counter();
    
    Increment msg = { ._message_id = 1000, .amount = 5 };
    printf("Created Increment message: id=%d, amount=%d\n", msg._message_id, msg.amount);
    
    pthread_join(c->thread, NULL);
    free(c);
    
    printf("Test complete\n");
    return 0;
}
