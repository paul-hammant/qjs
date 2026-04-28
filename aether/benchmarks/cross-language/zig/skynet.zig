// Zig Skynet Benchmark
// Based on https://github.com/atemerev/skynet
// Uses std.Thread for top THREAD_DEPTH levels, sequential below.
// Spawning 1M OS threads is not feasible; limits concurrent threads to ~1000.

const std = @import("std");
const Thread = std.Thread;
const print = std.debug.print;

const THREAD_DEPTH: usize = 3;

fn getLeaves() i64 {
    if (std.posix.getenv("SKYNET_LEAVES")) |val| {
        return std.fmt.parseInt(i64, val, 10) catch 1_000_000;
    }
    if (std.posix.getenv("BENCHMARK_MESSAGES")) |val| {
        return std.fmt.parseInt(i64, val, 10) catch 1_000_000;
    }
    return 1_000_000;
}

fn skynetSeq(offset: i64, size: i64) i64 {
    if (size == 1) return offset;
    const child_size = @divTrunc(size, 10);
    var sum: i64 = 0;
    var i: i64 = 0;
    while (i < 10) : (i += 1) {
        sum += skynetSeq(offset + i * child_size, child_size);
    }
    return sum;
}

const SkynetArg = struct {
    offset: i64,
    size: i64,
    depth: usize,
    result: i64 = 0,
};

fn skynetThread(arg: *SkynetArg) void {
    const offset = arg.offset;
    const size = arg.size;
    const depth = arg.depth;

    if (size == 1 or depth >= THREAD_DEPTH) {
        arg.result = skynetSeq(offset, size);
        return;
    }

    const child_size = @divTrunc(size, 10);
    var children: [10]SkynetArg = undefined;
    var threads: [10]Thread = undefined;

    for (0..10) |i| {
        children[i] = SkynetArg{
            .offset = offset + @as(i64, @intCast(i)) * child_size,
            .size = child_size,
            .depth = depth + 1,
        };
        threads[i] = Thread.spawn(.{}, skynetThread, .{&children[i]}) catch {
            // Fallback: compute sequentially if spawn fails
            children[i].result = skynetSeq(children[i].offset, children[i].size);
            threads[i] = undefined;
            continue;
        };
    }

    var sum: i64 = 0;
    for (0..10) |i| {
        threads[i].join();
        sum += children[i].result;
    }
    arg.result = sum;
}

fn getTimeNs() u64 {
    const ts = std.posix.clock_gettime(std.posix.CLOCK.MONOTONIC) catch return 0;
    return @as(u64, @intCast(ts.sec)) * 1_000_000_000 + @as(u64, @intCast(ts.nsec));
}

pub fn main() !void {
    const num_leaves = getLeaves();

    // Total actors = sum of nodes at each level
    var total_actors: i64 = 0;
    var n = num_leaves;
    while (n >= 1) : (n = @divTrunc(n, 10)) {
        total_actors += n;
    }

    print("=== Zig Skynet Benchmark ===\n", .{});
    print("Leaves: {} (std.Thread, top {} levels parallel)\n\n", .{ num_leaves, THREAD_DEPTH });

    var root = SkynetArg{ .offset = 0, .size = num_leaves, .depth = 0 };

    const start = getTimeNs();
    const root_thread = try Thread.spawn(.{}, skynetThread, .{&root});
    root_thread.join();
    const end = getTimeNs();

    const elapsed_ns = @as(i64, @intCast(end - start));
    const elapsed_us = @divTrunc(elapsed_ns, 1000);

    print("Sum: {}\n", .{root.result});
    if (elapsed_us > 0) {
        const ns_per_msg = @divTrunc(elapsed_ns, total_actors);
        const throughput_m = @divTrunc(total_actors, elapsed_us);
        const leftover = total_actors - (throughput_m * elapsed_us);
        const frac = @divTrunc(leftover * 100, elapsed_us);
        print("ns/msg:         {}\n", .{ns_per_msg});
        print("Throughput:     {}.{:0>2} M msg/sec\n", .{ throughput_m, frac });
    }
}
