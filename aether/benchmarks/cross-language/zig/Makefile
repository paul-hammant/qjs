.PHONY: all clean ping_pong counting thread_ring fork_join skynet

all: ping_pong counting thread_ring fork_join skynet

ping_pong: ping_pong.zig
	zig build -Doptimize=ReleaseFast
	cp zig-out/bin/ping_pong .

counting: counting.zig
	zig build -Doptimize=ReleaseFast
	cp zig-out/bin/counting .

thread_ring: thread_ring.zig
	zig build -Doptimize=ReleaseFast
	cp zig-out/bin/thread_ring .

fork_join: fork_join.zig
	zig build -Doptimize=ReleaseFast
	cp zig-out/bin/fork_join .

skynet: skynet.zig
	zig build -Doptimize=ReleaseFast
	cp zig-out/bin/skynet .

clean:
	rm -rf .zig-cache zig-out ping_pong counting thread_ring fork_join skynet *.o
