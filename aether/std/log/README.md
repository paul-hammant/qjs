# Aether Logging Library

Structured logging system with multiple log levels, colored output, and flexible configuration.

## Features

- **Multiple Log Levels:** DEBUG, INFO, WARN, ERROR, FATAL
- **Colored Output:** ANSI color codes for better readability (Linux/macOS)
- **Timestamps:** Automatic timestamp generation
- **Source Location:** Optional file:line:function tracking
- **File Output:** Write logs to files
- **Statistics:** Track log message counts by level
- **Zero Allocations:** Stack-based formatting
- **Thread-Safe:** Uses atomic operations for multi-threaded programs

## Quick Start

```c
#include "std/log/aether_log.h"

int main() {
    // Initialize logging
    aether_log_init("app.log", LOG_LEVEL_INFO);
    
    LOG_INFO("Application started");
    LOG_WARN("This is a warning");
    LOG_ERROR("Something went wrong");
    
    aether_log_shutdown();
    return 0;
}
```

## Log Levels

```c
LOG_DEBUG("Debug information");  // Detailed debugging
LOG_INFO("Info message");        // General information
LOG_WARN("Warning");              // Warning conditions
LOG_ERROR("Error occurred");      // Error conditions
LOG_FATAL("Fatal error");         // Fatal errors (aborts program)
```

## Source Location

Include source location (file, line, function):

```c
LOG_DEBUG_LOC("Entering function");
LOG_ERROR_LOC("Validation failed");
```

Output:
```
[2025-12-29 10:30:45] ERROR: [main.c:42 in validate_input] Validation failed
```

## Configuration

### Basic Configuration

```c
aether_log_init("myapp.log", LOG_LEVEL_DEBUG);
aether_log_set_colors(1);         // Enable colors
aether_log_set_timestamps(1);     // Show timestamps
```

### Advanced Configuration

```c
AetherLogConfig config = {
    .min_level = LOG_LEVEL_INFO,
    .output_file = fopen("app.log", "a"),
    .use_colors = 1,
    .show_timestamps = 1,
    .show_source_location = 0
};
aether_log_init_with_config(&config);
```

## Output Formats

### With Colors (Default)
```
[2025-12-29 10:30:45] INFO: Application started
[2025-12-29 10:30:46] WARN: Cache miss detected  
[2025-12-29 10:30:47] ERROR: Connection refused
```

### With Source Location
```
[2025-12-29 10:30:45] DEBUG: [worker.c:23 in process_task] Processing task 42
[2025-12-29 10:30:46] ERROR: [network.c:156 in connect_socket] Socket error
```

## Statistics

Track log message counts:

```c
AetherLogStats stats = aether_log_get_stats();
printf("Errors: %zu, Warnings: %zu\n", stats.error_count, stats.warn_count);

// Or print formatted stats
aether_log_print_stats();
```

Output:
```
========== Logging Statistics ==========
DEBUG: 42
INFO:  15
WARN:  3
ERROR: 1
FATAL: 0
========================================
```

## Build Demo

**Windows:**
```powershell
gcc -O2 -I. std/log/demo_log.c std/log/aether_log.c -o demo_log.exe
./demo_log.exe
```

**Linux/macOS:**
```bash
gcc -O2 -I. std/log/demo_log.c std/log/aether_log.c -o demo_log
./demo_log
```

## Integration with Aether Projects

Add to your Makefile:

```makefile
LDFLAGS += std/log/aether_log.c
```

## Future Enhancements

- [ ] Thread-safe logging with mutex
- [ ] Async logging (background thread)
- [ ] Log rotation (max file size, date-based)
- [ ] Custom formatters (JSON, structured)
- [ ] Syslog backend support
- [ ] Network logging (UDP/TCP)
- [ ] Filtering by category/tag

## API Reference

### Initialization
- `aether_log_init(filename, min_level)` - Initialize logging
- `aether_log_shutdown()` - Close log files

### Logging
- `LOG_DEBUG(fmt, ...)` - Debug level
- `LOG_INFO(fmt, ...)` - Info level
- `LOG_WARN(fmt, ...)` - Warning level
- `LOG_ERROR(fmt, ...)` - Error level
- `LOG_FATAL(fmt, ...)` - Fatal level (aborts)

### Configuration
- `aether_log_set_level(level)` - Change minimum log level
- `aether_log_set_colors(enabled)` - Toggle colors
- `aether_log_set_timestamps(enabled)` - Toggle timestamps

### Statistics
- `aether_log_get_stats()` - Get log statistics
- `aether_log_print_stats()` - Print formatted statistics

