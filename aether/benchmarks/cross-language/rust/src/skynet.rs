// Rust Skynet Benchmark
// Based on https://github.com/atemerev/skynet
// Uses std::thread for top levels (THREAD_DEPTH levels), sequential below.
// Spawning 1M OS threads is not feasible; this limits concurrent threads to ~1000.

use std::sync::mpsc::{channel, Sender};
use std::thread;
use std::time::Instant;

fn get_leaves() -> i64 {
    if let Ok(s) = std::env::var("SKYNET_LEAVES") {
        if let Ok(n) = s.parse() {
            return n;
        }
    }
    if let Ok(s) = std::env::var("BENCHMARK_MESSAGES") {
        if let Ok(n) = s.parse() {
            return n;
        }
    }
    1_000_000
}

// Below THREAD_DEPTH, compute the sub-tree sum sequentially on the current thread.
const THREAD_DEPTH: usize = 3;

fn skynet_seq(offset: i64, size: i64) -> i64 {
    if size == 1 {
        return offset;
    }
    let child_size = size / 10;
    let mut sum = 0i64;
    for i in 0..10i64 {
        sum += skynet_seq(offset + i * child_size, child_size);
    }
    sum
}

fn skynet(tx: Sender<i64>, offset: i64, size: i64, depth: usize) {
    if size == 1 || depth >= THREAD_DEPTH {
        tx.send(skynet_seq(offset, size)).unwrap();
        return;
    }
    let child_size = size / 10;
    let (child_tx, child_rx) = channel::<i64>();
    for i in 0..10i64 {
        let child_offset = offset + i * child_size;
        let child_tx = child_tx.clone();
        thread::spawn(move || {
            skynet(child_tx, child_offset, child_size, depth + 1);
        });
    }
    let mut sum = 0i64;
    for _ in 0..10 {
        sum += child_rx.recv().unwrap();
    }
    tx.send(sum).unwrap();
}

fn main() {
    let num_leaves = get_leaves();

    // Total actors = sum of nodes at each level
    let mut total_actors = 0i64;
    let mut n = num_leaves;
    while n >= 1 {
        total_actors += n;
        n /= 10;
    }

    println!("=== Rust Skynet Benchmark ===");
    println!("Leaves: {} (std::thread, top {} levels parallel)\n", num_leaves, THREAD_DEPTH);

    let (tx, rx) = channel::<i64>();
    let start = Instant::now();
    thread::spawn(move || skynet(tx, 0, num_leaves, 0));
    let sum = rx.recv().unwrap();
    let elapsed = start.elapsed();

    let elapsed_ns = elapsed.as_nanos() as i64;
    let elapsed_us = elapsed_ns / 1000;

    println!("Sum: {}", sum);
    if elapsed_us > 0 {
        let ns_per_msg = elapsed_ns / total_actors;
        let throughput_m = total_actors / elapsed_us;
        let leftover = total_actors - (throughput_m * elapsed_us);
        let throughput_frac = (leftover * 100) / elapsed_us;
        print!("ns/msg:         {}\n", ns_per_msg);
        print!("Throughput:     {}.", throughput_m);
        if throughput_frac < 10 {
            print!("0");
        }
        println!("{} M msg/sec", throughput_frac);
    }
}
