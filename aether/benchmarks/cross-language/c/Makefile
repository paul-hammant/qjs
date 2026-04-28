.PHONY: all clean ping_pong counting thread_ring fork_join skynet

CC = gcc
CFLAGS = -O3 -march=native -pthread -Wall
LDFLAGS = -pthread

all: ping_pong counting thread_ring fork_join skynet

ping_pong: ping_pong.c
	$(CC) $(CFLAGS) ping_pong.c -o ping_pong $(LDFLAGS)

counting: counting.c
	$(CC) $(CFLAGS) counting.c -o counting $(LDFLAGS)

thread_ring: thread_ring.c
	$(CC) $(CFLAGS) thread_ring.c -o thread_ring $(LDFLAGS)

fork_join: fork_join.c
	$(CC) $(CFLAGS) fork_join.c -o fork_join $(LDFLAGS)

skynet: skynet.c
	$(CC) $(CFLAGS) skynet.c -o skynet $(LDFLAGS)

clean:
	rm -f ping_pong counting thread_ring fork_join skynet
