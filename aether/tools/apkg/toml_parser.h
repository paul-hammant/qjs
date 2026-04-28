#ifndef TOML_PARSER_H
#define TOML_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char* key;
    char* value;
} TomlKeyValue;

typedef struct {
    char* name;
    TomlKeyValue* entries;
    int entry_count;
} TomlSection;

typedef struct {
    TomlSection* sections;
    int section_count;
} TomlDocument;

// Parse TOML file
TomlDocument* toml_parse_file(const char* path);

// Get value from section
const char* toml_get_value(TomlDocument* doc, const char* section, const char* key);

// Get all values in a section
TomlKeyValue* toml_get_section_entries(TomlDocument* doc, const char* section, int* count);

// Free document
void toml_free_document(TomlDocument* doc);

#endif
