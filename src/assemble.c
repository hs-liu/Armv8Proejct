#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "symbol_table.h"
#include "assemble.h"
#include "assemble_branch.h"
#include "assemble_dt.h"
#include "assemble_dp.h"
#include "utils.h"

// Assemble a special instruction
void assemble_special_instruction(char *opcode, char *line, assembler_state_t *state) {
  if (strcmp(opcode, ".int") == 0) {
    // Add the integer to memory
    char *int_str = strtok(NULL, " ");
    if (int_str == NULL) {
      fprintf(stderr, "Invalid line: %s\n", line);
      fprintf(stderr, "Exiting!\n");
      exit(EXIT_FAILURE);
    }
    int32_t integer = strtoll(int_str, NULL, 0);
    memcpy(state->memory + state->address, &integer, WORD_SIZE_BYTES);
  } else if (strcmp(opcode, "nop") == 0) {
    // Add a nop to memory
    uint32_t nop = NOP_INSTRUCTION;
    memcpy(state->memory + state->address, &nop, WORD_SIZE_BYTES);
  }
  state->address += WORD_SIZE_BYTES;
}

void build_memory(char *line, void *data) {
  assembler_state_t *state = (assembler_state_t *)data;
  int len;
  line = strip_line(line, &len);

  // Skip empty lines
  if (*line == '\0') {
    return;
  }
  // Skip symbol declarations
  if (is_symbol_declaration(line, len)) {
    return;
  }

  // Tokenize the line
  char *opcode = strtok(line, " ");

  if (opcode == NULL) {
    fprintf(stderr, "Invalid line: %s\n", line);
    fprintf(stderr, "Exiting!\n");
    exit(EXIT_FAILURE);
  }

  printf("Opcode: %s\n", opcode);
  if (is_branch_opcode(opcode)) {
    assemble_branch_instruction(opcode, line, state);
  } else if (is_data_processing_opcode(opcode)) {
    assemble_data_processing_instruction(opcode, line, state);
  } else if (is_load_store_opcode(opcode)) {
    assemble_load_store_instruction(opcode, line, state);
  } else if (is_special_instruction(opcode)) {
    assemble_special_instruction(opcode, line, state);
  } else {
    fprintf(stderr, "Invalid opcode: %s\n", opcode);
    fprintf(stderr, "Exiting!\n");
    exit(EXIT_FAILURE);
  }
}

// Remove inline comments
void preprocess_line(char *buffer) {
  char *comment_start = strstr(buffer, "//");
  if (comment_start != NULL) {
    *comment_start = '\0';
  }
}

// Assembly file reader to read the file line by line
// and call the process_line function on it
void read_file(FILE *fp_in, process_line_fn process_line, void *data) {
  char buffer[MAX_LINE_LENGTH];
  int lineNum = 0;
  while (fgets(buffer, MAX_LINE_LENGTH, fp_in) != NULL) {
    int len = strlen(buffer);
    if (buffer[len - 1] != '\n') {
      fprintf(stderr, "Line too long! %d\n", lineNum);
      fprintf(stderr, "Exiting!\n");
      exit(EXIT_FAILURE);
    }
    buffer[len - 1] = '\0';
    preprocess_line(buffer);
    process_line(buffer, data);
    lineNum++;
  }
}

void write_file(char *out_file, uint8_t *data, int size) {
  FILE *fp_out = fopen(out_file, "wb");
  if (fp_out == NULL) {
    fprintf(stderr, "Couldn't open the file %s\n", out_file);
    fprintf(stderr, "Exiting!\n");
    exit(EXIT_FAILURE);
  }

  fwrite(data, size, 1, fp_out);
  fclose(fp_out);
}

int main(int argc, char **argv) {
  if (argc != 3 && argc != 4) {
    printf("Usage: ./assemble <file_in> <file_out> [<base_address>]\n");
    return EXIT_FAILURE;
  }

  uint64_t base_address = argc == 4 ? strtoll(argv[3], NULL, 0) : 0;

  assembler_state_t state = {
    .base_address = base_address,
    .address = 0,
    .symbol_table = hashmap_create(),
    .memory = NULL,
  };

  FILE *fp_in = fopen(argv[1], "r");
  if (fp_in == NULL) {
    fprintf(stderr, "Couldn't open the file %s\n", argv[1]);
    fprintf(stderr, "Exiting!\n");
    exit(EXIT_FAILURE);
  }

  // First pass: build symbol table
  read_file(fp_in, build_symbol_table, &state);

  // Second pass: build memory
  uint64_t binary_size = state.address;
  state.memory = malloc(binary_size);
  state.base_address = base_address;
  state.address = 0; //TODO: change to base address of program loaded into for rpi
  fseek(fp_in, 0, SEEK_SET);
  read_file(fp_in, build_memory, &state);
  fclose(fp_in);

  write_file(argv[2], state.memory, binary_size);

 return EXIT_SUCCESS;
}
