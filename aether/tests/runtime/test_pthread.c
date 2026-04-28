// Test pthread creation on Windows

#include <stdio.h>
#include <pthread.h>

void* test_thread(void* arg) {
    printf("Thread %d running!\n", *(int*)arg);
    return NULL;
}

int main() {
    printf("Testing pthread creation...\n");
    
    pthread_t thread;
    int id = 1;
    
    printf("Creating thread...\n");
    int result = pthread_create(&thread, NULL, test_thread, &id);
    
    if (result != 0) {
        printf("pthread_create failed with error %d\n", result);
        return 1;
    }
    
    printf("Waiting for thread...\n");
    pthread_join(thread, NULL);
    
    printf("✓ pthread works\n");
    return 0;
}
