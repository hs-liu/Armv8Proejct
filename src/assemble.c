#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "symbol_table.h"

#define MAX_LINE_LENGTH 1024

typedef void (*process_line_fn)(char *line, int len, void *data);

typedef struct {
  uint64_t address;
  hashmap_t *symbol_table;
} assembler_state_t;

// Tokenizer for breaking a line into its label, opcode and operand field



// Instruction assembler


// Check if the string (may not be null terminated) is a symbol
bool is_symbol(char *symbol, int len) {
  if (len <= 0) {
    return false;
  }
  if (!isalpha(symbol[0]) && symbol[0] != '_' && symbol[0] != '.') {
    return false;
  }
  for (int i = 1; i < len; i++) {
    if (!isalnum(symbol[i]) && symbol[i] != '_' && symbol[i] != '.' && symbol[i] != '$') {
      return false;
    }
  }
  return true;
}

static inline bool is_symbol_declaration(char *line, int len) {
  return is_symbol(line, len - 1) && line[len - 1] == ':';
}

// Building symbol table
// Line should be null terminated and should not have a trailing newline
void build_symbol_table(char *line, int len, void *data) {
  assembler_state_t *state = (assembler_state_t *)data;
  // Skip trailing whitespace
  while (*line == ' ' || *line == '\t') {
    line++;
    len--;
  }
  // Skip empty lines
  if (*line == '\0') {
    return;
  }

  if (is_symbol_declaration(line, len)) {
    // Add symbol to symbol table
    char *symbol = line;
    symbol[len - 1] = '\0';
    hashmap_set(state->symbol_table, symbol, state->address);
  } else {
    // Increment address
    state->address += 4;
  }
}


// Assembly file reader to read the file line by line
// and split it up into a list of lines
void read_file(char *in_file, process_line_fn process_line, void *data) {
  FILE *fp_in = fopen(in_file, "r");
  if (fp_in == NULL) {
    fprintf(stderr, "Couldn't open the file %s\n", in_file);
    fprintf(stderr, "Exiting!\n");
    exit(EXIT_FAILURE);
  }
  char buffer[MAX_LINE_LENGTH];
  while (fgets(buffer, MAX_LINE_LENGTH, fp_in) != NULL) {
    int len = strlen(buffer);
    if (buffer[len - 1] != '\n') {
      fprintf(stderr, "Line too long!\n");
      fprintf(stderr, "Exiting!\n");
      exit(EXIT_FAILURE);
    }
    buffer[len - 1] = '\0';
    process_line(buffer, len - 1, data);
  }
}

// TODO: Implement assemble() function
int main(int argc, char **argv) {
  if (argc != 3) {
    printf("Usage: ./assemble <file_in> <file_out>\n");
    return EXIT_FAILURE;
  }

  assembler_state_t state = {
    .address = 0,
    .symbol_table = hashmap_create(),
  };

  read_file(argv[1], build_symbol_table, &state);

  printf("hashmap_get(state.symbol_table, main) = %llx", hashmap_find_entry(state.symbol_table, "main")->value);

  // char *in_file = argv[1];
  // char *out_file = argv[2];
  // FILE *fp_out = fopen(out_file, "wb");
  // if (fp_out == NULL) {
  //   fprintf(stderr, "Couldn't open the file %s\n", out_file);
  //   fprintf(stderr, "Exiting!\n");
  //   exit(EXIT_FAILURE);
  // }


  return EXIT_SUCCESS;
}
