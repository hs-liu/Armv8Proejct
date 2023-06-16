
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "symbol_table.h"
#include "assemble.h"
#include "assemble_dp.h"
#include "utils.h"

void assemble_data_processing_multiply_instruction(
    char *opcode,
    char *line,
    assembler_state_t *state
) {
    char *rd_str = strtok(NULL, ",");
    char *rn_str = strtok(NULL, ",");
    char *rm_str = strtok(NULL, ",");
    char *ra_str = strtok(NULL, ",");
    if (rd_str == NULL || rn_str == NULL || rm_str == NULL || ra_str == NULL) {
        fprintf(stderr, "Invalid line: %s\n", line);
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }
    rd_str = strip_line(rd_str, NULL);
    rn_str = strip_line(rn_str, NULL);
    rm_str = strip_line(rm_str, NULL);
    ra_str = strip_line(ra_str, NULL);
    uint8_t sf_rd;
    uint8_t rd = get_register(rd_str, &sf_rd);
    uint8_t sf_rn;
    uint8_t rn = get_register(rn_str, &sf_rn);
    uint8_t sf_rm;
    uint8_t rm = get_register(rm_str, &sf_rm);
    uint8_t sf_ra;
    uint8_t ra = get_register(ra_str, &sf_ra);

    if (!(sf_rd == sf_rn && sf_rn == sf_rm && sf_rm == sf_ra)) {
        fprintf(stderr, "Invalid line: %s\n", line);
        fprintf(stderr, "All the registers must have the same bit-width");
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }

    uint32_t instruction = 0;
    SET_BITS(instruction, ENCODING_OFFSET, ENCODING_SIZE, MULTIPLY_ENCODING);
    SET_BITS(instruction, REG_SF_OFFSET, REG_SF_SIZE, sf_rd);
    SET_BITS(instruction, REG_RD_OFFSET, REG_RD_SIZE, rd);
    SET_BITS(instruction, REG_RN_OFFSET, REG_RN_SIZE, rn);
    SET_BITS(instruction, REG_RM_OFFSET, REG_RM_SIZE, rm);
    SET_BITS(instruction, REG_RA_OFFSET, REG_RA_SIZE, ra);

    if (strcmp(opcode, "madd") == 0) {
        SET_BITS(instruction, REG_X_OFFSET, REG_X_SIZE, MADD_X);
    }
    else if (strcmp(opcode, "msub") == 0) {
        SET_BITS(instruction, REG_X_OFFSET, REG_X_SIZE, MSUB_X);
    }

    memcpy(state->memory + state->address, &instruction, WORD_SIZE_BYTES);
}

void assemble_data_processing_two_op_opcode_instruction(
    char *opcode,
    char *line,
    assembler_state_t *state
) {
}
void assemble_data_processing_single_op_opcode_instruction(
    char *opcode,
    char *line,
    assembler_state_t *state
) {
}
void assemble_data_processing_two_op_no_dest_opcode_instruction(
    char *opcode,
    char *line,
    assembler_state_t *state
) {
}

void assemble_data_processing_instruction(char *opcode, char *line, assembler_state_t *state) {
    if (is_data_processing_multiply_opcode(opcode)) {
        assemble_data_processing_multiply_instruction(opcode, line, state);
    }
    else if (is_data_processing_two_op_opcode(opcode)) {
        assemble_data_processing_two_op_opcode_instruction(opcode, line, state);
    }
    else if (is_data_processing_single_op_opcode(opcode)) {
        assemble_data_processing_single_op_opcode_instruction(opcode, line, state);
    }
    else if (is_data_processing_two_op_no_dest_opcode(opcode)) {
        assemble_data_processing_two_op_no_dest_opcode_instruction(opcode, line, state);
    }
    else {
        fprintf(stderr, "Invalid data processing opcode: %s\n", opcode);
        exit(EXIT_FAILURE);
    }
    // uint32_t instruction = HALT_INSTRUCTION;
    // memcpy(state->memory + state->address, &instruction, WORD_SIZE_BYTES);
    state->address += WORD_SIZE_BYTES;
}