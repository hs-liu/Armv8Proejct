#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "symbol_table.h"
#include "assemble.h"
#include "utils.h"

bool is_load_literal_instruction(char *address_str, int address_str_len) {
    return address_str[0] == '#' || is_symbol(address_str, address_str_len);
}

bool is_pre_index_addressing(char *next_token) {
    return next_token != NULL && next_token[0] == '!';
}

bool is_post_index_addressing(char *next_token) {
    return next_token != NULL && next_token[0] == '#';
}

void assemble_load_literal_instruction(char *address_str, uint32_t *instruction, assembler_state_t *state) {
    uint64_t transfer_address = 0;
    if (address_str[0] == '#') {
        SET_BITS(*instruction, ENCODING_OFFSET, ENCODING_SIZE, LOAD_LIT_ENCODING);
        transfer_address = strtoll(address_str + 1, NULL, 0);
    }
    else {
        // Address is a symbol
        SET_BITS(*instruction, ENCODING_OFFSET, ENCODING_SIZE, LOAD_LIT_ENCODING);
        hashmap_entry_t *entry = hashmap_find_entry(state->symbol_table, address_str);
        if (entry == NULL) {
            fprintf(stderr, "Symbol '%s' not found!\n", address_str);
            fprintf(stderr, "Exiting!\n");
            exit(EXIT_FAILURE);
        }
        transfer_address = entry->value;
    }

    if ((abs(transfer_address - state->address) >= MB_SIZE) ||
        (transfer_address % WORD_SIZE_BYTES != 0)) {
        fprintf(stderr, "Address '%s' is not within 1MB of the current address!\n", address_str);
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }
 
    transfer_address = transfer_address - state->address;
    transfer_address = transfer_address / WORD_SIZE_BYTES;
    SET_BITS(*instruction, DT_SIMM19_OFFSET, DT_SIMM19_SIZE, transfer_address);
}

void assemble_pre_index_addressing_instruction(char *address_str, uint32_t *instruction, assembler_state_t *state) {
    SET_BITS(*instruction, ENCODING_OFFSET, ENCODING_SIZE, PRE_POST_ENCODING);
    SET_BITS(*instruction, PRE_POST_I_OFFSET, PRE_POST_I_SIZE, PRE_INDEX_I);
    if (address_str[0] != '[') {
        fprintf(stderr, "Invalid address: %s\n", address_str);
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }

    char *xn_str = strtok(address_str + 1, ",");
    char *simm_str = strtok(NULL, "]");
    xn_str = strip_line(xn_str, NULL);
    simm_str = strip_line(simm_str, NULL);

    uint8_t sf = SF_64;
    uint8_t xn = get_register(xn_str, &sf);
    if (sf != SF_64) {
        fprintf(stderr, "Invalid index register: %s\n", xn_str);
        fprintf(stderr, "Address register must be an X register!");
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }
    SET_BITS(*instruction, SDT_XN_OFFSET, SDT_XN_SIZE, xn);

    if (simm_str[0] != '#') {
        fprintf(stderr, "Invalid immediate: %s\n", simm_str);
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }
    int64_t simm = strtoll(simm_str + 1, NULL, 0);
    if (simm < MIN_SIMM9 || simm > MAX_SIMM9) {
        fprintf(stderr, "Immediate '%s' is not a valid 9-bit signed immediate!\n", simm_str);
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }
    SET_BITS(*instruction, PRE_POST_SIMM9_OFFSET, PRE_POST_SIMM9_SIZE, simm);
}

void assemble_post_index_addressing_instruction(
    char *xn_str,
    char *simm_str,
    uint32_t *instruction,
    assembler_state_t *state
) {
    SET_BITS(*instruction, ENCODING_OFFSET, ENCODING_SIZE, PRE_POST_ENCODING);
    SET_BITS(*instruction, PRE_POST_I_OFFSET, PRE_POST_I_SIZE, POST_INDEX_I);

    if (xn_str[0] != '[') {
        fprintf(stderr, "Invalid register address: %s\n", xn_str);
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }
    uint8_t sf = SF_64;
    uint8_t xn = get_register(xn_str + 1, &sf);
    if (sf != SF_64) {
        fprintf(stderr, "Invalid index register: %s\n", xn_str);
        fprintf(stderr, "Address register must be an X register!");
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }
    SET_BITS(*instruction, SDT_XN_OFFSET, SDT_XN_SIZE, xn);


    assert(simm_str[0] == '#');
    int64_t simm = strtoll(simm_str + 1, NULL, 0);
    if (simm < MIN_SIMM9 || simm > MAX_SIMM9) {
        fprintf(stderr, "Immediate '%s' is not a valid 9-bit signed immediate!\n", simm_str);
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }
    SET_BITS(*instruction, PRE_POST_SIMM9_OFFSET, PRE_POST_SIMM9_SIZE, simm);
}

// Assemble a Load / Store instrucution
void assemble_load_store_instruction(char *opcode, char *line, assembler_state_t *state) {
  if (strcmp(opcode, "ldr") == 0) {
    char *rt_str = strtok(NULL, ",");
    char *address_str = strtok(NULL, "]");
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
    SET_BITS(instruction, DT_RT_OFFSET, DT_RT_SIZE, rt);
    SET_BITS(instruction, DT_SF_OFFSET, DT_SF_SIZE, sf);

    char *next_token = strtok(NULL, ",");
    if (next_token != NULL) {
        next_token = strip_line(next_token, NULL);
    }

    if (is_load_literal_instruction(address_str, address_str_len)) {
        assemble_load_literal_instruction(address_str, &instruction, state);
    }
    else if (is_pre_index_addressing(next_token)) {
        SET_BITS(instruction, SDT_L_OFFSET, SDT_L_SIZE, LOAD_L);
        assemble_pre_index_addressing_instruction(address_str, &instruction, state);
    }
    else if (is_post_index_addressing(next_token)) {
        SET_BITS(instruction, SDT_L_OFFSET, SDT_L_SIZE, LOAD_L);
        assemble_post_index_addressing_instruction(address_str, next_token, &instruction, state);

    }

    memcpy(state->memory + state->address, &instruction, WORD_SIZE_BYTES);
  }
}

