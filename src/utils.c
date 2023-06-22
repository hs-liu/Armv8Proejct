#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

#include "symbol_table.h"
#include "assemble.h"
#include "utils.h"


const char *data_processing_multiply_opcodes[] = {
  "madd", "msub",
};

const char *data_processing_arithmetic_opcodes[] = {
  "add", "adds", "sub", "subs",
};

const char *data_processing_bit_logic_opcodes[] = {
  "and", "ands", "bic", "bics", "eor", "orr", "eon", "orn",
};

const char *data_processing_single_op_alias_opcodes[] = {
  "mov",
  "mul", "mneg",
  "neg", "negs",
  "mvn",
};

const char *data_processing_wide_move_opcodes[] = {
  "movk", "movn", "movz",
};

const char *data_processing_two_op_no_dest_opcodes[] = {
  "cmp", "cmn", 
  "tst",
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

  for (int i = 0; i < len; i++) {
    line[i] = tolower(line[i]);
  }

  return line;
}

bool is_immediate(char *operand) {
  return operand[0] == '#';
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

bool is_symbol_declaration(char *line, int len) {
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
    hashmap_set(state->symbol_table, symbol, state->base_address + state->address);
  } else {
    // Increment address
    state->address += WORD_SIZE_BYTES;
  }
}

bool is_data_processing_multiply_opcode(char *opcode) {
  for (int i = 0; i < sizeof(data_processing_multiply_opcodes) / sizeof(char *); i++) {
    if (strcmp(opcode, data_processing_multiply_opcodes[i]) == 0) {
      return true;
    }
  }
  return false;
}

bool is_data_processing_arithmetic_opcode(char *opcode) {
  for (int i = 0; i < sizeof(data_processing_arithmetic_opcodes) / sizeof(char *); i++) {
    if (strcmp(opcode, data_processing_arithmetic_opcodes[i]) == 0) {
      return true;
    }
  }
  return false;
}

bool is_data_processing_bit_logic_opcode(char *opcode) {
  for (int i = 0; i < sizeof(data_processing_bit_logic_opcodes) / sizeof(char *); i++) {
    if (strcmp(opcode, data_processing_bit_logic_opcodes[i]) == 0) {
      return true;
    }
  }
  return false;
}

bool is_data_processing_two_op_opcode(char *opcode) {
  return is_data_processing_arithmetic_opcode(opcode) ||
         is_data_processing_bit_logic_opcode(opcode);
}

bool is_data_processing_single_op_alias_opcode(char *opcode) {
  for (int i = 0; i < sizeof(data_processing_single_op_alias_opcodes) / sizeof(char *); i++) {
    if (strcmp(opcode, data_processing_single_op_alias_opcodes[i]) == 0) {
      return true;
    }
  }
  return false;
}

bool is_data_processing_wide_move_opcode(char *opcode) {
  for (int i = 0; i < sizeof(data_processing_wide_move_opcodes) / sizeof(char *); i++) {
    if (strcmp(opcode, data_processing_wide_move_opcodes[i]) == 0) {
      return true;
    }
  }
  return false;
}

bool is_data_processing_single_op_opcode(char *opcode) {
  return is_data_processing_single_op_alias_opcode(opcode) ||
    is_data_processing_wide_move_opcode(opcode);
}

bool is_data_processing_two_op_no_dest_opcode(char *opcode) {
  for (int i = 0; i < sizeof(data_processing_two_op_no_dest_opcodes) / sizeof(char *); i++) {
    if (strcmp(opcode, data_processing_two_op_no_dest_opcodes[i]) == 0) {
      return true;
    }
  }
  return false;
}

// Takes in null terminated opcode and
// returns true if it is a data processing opcode
bool is_data_processing_opcode(char *opcode) {
  return is_data_processing_multiply_opcode(opcode) ||
    is_data_processing_two_op_opcode(opcode) ||
    is_data_processing_single_op_opcode(opcode) ||
    is_data_processing_two_op_no_dest_opcode(opcode);
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

uint8_t get_register(char *reg_str, uint8_t *sf) {
  if (reg_str[0] != 'x' && reg_str[0] != 'w') {
    fprintf(stderr, "Invalid register: %s\n", reg_str);
    fprintf(stderr, "Exiting!\n");
    exit(EXIT_FAILURE);
  }
  
  if (reg_str[0] == 'x') {
    *sf = SF_64;
  } else {
    *sf = SF_32;
  }

  if (strcmp(reg_str + 1, "zr") == 0) {
    return ZR_REG;
  }
  uint8_t reg = atoi(reg_str + 1);
  if (reg > 31) {
    fprintf(stderr, "Invalid register: %s\n", reg_str);
    fprintf(stderr, "Exiting!\n");
    exit(EXIT_FAILURE);
  }
  return reg;
}
