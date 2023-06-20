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

/**
 * returns true if the given string is a load literal instruction
 * 
 * @param address_str the string containing the address
 * @param address_str_len the length of the address string
 * @return true if the given string is a load literal instruction
 */
bool is_load_literal_instruction(char *address_str, int address_str_len) {
    return address_str[0] == '#' || is_symbol(address_str, address_str_len);
}

/**
 * returns true if the given string is a pre-index addressing instruction
 * 
 * @param next_token the string containing the next token
 * @return true if the given string is a pre-index addressing instruction
 */
bool is_pre_index_addressing(char *next_token) {
    return next_token != NULL && next_token[0] == '!';
}

/**
 * returns true if the given string is a post-index addressing instruction
 * 
 * @param next_token the string containing the next token
 * @return true if the given string is a post-index addressing instruction
 */
bool is_post_index_addressing(char *next_token) {
    return next_token != NULL && next_token[0] == '#';
}

/**
 * returns true if the given string is a register offset addressing instruction
 * 
 * @param offset_str the string containing the offset
 * @return true if the given string is a register offset addressing instruction
 */
bool is_register_offset_addressing(char *offset_str) {
    return offset_str != NULL && offset_str[0] == 'x';
}

/**
 * returns true if the given string is a unsigned offset addressing instruction
 * 
 * @param offset_str the string containing the offset
 * @return true if the given string is a signed offset addressing instruction
 */
bool is_unsigned_offset_addressing(char *offset_str) {
    return offset_str != NULL && offset_str[0] == '#';
}

/**
 * returns true if the given string is a zero unsigned offset addressing instruction
 * 
 * @param offset_str the string containing the offset
 * @return true if the given string is a zero unsigned offset addressing instruction
 */
bool is_zero_unsigned_offset(char *offset_str) {
    return offset_str == NULL;
}

/**
 * assembles a load literal instruction
 * 
 * @param address_str 
 * @param instruction 
 * @param state 
 */
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

 
    transfer_address = transfer_address - (state->base_address + state->address);
    transfer_address = transfer_address / WORD_SIZE_BYTES;
    SET_BITS(*instruction, DT_SIMM19_OFFSET, DT_SIMM19_SIZE, transfer_address);
}

/**
 * assembles a pre-index addressing instruction
 * 
 * @param address_str 
 * @param instruction 
 * @param state 
 */
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

/**
 * assembles a post-index addressing instruction
 * 
 * @param xn_str 
 * @param simm_str 
 * @param instruction 
 * @param state 
 */
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

/**
 * assembles a register offset addressing instruction
 * 
 * @param xn_str
 * @param xm_str 
 * @param instruction 
 * @param state 
 */
void assemble_register_offset_addressing_instruction(
    char *xn_str,
    char *xm_str,
    uint32_t *instruction,
    assembler_state_t *state
) {
    SET_BITS(*instruction, ENCODING_OFFSET, ENCODING_SIZE, REG_OFFSET_ENCODING);
    uint8_t sf = SF_64;
    uint8_t xn = get_register(xn_str, &sf);
    if (sf != SF_64) {
        fprintf(stderr, "Invalid index register: %s\n", xn_str);
        fprintf(stderr, "Address register must be an X register!");
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }
    SET_BITS(*instruction, XN_OFFSET, XN_SIZE, xn);

    uint8_t xm = get_register(xm_str, &sf);
    if (sf != SF_64) {
        fprintf(stderr, "Invalid offset register: %s\n", xm_str);
        fprintf(stderr, "Offset register must be an X register!");
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }
    SET_BITS(*instruction, XM_OFFSET, XM_SIZE, xm);
}

/**
 * assembles a unsigned offset addressing instruction
 * 
 * @param xn_str 
 * @param offset_str 
 * @param instruction 
 * @param state 
 */
void assemble_unsigned_offset_addressing_instruction(
    char *xn_str,
    char *offset_str,
    uint8_t rt_sf,
    uint32_t *instruction,
    assembler_state_t *state
) {
    SET_BITS(*instruction, ENCODING_OFFSET, ENCODING_SIZE, SDT_UNSIGNED_OFFSET_ENCODING);
    SET_BITS(*instruction, SDT_U_OFFSET, SDT_U_SIZE, SDT_UNSIGNED_OFFSET_U);
    uint8_t sf = SF_64;
    uint8_t xn = get_register(xn_str, &sf);
    if (sf != SF_64) {
        fprintf(stderr, "Invalid index register: %s\n", xn_str);
        fprintf(stderr, "Address register must be an X register!");
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }
    SET_BITS(*instruction, XN_OFFSET, XN_SIZE, xn);

    assert(offset_str[0] == '#');
    uint64_t offset = strtoll(offset_str + 1, NULL, 0);
    if (rt_sf == SF_64) {
        if (offset % BYTES_IN_64 != 0) {
            fprintf(stderr, "Immediate '%s' is not a multiple of 8!\n", offset_str);
            fprintf(stderr, "Exiting!\n");
            exit(EXIT_FAILURE);
        }
        offset /= BYTES_IN_64;
    } else if (rt_sf == SF_32) {
        if (offset % BYTES_IN_32 != 0) {
            fprintf(stderr, "Immediate '%s' is not a multiple of 4!\n", offset_str);
            fprintf(stderr, "Exiting!\n");
            exit(EXIT_FAILURE);
        }
        offset /= BYTES_IN_32;
    }   

    if (offset > MAX_UIMM12) {
        fprintf(stderr, "Immediate '%s' is not a valid 12-bit unsigned immediate!\n", offset_str);
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }

    SET_BITS(*instruction, SDT_IMM12_OFFSET , SDT_IMM12_SIZE, offset);
}

/**
 * handles unsigned offset addressing
 * 
 * @param address_str 
 * @param rt_sf 
 * @param instruction 
 * @param state 
 */
void handle_offset_addressing(
    char *address_str,
    uint8_t rt_sf,
    uint32_t *instruction,
    assembler_state_t *state
) {
    if (address_str[0] != '[') {
        fprintf(stderr, "Invalid address: %s\n", address_str);
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }

    char *xn_str = strtok(address_str + 1, ",");
    char *offset_str = strtok(NULL, "]");
    if (offset_str != NULL) {
        offset_str = strip_line(offset_str, NULL);
    }

    if (is_register_offset_addressing(offset_str)) {
        assemble_register_offset_addressing_instruction(xn_str, offset_str, instruction, state);
    }
    else if (is_unsigned_offset_addressing(offset_str)) {
        assemble_unsigned_offset_addressing_instruction(xn_str, offset_str, rt_sf, instruction, state);
    }
    else if (is_zero_unsigned_offset(offset_str)) {
        assemble_unsigned_offset_addressing_instruction(xn_str, "#0", rt_sf, instruction, state);
    }
}

/**
 * assembles a load / store instruction
 * 
 * @param opcode 
 * @param line 
 * @param state 
 */
void assemble_load_store_instruction(char *opcode, char *line, assembler_state_t *state) {
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

    if (strcmp(opcode, "ldr") == 0 && is_load_literal_instruction(address_str, address_str_len)) {
        assemble_load_literal_instruction(address_str, &instruction, state);
    }
    else {
        // Single Data Transfer instruction
        uint8_t l = strcmp(opcode, "ldr") == 0 ? LOAD_L : STORE_L;
        SET_BITS(instruction, SDT_L_OFFSET, SDT_L_SIZE, l);
        if (is_pre_index_addressing(next_token)) {
            assemble_pre_index_addressing_instruction(address_str, &instruction, state);
        }
        else if (is_post_index_addressing(next_token)) {
            assemble_post_index_addressing_instruction(address_str, next_token, &instruction, state);
        }
        else {
            handle_offset_addressing(address_str, sf, &instruction, state);
        }
    }

    memcpy(state->memory + state->address, &instruction, WORD_SIZE_BYTES);
    state->address += WORD_SIZE_BYTES;
}
