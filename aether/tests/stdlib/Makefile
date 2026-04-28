# Aether Standard Library Test Makefile
# Cross-platform test compilation and execution

CC = gcc
CFLAGS = -I. -Istd -Iruntime -Itests/stdlib -std=c11 -Wall -Wextra
RUNTIME_SRC = runtime/memory.c

# Test executables
VECTOR_TEST = test_vector_run
HASHMAP_TEST = test_hashmap_run
SET_TEST = test_set_run

# Platform detection
ifeq ($(OS),Windows_NT)
    RM = del /Q
    EXE = .exe
    TEST_RUNNER = powershell -ExecutionPolicy Bypass -File tests/stdlib/run_tests.ps1
else
    RM = rm -f
    EXE =
    TEST_RUNNER = bash tests/stdlib/run_tests.sh
endif

.PHONY: all test clean vector hashmap set

# Default target
all: test

# Run all tests
test:
	@echo "Running all tests..."
	@$(TEST_RUNNER)

# Individual test targets
vector: $(VECTOR_TEST)$(EXE)
	@./$(VECTOR_TEST)$(EXE)

hashmap: $(HASHMAP_TEST)$(EXE)
	@./$(HASHMAP_TEST)$(EXE)

set: $(SET_TEST)$(EXE)
	@./$(SET_TEST)$(EXE)

# Compilation rules
$(VECTOR_TEST)$(EXE): tests/stdlib/test_vector.c std/collections/aether_vector.c $(RUNTIME_SRC)
	$(CC) $(CFLAGS) -o $@ $^

$(HASHMAP_TEST)$(EXE): tests/stdlib/test_hashmap.c std/collections/aether_hashmap.c $(RUNTIME_SRC)
	$(CC) $(CFLAGS) -o $@ $^

$(SET_TEST)$(EXE): tests/stdlib/test_set.c std/collections/aether_set.c std/collections/aether_hashmap.c $(RUNTIME_SRC)
	$(CC) $(CFLAGS) -o $@ $^

# Clean compiled tests
clean:
	@$(RM) $(VECTOR_TEST)$(EXE) $(HASHMAP_TEST)$(EXE) $(SET_TEST)$(EXE) 2>nul || true

# Help target
help:
	@echo "Aether Standard Library Test Suite"
	@echo ""
	@echo "Available targets:"
	@echo "  make test     - Run all tests (default)"
	@echo "  make vector   - Run only Vector tests"
	@echo "  make hashmap  - Run only HashMap tests"
	@echo "  make set      - Run only Set tests"
	@echo "  make clean    - Remove compiled test binaries"
	@echo "  make help     - Show this help message"
