#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "symbol_table.h"
#include "assemble.h"
#include "utils.h"

// Assemble a Load / Store instrcution
void assemble_load_store_instruction(char *opcode, char *line, assembler_state_t *state) {
  if (strcmp(opcode, "ldr") == 0) {
    char *rt_str = strtok(NULL, ",");
    char *address_str = strtok(NULL, ",");
    rt_str = strip_line(rt_str, NULL);
    int address_str_len;
    address_str = strip_line(address_str, &address_str_len);

    if (rt_str == NULL || address_str == NULL) {
      fprintf(stderr, "Invalid line: %s\n", line);
      fprintf(stderr, "Exiting!\n");
      exit(EXIT_FAILURE);
    }

    uint32_t instruction = 0;
    uint8_t sf = SF_64;
    uint8_t rt = get_register(rt_str, &sf);
    SET_BITS(instruction, DT_RT_OFFSET, rt);
    SET_BITS(instruction, DT_SF_OFFSET, sf);

    uint64_t transfer_address = 0;
    // Check if the address is a literal
    if (address_str[0] == '#') {
      SET_BITS(instruction, 0, LOAD_LIT_ENCODING);
      transfer_address = strtoll(address_str + 1, NULL, 0);
      SET_BITS(instruction, DT_SIMM19_OFFSET, transfer_address);
    }
    else if (is_symbol(address_str, address_str_len)) {
      SET_BITS(instruction, 0, LOAD_LIT_ENCODING);
      hashmap_entry_t *entry = hashmap_find_entry(state->symbol_table, address_str);
      if (entry == NULL) {
        fprintf(stderr, "Symbol '%s' not found!\n", address_str);
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
      }
      transfer_address = entry->value;
      SET_BITS(instruction, DT_SIMM19_OFFSET, transfer_address);
    }


    memcpy(state->memory + state->address, &instruction, WORD_SIZE_BYTES);
  }
}

