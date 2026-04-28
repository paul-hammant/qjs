# C API Examples

These examples demonstrate how to use Aether's runtime directly from C code.

## Examples

### Basic Usage
- **runtime_config_example.c** - Runtime configuration and auto-detection
  ```bash
  gcc -O2 -o runtime_config runtime_config_example.c ../../runtime/*.c -I../..
  ./runtime_config
  ```

### Advanced Features
- **high_throughput_actor.c** - SIMD batching + message coalescing
- **simd_batch_processing.c** - SIMD message processing
  ```bash
  gcc -O3 -march=native -mavx2 -o high_throughput high_throughput_actor.c -I../..
  ./high_throughput
  ```

### Performance Testing
- **multicore_bench.c** - Multi-core scheduler benchmark
- **ring_benchmark_manual.c** - Ring topology benchmark
- **state_machine_bench.c** - State machine performance
- **manual_actor_test.c** - Manual actor creation test

## Note

These are **C examples** for using the runtime directly. For **Aether language examples**, see `../../examples/` directory (.ae files).

## Compilation

All examples require:
- GCC or Clang with C11 support
- Runtime source files compiled and linked
- Include path to repository root (`-I../..`)

Optional (for SIMD examples):
- AVX2 support: `-march=native -mavx2`
