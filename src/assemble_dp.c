
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "symbol_table.h"
#include "assemble.h"
#include "assemble_dp.h"
#include "utils.h"

/*
 * Sets rn and rm and multiply encoding bits of instruction
 * Also sets x bits of instruction based on opcode
 * Separated for reuse in alias instructions
*/
void handle_data_processing_multiply_instruction(
    char *opcode,
    char *rn_str,
    char *rm_str,
    uint32_t *instruction
) {
    uint8_t sf_rn;
    uint8_t rn = get_register(rn_str, &sf_rn);
    uint8_t sf_rm;
    uint8_t rm = get_register(rm_str, &sf_rm);

    if (sf_rn != sf_rm) {
        fprintf(stderr, "All the registers must have the same bit-width");
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }

    SET_BITS(*instruction, ENCODING_OFFSET, ENCODING_SIZE, MULTIPLY_ENCODING);
    SET_BITS(*instruction, REG_RN_OFFSET, REG_RN_SIZE, rn);
    SET_BITS(*instruction, REG_RM_OFFSET, REG_RM_SIZE, rm);

    if (strcmp(opcode, "madd") == 0) {
        SET_BITS(*instruction, REG_X_OFFSET, REG_X_SIZE, MADD_X);
    }
    else if (strcmp(opcode, "msub") == 0) {
        SET_BITS(*instruction, REG_X_OFFSET, REG_X_SIZE, MSUB_X);
    }
}

/*
 * Assembles multiply instruction
 * Sets sf, rd, and ra bits of instruction, before passing to helper function
 * to set rn and rm bits, and x bits based on opcode
*/
void assemble_data_processing_multiply_instruction(
    char *opcode,
    char *line,
    assembler_state_t *state
) {
    uint32_t instruction = 0;
    char *rd_str = strtok(NULL, ",");
    char *rn_str = strtok(NULL, ",");
    char *rm_str = strtok(NULL, ",");
    char *ra_str = strtok(NULL, ",");
    if (ra_str == NULL) {
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
    uint8_t sf_ra;
    uint8_t ra = get_register(ra_str, &sf_ra);
    SET_BITS(instruction, REG_SF_OFFSET, REG_SF_SIZE, sf_rd);
    SET_BITS(instruction, REG_RD_OFFSET, REG_RD_SIZE, rd);
    SET_BITS(instruction, REG_RA_OFFSET, REG_RA_SIZE, ra);
    handle_data_processing_multiply_instruction(opcode, rn_str, rm_str, &instruction);

    memcpy(state->memory + state->address, &instruction, WORD_SIZE_BYTES);
}

/*
 * Sets the OPC bits of the instruction based on the opcode.
 * For an arithmetic opcode
*/
void handle_data_processing_arithmetic(char *opcode, uint32_t *instruction) {
    if (strcmp(opcode, "add") == 0) {
        SET_BITS(*instruction, REG_OPC_OFFSET, REG_OPC_SIZE, ADD_OPC);
    }
    else if (strcmp(opcode, "adds") == 0) {
        SET_BITS(*instruction, REG_OPC_OFFSET, REG_OPC_SIZE, ADDS_OPC);
    }
    else if (strcmp(opcode, "sub") == 0) {
        SET_BITS(*instruction, REG_OPC_OFFSET, REG_OPC_SIZE, SUB_OPC);
    }
    else if (strcmp(opcode, "subs") == 0) {
        SET_BITS(*instruction, REG_OPC_OFFSET, REG_OPC_SIZE, SUBS_OPC);
    }
}

/*
 * Sets the OPC bits of the instruction based on the opcode.
 * For a bit-wise logical opcode
*/
void handle_data_processing_bit_logic(char *opcode, uint32_t *instruction) {
    if (strcmp(opcode, "and") == 0) {
        SET_BITS(*instruction, REG_OPC_OFFSET, REG_OPC_SIZE, AND_OPC);
    }
    else if (strcmp(opcode, "bic") == 0) {
        SET_BITS(*instruction, REG_OPC_OFFSET, REG_OPC_SIZE, AND_OPC);
        SET_BITS(*instruction, REG_N_OFFSET, REG_N_SIZE, 1);
    }
    else if (strcmp(opcode, "orr") == 0) {
        SET_BITS(*instruction, REG_OPC_OFFSET, REG_OPC_SIZE, ORR_OPC);
    }
    else if (strcmp(opcode, "orn") == 0) {
        SET_BITS(*instruction, REG_OPC_OFFSET, REG_OPC_SIZE, ORR_OPC);
        SET_BITS(*instruction, REG_N_OFFSET, REG_N_SIZE, 1);
    }
    else if (strcmp(opcode, "eor") == 0) {
        SET_BITS(*instruction, REG_OPC_OFFSET, REG_OPC_SIZE, EON_OPC);
    }
    else if (strcmp(opcode, "eon") == 0) {
        SET_BITS(*instruction, REG_OPC_OFFSET, REG_OPC_SIZE, EON_OPC);
        SET_BITS(*instruction, REG_N_OFFSET, REG_N_SIZE, 1);
    }
    else if (strcmp(opcode, "ands") == 0) {
        SET_BITS(*instruction, REG_OPC_OFFSET, REG_OPC_SIZE, ANDS_OPC);
    }
    else if (strcmp(opcode, "bics") == 0) {
        SET_BITS(*instruction, REG_OPC_OFFSET, REG_OPC_SIZE, ANDS_OPC);
        SET_BITS(*instruction, REG_N_OFFSET, REG_N_SIZE, 1);
    }
}

void handle_data_processing_reg_shift(
    char *shift_str,
    uint32_t *instruction
) {
    if (shift_str == NULL) {
        return;
    }
    char *shift_type_str = strtok(shift_str, " ");
    char *shift_operand_str = strtok(NULL, " ");
    if (shift_type_str == NULL || shift_operand_str == NULL) {
        fprintf(stderr, "Invalid shift");
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(shift_type_str, "lsl") == 0) {
        SET_BITS(*instruction, REG_SHIFT_OFFSET, REG_SHIFT_SIZE, LSL_VALUE);
    }
    else if (strcmp(shift_type_str, "lsr") == 0) {
        SET_BITS(*instruction, REG_SHIFT_OFFSET, REG_SHIFT_SIZE, LSR_VALUE);
    }
    else if (strcmp(shift_type_str, "asr") == 0) {
        SET_BITS(*instruction, REG_SHIFT_OFFSET, REG_SHIFT_SIZE, ASR_VALUE);
    }
    else if (strcmp(shift_type_str, "ror") == 0) {
        SET_BITS(*instruction, REG_SHIFT_OFFSET, REG_SHIFT_SIZE, ROR_VALUE);
    }
    else {
        fprintf(stderr, "Invalid shift type: %s\n", shift_type_str);
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }

    if (!is_immediate(shift_operand_str)) {
        fprintf(stderr, "Invalid shift operand: %s\n", shift_operand_str);
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }
    uint8_t shift_operand = strtoll(shift_operand_str + 1, NULL, 0);
    SET_BITS(*instruction, REG_OPERAND_OFFSET, REG_OPERAND_SIZE, shift_operand);
}

void handle_data_processing_two_op_reg(
    char *opcode,
    char *rm_str,
    char *shift_str,
    uint32_t *instruction
) {
    uint8_t sf_rm;
    uint8_t rm = get_register(rm_str, &sf_rm);
    SET_BITS(*instruction, ENCODING_OFFSET, ENCODING_SIZE, DPREG_ENCODING);
    SET_BITS(*instruction, REG_RM_OFFSET, REG_RM_SIZE, rm);
    if (is_data_processing_arithmetic_opcode(opcode)) {
        SET_BITS(*instruction, REG_OPR_OFFSET, REG_OPR_SIZE, ARITHMETIC_OPR);
        handle_data_processing_arithmetic(opcode, instruction);
    }
    else if (is_data_processing_bit_logic_opcode(opcode)) {
        SET_BITS(*instruction, REG_OPR_OFFSET, REG_OPR_SIZE, BIT_LOGIC_OPR);
        handle_data_processing_bit_logic(opcode, instruction);
    }
    handle_data_processing_reg_shift(shift_str, instruction);
}

void handle_data_processing_imm_shift(
    char *shift_str,
    uint32_t *instruction
) {
    if (shift_str == NULL) {
        return;
    }
    char *shift_type_str = strtok(shift_str, " ");
    char *shift_operand_str = strtok(NULL, " ");
    if (shift_type_str == NULL || shift_operand_str == NULL) {
        fprintf(stderr, "Invalid shift");
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(shift_type_str, "lsl") != 0 || !is_immediate(shift_operand_str)) {
        fprintf(stderr, "Invalid shift type");
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }
    uint8_t shift_operand = strtoll(shift_operand_str + 1, NULL, 0);
    if (shift_operand != 0 && shift_operand != IMM_SHIFT_AMOUNT) {
        fprintf(stderr, "Invalid shift operand: only 0 and 12 supported");
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }
    shift_operand /= IMM_SHIFT_AMOUNT;
    SET_BITS(*instruction, IMM_SH_OFFSET, IMM_SH_SIZE, shift_operand);
}

/*
 * Handle data processing instructions with two operands, one of which is an
 * immediate.
 * 
 * Sets the encoding for Data Processing (Immediate) instruction,
 * as well as IMM12, OPI, and the opcode
*/
void handle_data_processing_two_op_imm(
    char *opcode,
    char *operand_str,
    char *shift_str,
    uint32_t *instruction
) {
    assert(is_data_processing_arithmetic_opcode(opcode));
    if (operand_str[0] != '#') {
        fprintf(stderr, "Expected immediate operand to start with #");
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }
    uint32_t imm12 = strtoll(operand_str + 1, NULL, 0);
    SET_BITS(*instruction, ENCODING_OFFSET, ENCODING_SIZE, DPIMM_ENCODING);
    SET_BITS(*instruction, IMM_IMM12_OFFSET, IMM_IMM12_SIZE, imm12);
    SET_BITS(*instruction, IMM_OPI_OFFSET, IMM_OPI_SIZE, IMM_ARITHMETIC_OPI);
    handle_data_processing_arithmetic(opcode, instruction);
    handle_data_processing_imm_shift(shift_str, instruction);
}

void assemble_data_processing_two_op_opcode_instruction(
    char *opcode,
    char *line,
    assembler_state_t *state
) {
    char *rd_str = strtok(NULL, ",");
    char *rn_str = strtok(NULL, ",");
    char *operand_str = strtok(NULL, ",");
    char *shift_str = strtok(NULL, ",");
    if (rd_str == NULL || rn_str == NULL || operand_str == NULL) {
        fprintf(stderr, "Invalid line: %s\n", line);
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }
    rd_str = strip_line(rd_str, NULL);
    rn_str = strip_line(rn_str, NULL);
    operand_str = strip_line(operand_str, NULL);
    if (shift_str != NULL) {
        shift_str = strip_line(shift_str, NULL);
    }
    uint8_t sf_rd;
    uint8_t rd = get_register(rd_str, &sf_rd);
    uint8_t sf_rn;
    uint8_t rn = get_register(rn_str, &sf_rn);

    if (sf_rd != sf_rn) {
        fprintf(stderr, "Invalid line: %s\n", line);
        fprintf(stderr, "All the registers must have the same bit-width");
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }

    uint32_t instruction = 0;
    SET_BITS(instruction, REG_SF_OFFSET, REG_SF_SIZE, sf_rd);
    SET_BITS(instruction, REG_RD_OFFSET, REG_RD_SIZE, rd);
    SET_BITS(instruction, REG_RN_OFFSET, REG_RN_SIZE, rn);

    if (is_immediate(operand_str)) {
        handle_data_processing_two_op_imm(opcode, operand_str, shift_str, &instruction);
    }
    else {
        handle_data_processing_two_op_reg(opcode, operand_str, shift_str, &instruction);
    }
    memcpy(state->memory + state->address, &instruction, WORD_SIZE_BYTES);
}

void handle_data_processing_wide_move(
    char *opcode,
    char *imm_str,
    char *shift_str,
    uint32_t *instruction
) {
    assert(is_data_processing_wide_move_opcode(opcode));
    uint32_t imm16 = strtoll(imm_str + 1, NULL, 0);
    SET_BITS(*instruction, ENCODING_OFFSET, ENCODING_SIZE, DPIMM_ENCODING);
    SET_BITS(*instruction, IMM_OPI_OFFSET, IMM_OPI_SIZE, IMM_WIDE_MOVE_OPI);
    SET_BITS(*instruction, IMM_IMM16_OFFSET, IMM_IMM16_SIZE, imm16);

    if (strcmp(opcode, "movn") == 0) {
        SET_BITS(*instruction, IMM_OPC_OFFSET, IMM_OPC_SIZE, MOVN_OPC);
    }
    else if (strcmp(opcode, "movz") == 0) {
        SET_BITS(*instruction, IMM_OPC_OFFSET, IMM_OPC_SIZE, MOVZ_OPC);
    }
    else if (strcmp(opcode, "movk") == 0) {
        SET_BITS(*instruction, IMM_OPC_OFFSET, IMM_OPC_SIZE, MOVK_OPC);
    }

    if (shift_str != NULL) {
        char *shift_type_str = strtok(shift_str, " ");
        char *shift_operand_str = strtok(NULL, " ");
        if (shift_type_str == NULL || shift_operand_str == NULL) {
            fprintf(stderr, "Invalid shift");
            fprintf(stderr, "Exiting!\n");
            exit(EXIT_FAILURE);
        }
        if (strcmp(shift_type_str, "lsl") != 0 || !is_immediate(shift_operand_str)) {
            fprintf(stderr, "Invalid shift type");
            fprintf(stderr, "Exiting!\n");
            exit(EXIT_FAILURE);
        }
        uint8_t shift_operand = strtoll(shift_operand_str + 1, NULL, 0);
        if (shift_operand % WIDE_MOVE_SHIFT_AMOUNT != 0) {
            fprintf(stderr, "Shift operand must be a multiple of 16 for wide move\n");
            fprintf(stderr, "Exiting!\n");
            exit(EXIT_FAILURE);
        }
        uint8_t hw = shift_operand / WIDE_MOVE_SHIFT_AMOUNT;
        SET_BITS(*instruction, IMM_HW_OFFSET, IMM_HW_SIZE, hw);
    }
}

void handle_data_processing_single_op_imm(
    char *opcode,
    char *operand_str,
    char *shift_str,
    uint32_t *instruction
) {
    if (strcmp(opcode, "mov") == 0) {
        handle_data_processing_two_op_imm("orr", operand_str, shift_str, instruction);
        handle_data_processing_imm_shift(shift_str, instruction);
    }
    else if (strcmp(opcode, "mvn") == 0) {
        handle_data_processing_two_op_imm("orn", operand_str, shift_str, instruction);
        handle_data_processing_imm_shift(shift_str, instruction);
    }
    else if (strcmp(opcode, "neg") == 0) {
        handle_data_processing_two_op_imm("sub", operand_str, shift_str, instruction);
        handle_data_processing_imm_shift(shift_str, instruction);
    }
    else if (strcmp(opcode, "negs") == 0) {
        handle_data_processing_two_op_imm("subs", operand_str, shift_str, instruction);
        handle_data_processing_imm_shift(shift_str, instruction);
    }
    else if (is_data_processing_wide_move_opcode(opcode)) {
        handle_data_processing_wide_move(opcode, operand_str, shift_str, instruction);
    }
}

void handle_data_processing_single_op_reg(
    char *opcode,
    char *rm_str,
    char *shift_str,
    uint32_t *instruction
) {
    assert(is_data_processing_single_op_alias_opcode(opcode));
    if (strcmp(opcode, "mov") == 0) {
        handle_data_processing_two_op_reg("orr", rm_str, shift_str, instruction);
        handle_data_processing_reg_shift(shift_str, instruction);
    }
    else if (strcmp(opcode, "mvn") == 0) {
        handle_data_processing_two_op_reg("orn", rm_str, shift_str, instruction);
        handle_data_processing_reg_shift(shift_str, instruction);
    }
    else if (strcmp(opcode, "neg") == 0) {
        handle_data_processing_two_op_reg("sub", rm_str, shift_str, instruction);
        handle_data_processing_reg_shift(shift_str, instruction);
    }
    else if (strcmp(opcode, "negs") == 0) {
        handle_data_processing_two_op_reg("subs", rm_str, shift_str, instruction);
        handle_data_processing_reg_shift(shift_str, instruction);
    }
    else if (strcmp(opcode, "mul") == 0) {
        SET_BITS(*instruction, REG_RA_OFFSET, REG_RA_SIZE, ZR_REG);
        handle_data_processing_multiply_instruction("madd", rm_str, shift_str, instruction);
    }
    else if (strcmp(opcode, "mneg") == 0) {
        SET_BITS(*instruction, REG_RA_OFFSET, REG_RA_SIZE, ZR_REG);
        handle_data_processing_multiply_instruction("msub", rm_str, shift_str, instruction);
    }
}

void assemble_data_processing_single_op_opcode_instruction(
    char *opcode,
    char *line,
    assembler_state_t *state
) {
    char *rd_str = strtok(NULL, ",");
    char *operand_str = strtok(NULL, ",");
    char *shift_str = strtok(NULL, ",");
    if (rd_str == NULL || operand_str == NULL) {
        fprintf(stderr, "Invalid line: %s\n", line);
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }
    rd_str = strip_line(rd_str, NULL);
    operand_str = strip_line(operand_str, NULL);
    if (shift_str != NULL) {
        shift_str = strip_line(shift_str, NULL);
    }
    uint8_t sf_rd;
    uint8_t rd = get_register(rd_str, &sf_rd);

    uint32_t instruction = 0;
    SET_BITS(instruction, REG_SF_OFFSET, REG_SF_SIZE, sf_rd);
    SET_BITS(instruction, REG_RD_OFFSET, REG_RD_SIZE, rd);

    if (is_immediate(operand_str)) {
        handle_data_processing_single_op_imm(opcode, operand_str, shift_str, &instruction);
    }
    else {
        handle_data_processing_single_op_reg(opcode, operand_str, shift_str, &instruction);
    }
    memcpy(state->memory + state->address, &instruction, WORD_SIZE_BYTES);
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