#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "symbol_table.h"
#include "utils.h"

#define MAX_LINE_LENGTH 1024

const char *data_processing_opcodes[] = {
  "add", "adds", "sub", "subs",
  "cmp", "cmn", 
  "neg", "negs",
  "and", "ands", "bic", "bics", "eor", "orr", "eon", "orn",
  "tst",
  "movk", "movn", "movz",
  "mov",
  "mvn",
  "madd", "msub",
  "mul", "mneg",
};

const char *branch_opcodes[] = {
  "b", "b.eq", "b.ne", "b.ge", "b.lt", "b.gt", "b.le", "b.al",
  "br",
};


const char *load_store_opcodes[] = {
  "str", "ldr",
};

const char *special[] = {
  ".int", "nop",
};

typedef void (*process_line_fn)(char *line, void *data);

typedef struct {
  uint64_t address;
  hashmap_t *symbol_table;
  uint8_t *memory;
} assembler_state_t;

// Tokenizer for breaking a line into its label, opcode and operand field



// Instruction assembler


// Strip line of trailing and leading whitespace
// returns resulting length of the line in second argument
char *strip_line(char *line, int *res_len) {
  int len = strlen(line);
  
  // Skip leading whitespace
  while (len > 0 && isspace(line[0])) {
    line++;
    len--;
  }
  // Skip trailing whitespace
  while (len > 0 && isspace(line[len - 1])) {
    len--;
    line[len] = '\0';
  }

  if (res_len != NULL) {
    *res_len = len;
  }
  return line;
}


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
void build_symbol_table(char *line, void *data) {
  assembler_state_t *state = (assembler_state_t *)data;
  int len;
  line = strip_line(line, &len);

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
    state->address += WORD_SIZE_BYTES;
  }
}

// Takes in null terminated opcode and
// returns true if it is a data processing opcode
bool is_data_processing_opcode(char *opcode) {
  for (int i = 0; i < sizeof(data_processing_opcodes) / sizeof(char *); i++) {
    if (strcmp(opcode, data_processing_opcodes[i]) == 0) {
      return true;
    }
  }
  return false;
}

bool is_branch_opcode(char *opcode) {
  for (int i = 0; i < sizeof(branch_opcodes) / sizeof(char *); i++) {
    if (strcmp(opcode, branch_opcodes[i]) == 0) {
      return true;
    }
  }
  return false;
}

bool is_load_store_opcode(char *opcode) {
  for (int i = 0; i < sizeof(load_store_opcodes) / sizeof(char *); i++) {
    if (strcmp(opcode, load_store_opcodes[i]) == 0) {
      return true;
    }
  }
  return false;
}

bool is_special_instruction(char *opcode) {
  for (int i = 0; i < sizeof(special) / sizeof(char *); i++) {
    if (strcmp(opcode, special[i]) == 0) {
      return true;
    }
  }
  return false;
}

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
    memcpy(state->memory + state->address, &integer, 4);
  } else if (strcmp(opcode, "nop") == 0) {
    // Add a nop to memory
    uint32_t nop = NOP_INSTRUCTION;
    memcpy(state->memory + state->address, &nop, 4);
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
  } else if (is_data_processing_opcode(opcode)) {
  } else if (is_load_store_opcode(opcode)) {
  } else if (is_special_instruction(opcode)) {
    assemble_special_instruction(opcode, line, state);
  } else {
    fprintf(stderr, "Invalid opcode: %s\n", opcode);
    fprintf(stderr, "Exiting!\n");
    exit(EXIT_FAILURE);
  }
}

// Assembly file reader to read the file line by line
// and call the process_line function on it
void read_file(FILE *fp_in, process_line_fn process_line, void *data) {
  char buffer[MAX_LINE_LENGTH];
  while (fgets(buffer, MAX_LINE_LENGTH, fp_in) != NULL) {
    int len = strlen(buffer);
    if (buffer[len - 1] != '\n') {
      fprintf(stderr, "Line too long!\n");
      fprintf(stderr, "Exiting!\n");
      exit(EXIT_FAILURE);
    }
    buffer[len - 1] = '\0';
    process_line(buffer, data);
  }
}

int main(int argc, char **argv) {
  if (argc != 3) {
    printf("Usage: ./assemble <file_in> <file_out>\n");
    return EXIT_FAILURE;
  }

  assembler_state_t state = {
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
  state.address = 0;
  fseek(fp_in, 0, SEEK_SET);
  read_file(fp_in, build_memory, &state);
  
  fclose(fp_in);


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
