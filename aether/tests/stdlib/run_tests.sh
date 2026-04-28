#!/usr/bin/env bash
# Aether Standard Library Test Runner (Unix/Linux/macOS)

# Change to project root directory (tests/stdlib -> project root)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
cd "$PROJECT_ROOT"

echo ""
echo "=================================="
echo "  Aether Standard Library Tests"
echo "=================================="

passed=0
failed=0

# Compiler flags
CFLAGS="-I. -Istd -Iruntime -Itests/stdlib -std=c11 -Wall -Wextra"

# Test Vector
echo ""
echo "Compiling Vector tests..."
if gcc -o test_vector_run tests/stdlib/test_vector.c std/collections/aether_vector.c runtime/memory/memory.c $CFLAGS 2>&1; then
    echo "Running Vector tests..."
    if ./test_vector_run; then
        ((passed++))
    else
        ((failed++))
    fi
else
    echo "FAILED to compile Vector tests"
    ((failed++))
fi

# Test HashMap
echo ""
echo "Compiling HashMap tests..."
if gcc -o test_hashmap_run tests/stdlib/test_hashmap.c std/collections/aether_hashmap.c runtime/memory/memory.c $CFLAGS 2>&1; then
    echo "Running HashMap tests..."
    if ./test_hashmap_run; then
        ((passed++))
    else
        ((failed++))
    fi
else
    echo "FAILED to compile HashMap tests"
    ((failed++))
fi

# Test Set
echo ""
echo "Compiling Set tests..."
if gcc -o test_set_run tests/stdlib/test_set.c std/collections/aether_set.c std/collections/aether_hashmap.c runtime/memory/memory.c $CFLAGS 2>&1; then
    echo "Running Set tests..."
    if ./test_set_run; then
        ((passed++))
    else
        ((failed++))
    fi
else
    echo "FAILED to compile Set tests"
    ((failed++))
fi

# Summary
echo ""
echo "=================================="
echo "  Test Summary"
echo "=================================="
echo "Passed: $passed"
echo "Failed: $failed"
echo "=================================="

# Cleanup
rm -f test_vector_run test_hashmap_run test_set_run

if [ $failed -eq 0 ]; then
    echo ""
    echo "All tests passed!"
    exit 0
else
    echo ""
    echo "Some tests failed."
    exit 1
fi
