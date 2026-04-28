// Go Skynet Benchmark
// Based on https://github.com/atemerev/skynet
// Recursive tree of goroutines: root spawns 10 children, each spawns 10 more, etc.
// Leaves report their offset; parents aggregate 10 children's results and report up.
package main

import (
	"fmt"
	"os"
	"strconv"
	"time"
)

func getLeaves() int64 {
	if env := os.Getenv("SKYNET_LEAVES"); env != "" {
		if n, err := strconv.ParseInt(env, 10, 64); err == nil {
			return n
		}
	}
	if env := os.Getenv("BENCHMARK_MESSAGES"); env != "" {
		if n, err := strconv.ParseInt(env, 10, 64); err == nil {
			return n
		}
	}
	return 1000000
}

// skynetNode sends its subtree sum to the result channel.
// Leaves send their offset directly; internal nodes spawn 10 children and sum.
func skynetNode(result chan<- int64, offset, size int64) {
	if size == 1 {
		result <- offset
		return
	}
	children := make(chan int64, 10)
	childSize := size / 10
	for i := int64(0); i < 10; i++ {
		go skynetNode(children, offset+i*childSize, childSize)
	}
	var sum int64
	for i := 0; i < 10; i++ {
		sum += <-children
	}
	result <- sum
}

func main() {
	numLeaves := getLeaves()

	// Total actors = sum of nodes at each level
	totalActors := int64(0)
	n := numLeaves
	for n >= 1 {
		totalActors += n
		n /= 10
	}

	fmt.Println("=== Go Skynet Benchmark ===")
	fmt.Printf("Leaves: %d\n\n", numLeaves)

	root := make(chan int64, 1)
	start := time.Now()
	go skynetNode(root, 0, numLeaves)
	sum := <-root
	elapsed := time.Since(start)

	elapsedNs := elapsed.Nanoseconds()
	elapsedUs := elapsedNs / 1000

	fmt.Printf("Sum: %d\n", sum)
	if elapsedUs > 0 {
		nsPerMsg := elapsedNs / totalActors
		throughputM := totalActors / elapsedUs
		leftover := totalActors - (throughputM * elapsedUs)
		throughputFrac := (leftover * 100) / elapsedUs
		fmt.Printf("ns/msg:         %d\n", nsPerMsg)
		fmt.Printf("Throughput:     %d.", throughputM)
		if throughputFrac < 10 {
			fmt.Printf("0")
		}
		fmt.Printf("%d M msg/sec\n", throughputFrac)
	}
}
