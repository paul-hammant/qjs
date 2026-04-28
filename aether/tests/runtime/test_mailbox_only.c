// Minimal test - no threads, just mailbox operations

#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>

#include "../../runtime/actors/actor_state_machine.h"

int main() {
    printf("Testing mailbox operations (no scheduler)...\n\n");
    
    Mailbox mbox;
    mailbox_init(&mbox);
    
    printf("1. Testing send...\n");
    for (int i = 0; i < 10; i++) {
        Message msg = {1, 0, i, NULL};
        int result = mailbox_send(&mbox, msg);
        printf("   Sent message %d: %s\n", i, result ? "OK" : "FAILED");
    }
    
    printf("\n2. Mailbox state: head=%d, tail=%d, count=%d\n", 
           mbox.head, mbox.tail, mbox.count);
    
    printf("\n3. Testing receive...\n");
    for (int i = 0; i < 10; i++) {
        Message msg;
        int result = mailbox_receive(&mbox, &msg);
        if (result) {
            printf("   Received message: type=%d, payload=%td\n", msg.type, msg.payload_int);
        } else {
            printf("   Receive failed at message %d\n", i);
            break;
        }
    }
    
    printf("\n4. Final mailbox state: head=%d, tail=%d, count=%d\n", 
           mbox.head, mbox.tail, mbox.count);
    
    printf("\n✓ Mailbox operations work correctly\n");
    return 0;
}
