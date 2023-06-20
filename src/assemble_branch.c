#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "symbol_table.h"
#include "assemble.h"
#include "assemble_branch.h"
#include "utils.h"

/**
 * Returns the offset of the target address from the current address
 * 
 * @param target_str the string containing the target address
 * @param target_str_len the length of the target string
 * @param state the assembler state
 * @return the offset of the target address from the current address
 */
int64_t get_target_address_offset(char *target_str, int target_str_len, assembler_state_t *state) {
    uint64_t target_address = 0;
    if (is_symbol(target_str, target_str_len)) {
        hashmap_entry_t *entry = hashmap_find_entry(state->symbol_table, target_str);
        if (entry == NULL) {
            fprintf(stderr, "Symbol '%s' not found!\n", target_str);
            fprintf(stderr, "Exiting!\n");
            exit(EXIT_FAILURE);
        }
        target_address = entry->value;
    }
    else {
        target_address = strtoll(target_str, NULL, 0);
    }

    if (target_address % WORD_SIZE_BYTES != 0) {
        fprintf(stderr, "Symbol '%s' not word aligned!\n", target_str);
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }

    return (target_address - (state->base_address + state->address)) / WORD_SIZE_BYTES;
}

/**
 * Assembles an unconditional branch instruction
 * 
 * @param target_str the string containing the target address
 * @param target_str_len the length of the target string
 * @param line the line containing the instruction
 * @param state the assembler state
 */
void assemble_unconditional_branch(char *target_str, int target_str_len, char *line, assembler_state_t *state) {
    uint64_t instruction = 0;
    SET_BITS(instruction, ENCODING_OFFSET, ENCODING_SIZE, UNCON_BRANCH_VALUE);
    int64_t offset = get_target_address_offset(target_str, target_str_len, state);
    SET_BITS(instruction, BRANCH_SIMM26_OFFSET , BRANCH_SIMM26_SIZE , offset);
    memcpy(state->memory + state->address, &instruction, WORD_SIZE_BYTES);
}

/**
 * Assembles a register branch instruction
 * 
 * @param target_str the string containing the target register
 * @param line the line containing the instruction
 * @param state the assembler state
 */
void assemble_reg_branch(char *target_str, assembler_state_t *state) {
    uint64_t instruction = 0;
    SET_BITS(instruction, ENCODING_OFFSET, ENCODING_SIZE, REGISTER_BRANCH_VALUE);
    uint8_t sf = SF_64;
    uint8_t xn = get_register(target_str, &sf);
    if (sf != SF_64) {
        fprintf(stderr, "Jump target must be 64-bit register!\n");
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }
    SET_BITS(instruction, BRANCH_REG_OFFSET, BRANCH_REG_SIZE, xn);
    memcpy(state->memory + state->address, &instruction, WORD_SIZE_BYTES);
}

/**
 * Assembles a conditional branch instruction
 * 
 * @param opcode the opcode of the instruction
 * @param target_str the string containing the target address
 * @param target_str_len the length of the target string
 * @param line the line containing the instruction
 * @param state the assembler state
 */
void assemble_cond_branch(char *opcode, char *target_str, int target_str_len, assembler_state_t *state) {
    uint64_t instruction = 0;
    SET_BITS(instruction, ENCODING_OFFSET, ENCODING_SIZE, CON_BRANCH_VALUE);

    int64_t offset = get_target_address_offset(target_str, target_str_len, state);
    SET_BITS(instruction, BRANCH_SIMM19_OFFSET, BRANCH_SIMM19_SIZE, offset);

    char *condition_str = opcode + 2;
    if (strcmp(condition_str, "eq") == 0) {
        SET_BITS(instruction, BRANCH_COND_OFFSET, BRANCH_COND_SIZE, COND_EQ);
    }
    else if (strcmp(condition_str, "ne") == 0) {
        SET_BITS(instruction, BRANCH_COND_OFFSET, BRANCH_COND_SIZE, COND_NE);
    }
    else if (strcmp(condition_str, "ge") == 0) {
        SET_BITS(instruction, BRANCH_COND_OFFSET, BRANCH_COND_SIZE, COND_GE);
    }
    else if (strcmp(condition_str, "lt") == 0) {
        SET_BITS(instruction, BRANCH_COND_OFFSET, BRANCH_COND_SIZE, COND_LT);
    }
    else if (strcmp(condition_str, "gt") == 0) {
        SET_BITS(instruction, BRANCH_COND_OFFSET, BRANCH_COND_SIZE, COND_GT);
    }
    else if (strcmp(condition_str, "le") == 0) {
        SET_BITS(instruction, BRANCH_COND_OFFSET, BRANCH_COND_SIZE, COND_LE);
    }
    else if (strcmp(condition_str, "al") == 0) {
        SET_BITS(instruction, BRANCH_COND_OFFSET, BRANCH_COND_SIZE, COND_AL);
    }
    else {
        fprintf(stderr, "Invalid branch condition: %s\n", condition_str);
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }
    memcpy(state->memory + state->address, &instruction, WORD_SIZE_BYTES);
}

/**
 * Assembles a branch instruction
 * 
 * @param opcode the opcode of the instruction
 * @param line the line containing the instruction
 * @param state the assembler state
 */
void assemble_branch_instruction(char *opcode, char *line, assembler_state_t *state) {
    char *target_str = strtok(NULL, " ");
    int target_str_len;
    target_str = strip_line(target_str, &target_str_len);
    if (target_str == NULL) {
        fprintf(stderr, "Invalid line: %s\n", line);
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(opcode, "b") == 0) {
        assemble_unconditional_branch(target_str, target_str_len, line, state);
    }
    else if (strcmp(opcode, "br") == 0) {
        assemble_reg_branch(target_str, state);
    }
    else if (strncmp(opcode, "b.", 2) == 0) {
        assemble_cond_branch(opcode, target_str, target_str_len, state);
    }

    state->address += WORD_SIZE_BYTES;
}
