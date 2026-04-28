#ifndef AETHER_DIAGNOSTICS_H
#define AETHER_DIAGNOSTICS_H

#include "aether_error.h"

// Enhanced diagnostic system with suggestions and context

#define MAX_CONTEXT_LINES 3
#define MAX_LINE_LENGTH 256

// Error codes for documentation linking
typedef enum {
    ERR_PARSE_UNEXPECTED_TOKEN = 1001,
    ERR_PARSE_EXPECTED_SEMICOLON = 1002,
    ERR_PARSE_EXPECTED_IDENTIFIER = 1003,
    ERR_TYPE_MISMATCH = 2001,
    ERR_UNDEFINED_VARIABLE = 2002,
    ERR_UNDEFINED_FUNCTION = 2003,
    ERR_UNDEFINED_TYPE = 2004,
    ERR_INVALID_OPERATION = 2005,
    ERR_CODEGEN_UNSUPPORTED = 3001,
    ERR_CODEGEN_INTERNAL = 3002
} ErrorCode;

// Diagnostic with enhanced information
typedef struct {
    ErrorCode code;
    const char* message;
    const char* suggestion;  // "Did you mean...?" suggestion
    int line;
    int column;
    const char* filename;
    // Source context (up to 3 lines before/after for better context)
    char context_before[MAX_CONTEXT_LINES][MAX_LINE_LENGTH];
    int context_before_count;
    char context_line[MAX_LINE_LENGTH];
    char context_after[MAX_CONTEXT_LINES][MAX_LINE_LENGTH];
    int context_after_count;
} EnhancedDiagnostic;

// Initialize diagnostics system
void diagnostics_init(const char* source_code, const char* filename);

// Report enhanced error with suggestions
void report_error_enhanced(ErrorCode code, int line, int column, 
                          const char* message, const char* suggestion);

// Find similar identifier (for "did you mean" suggestions)
const char* find_similar_identifier(const char* typo, const char** valid_ids, int count);

// Calculate Levenshtein distance for string similarity
int levenshtein_distance(const char* s1, const char* s2);

// Print error with ANSI colors and context
void print_diagnostic_colored(EnhancedDiagnostic* diag);

// Get error documentation URL
const char* get_error_docs_url(ErrorCode code);

#endif // AETHER_DIAGNOSTICS_H

