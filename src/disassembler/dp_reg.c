#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "disassemble.h"
#include "dp_reg.h"
#include "dp_imm.h"

/**
 * disassemble bit logic instruction
 * 
 * @param fp the file to print to
 * @param op_string the string to print
 * @param rd the destination register
 * @param rn the first source register
 * @param rm the second source register
 * @param shift_str the shift string
 * @param shift_amount the shift amount
 * @param sf the size bit
*/
void handle_bit_logic(FILE* fp, char *op_string, uint8_t rd, uint8_t rn, uint64_t rm, char *shift_str, uint8_t shift_amount, uint8_t sf) {
    assert(sf == SF_32 || sf == SF_64);
    if (sf == SF_32) {
        fprintf(fp, "%s w%d, w%d, w%ld, %s #%d\n", op_string, rd, rn, rm, shift_str, shift_amount);
    } else {
        fprintf(fp, "%s x%d, x%d, x%ld, %s #%d\n", op_string, rd, rn, rm, shift_str, shift_amount);
    }
}

/**
 * disassemble arithmetic instruction
 * 
 * @param fp the file to print to
 * @param op_string the string to print
 * @param rd the destination register
 * @param rn the first source register
 * @param rm the second source register
 * @param shift_str the shift string
 * @param shift_amount the shift amount
 * @param sf the size bit
*/
void handle_reg_arithmetic(FILE* fp, char *op_string, uint8_t rd, uint8_t rn, uint64_t rm, char *shift_str, uint8_t shift_amount, uint8_t sf) {
    assert(sf == SF_32 || sf == SF_64);
    if (sf == SF_32) {
        fprintf(fp, "%s w%d, w%d, w%ld, %s #%d\n", op_string, rd, rn, rm, shift_str, shift_amount);
    } else {
        fprintf(fp, "%s x%d, x%d, x%ld, %s #%d\n", op_string, rd, rn, rm, shift_str, shift_amount);
    }
}

/**
 * disassemble multiply instruction
 * 
 * @param fp the file to print to
 * @param op_string the string to print
 * @param rd the destination register
 * @param rn the first source register
 * @param rm the second source register
 * @param ra the third source register
 * @param sf the size bit
*/
void handle_multiply(FILE* fp, char *op_string, uint8_t rd, uint8_t rn, uint64_t rm, uint8_t ra, uint8_t sf) {
    assert(sf == SF_32 || sf == SF_64);
    if (sf == SF_32) {
        fprintf(fp, "%s w%d, w%d, w%ld, w%d\n", op_string, rd, rn, rm, ra);
    } else {
        fprintf(fp, "%s x%d, x%d, x%ld, x%d\n", op_string, rd, rn, rm, ra);
    }
}

/**
 * delegate to the correct function to disassemble the arithmetic instruction
 * 
 * @param fp the file to print to
 * @param instruction the instruction to disassemble
*/
void disassemble_arithmetic_instruction(FILE* fp, uint32_t instruction) {
    uint8_t sf = SELECT_BITS(instruction, REG_SF_OFFSET, REG_SF_SIZE);
    uint8_t opc = SELECT_BITS(instruction, REG_OPC_OFFSET, REG_OPC_SIZE);
    uint8_t opr = SELECT_BITS(instruction, REG_OPR_OFFSET, REG_OPR_SIZE);
    uint8_t rm = SELECT_BITS(instruction, REG_RM_OFFSET, REG_RM_SIZE);
    uint8_t operand = SELECT_BITS(instruction, REG_OPERAND_OFFSET, REG_OPERAND_SIZE);
    uint8_t rn = SELECT_BITS(instruction, REG_RN_OFFSET, REG_RN_SIZE);
    uint8_t rd = SELECT_BITS(instruction, REG_RD_OFFSET, REG_RD_SIZE);
    uint8_t shift = SELECT_BITS(opr, SHIFT_OFFSET, SHIFT_SIZE);
    char shift_str[4];
    switch (shift) {
        case LSL_VALUE:
            strcpy(shift_str, "lsl");
            break;
        case LSR_VALUE:
            strcpy(shift_str, "lsr");
            break;
        case ASR_VALUE:
            strcpy(shift_str, "asr");
            break;
        default:
            fprintf(stderr, "Illegal instruction: invalid shift value\n");
            fprintf(stderr, "Exiting!\n");
            exit(EXIT_FAILURE);
    }
    char op_str[5];
    switch (opc) {
        case ADD_OPC:
            strcpy(op_str, "add");
            break;
        case SUB_OPC:
            strcpy(op_str, "sub");
            break;
        case ADDS_OPC:
            strcpy(op_str, "adds");
            break;
        case SUBS_OPC:
            strcpy(op_str, "subs");
            break;
        default:
            fprintf(stderr, "Illegal instruction: invalid opc value\n");
            fprintf(stderr, "Exiting!\n");
            exit(EXIT_FAILURE);
    }
    handle_reg_arithmetic(fp, op_str, rd, rn, rm, shift_str, operand, sf);
}

/**
 * delegate to the correct function to disassemble the bit logic instruction
 * 
 * @param fp the file to print to
 * @param instruction the instruction to disassemble
*/
void disassemble_bit_logic_instruction(FILE* fp, uint32_t instruction) {
    uint8_t sf = SELECT_BITS(instruction, REG_SF_OFFSET, REG_SF_SIZE);
    uint8_t opc = SELECT_BITS(instruction, REG_OPC_OFFSET, REG_OPC_SIZE);
    uint8_t opr = SELECT_BITS(instruction, REG_OPR_OFFSET, REG_OPR_SIZE);
    uint8_t rm = SELECT_BITS(instruction, REG_RM_OFFSET, REG_RM_SIZE);
    uint8_t operand = SELECT_BITS(instruction, REG_OPERAND_OFFSET, REG_OPERAND_SIZE);
    uint8_t rn = SELECT_BITS(instruction, REG_RN_OFFSET, REG_RN_SIZE);
    uint8_t rd = SELECT_BITS(instruction, REG_RD_OFFSET, REG_RD_SIZE);
    uint8_t shift = SELECT_BITS(opr, SHIFT_OFFSET, SHIFT_SIZE);
    uint8_t N = SELECT_BITS(opr, N_OFFSET, N_SIZE);
    char shift_str[4];
    switch (shift) {
        case LSL_VALUE:
            strcpy(shift_str, "lsl");
            break;
        case LSR_VALUE:
            strcpy(shift_str, "lsr");
            break;
        case ASR_VALUE:
            strcpy(shift_str, "asr");
            break;
        case ROR_VALUE:
            strcpy(shift_str, "ror");
            break;
    }
    char op_str[5];
    if (N == 0) {

        switch (opc) {
            case AND_OPC:
                strcpy(op_str, "and");
                break;
            case ORR_OPC:
                strcpy(op_str, "orr");
                break;
            case EON_OPC:
                strcpy(op_str, "eor");
                break;
            case ANDS_OPC:
                strcpy(op_str, "ands");
                break;
            default:
                fprintf(stderr, "Illegal instruction: invalid opc value\n");
                fprintf(stderr, "Exiting!\n");
                exit(EXIT_FAILURE);
        }

    }
    if (N == 1) {
        switch (opc) {
            case AND_OPC:
                strcpy(op_str, "bic");
                break;
            case ORR_OPC:
                strcpy(op_str, "orn");
                break;
            case EON_OPC:
                strcpy(op_str, "eon");
                break;
            case ANDS_OPC:
                strcpy(op_str, "bics");
                break;
            default:
                fprintf(stderr, "Illegal instruction: invalid opc value\n");
                fprintf(stderr, "Exiting!\n");
                exit(EXIT_FAILURE);
        }
    }
        handle_bit_logic(fp, op_str, rd, rn, rm, shift_str, operand, sf);
}

/**
 * delegate to the correct function to disassemble the multiply instruction
 * 
 * @param fp the file to print to
 * @param instruction the instruction to disassemble
*/
void disassemble_multiply_instruction(FILE* fp, uint32_t instruction) {
    uint8_t sf = SELECT_BITS(instruction, REG_SF_OFFSET, REG_SF_SIZE);
    uint8_t rm = SELECT_BITS(instruction, REG_RM_OFFSET, REG_RM_SIZE);
    uint8_t rn = SELECT_BITS(instruction, REG_RN_OFFSET, REG_RN_SIZE);
    uint8_t rd = SELECT_BITS(instruction, REG_RD_OFFSET, REG_RD_SIZE);
    uint8_t x = SELECT_BITS(instruction, REG_X_OFFSET, REG_M_SIZE);
    uint8_t ra = SELECT_BITS(instruction, REG_RA_OFFSET, REG_RA_SIZE);
    char op_str[5];
    if (x == MADD_X) {
        strcpy(op_str, "madd");
    }
    if (x == MSUB_X) {
        strcpy(op_str, "msub");
    }
    handle_multiply(fp, op_str, rd, rn, rm, ra, sf);

}

/**
 * delegate to the correct function to disassemble the dpreg instruction
 * 
 * @param fp the file to print to
 * @param instruction the instruction to disassemble
*/
void disassemble_dpreg_instruction(FILE* fp, uint32_t instruction) {
    assert(SELECT_BITS(instruction, DPREG_OFFSET, DPREG_SIZE) == 0x5);
    uint8_t sf = SELECT_BITS(instruction, REG_SF_OFFSET, REG_SF_SIZE);
    uint8_t m = SELECT_BITS(instruction, REG_M_OFFSET, REG_M_SIZE);
    uint8_t opr = SELECT_BITS(instruction, REG_OPR_OFFSET, REG_OPR_SIZE);
    assert(sf == SF_32 || sf == SF_64);

    if (m == ARITHMETIC_M && CHECK_BITS(opr, ARITHMETIC_MASK, ARITHMETIC_VALUE)) {
        disassemble_arithmetic_instruction(fp, instruction);
    }

    if (m == BIT_LOGIC_M && CHECK_BITS(opr, BIT_LOGIC_MASK, BIT_LOGIC_VALUE)) {
        disassemble_bit_logic_instruction(fp, instruction);
    }

    if (m == MULTIPLY_M && CHECK_BITS(opr, MULTIPLY_MASK, MULTIPLY_VALUE)) {
        disassemble_multiply_instruction(fp, instruction);
    }
}
