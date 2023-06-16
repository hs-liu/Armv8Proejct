#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "disassemble.h"
#include "dp_reg.h"
#include "dp_imm.h"

void handle_bit_logic(FILE* fp, char *op_string, uint8_t rd, uint8_t rn, uint64_t rm, char *shift_str, uint8_t shift_amount, uint8_t sf) {
    assert(sf == SF_32 || sf == SF_64);
    if (sf == SF_32) {
        fprintf(fp, "%s w%d, w%d, w%d, %s #%d\n", op_string, rd, rn, rm, shift_str, shift_amount);
    } else {
        fprintf(fp, "%s x%d, x%d, x%d, %s #%d\n", op_string, rd, rn, rm, shift_str, shift_amount);
    }
}

void madd(FILE* fp, uint8_t rd, uint8_t rn, uint64_t rm, uint8_t ra, uint8_t sf) {
    assert(sf == SF_32 || sf == SF_64);
    if (sf == SF_32) {
        fprintf(fp, "madd w%d, w%d, w%d, w%d\n", rd, rn, rm, ra);
    } else {
        fprintf(fp, "madd x%d, x%d, x%d, x%d\n", rd, rn, rm, ra);
    }
}


void msub(FILE* fp, uint8_t rd, uint8_t rn, uint64_t rm, uint8_t ra, uint8_t sf) {
    assert(sf == SF_32 || sf == SF_64);
    if (sf == SF_32) {
        fprintf(fp, "msub w%d, w%d, w%d, w%d\n", rd, rn, rm, ra);
    } else {
        fprintf(fp, "msub x%d, x%d, x%d, x%d\n", rd, rn, rm, ra);
    }
}



void disassemble_arithmetic_instruction(FILE* fp, uint32_t instruction) {
    uint8_t sf = SELECT_BITS(instruction, REG_SF_OFFSET, REG_SF_SIZE);
    uint8_t opc = SELECT_BITS(instruction, REG_OPC_OFFSET, REG_OPC_SIZE);
    uint8_t opr = SELECT_BITS(instruction, REG_OPR_OFFSET, REG_OPR_SIZE);
    uint8_t rm = SELECT_BITS(instruction, REG_RM_OFFSET, REG_RM_SIZE);
    uint8_t operand = SELECT_BITS(instruction, REG_OPERAND_OFFSET, REG_OPERAND_SIZE);
    uint8_t rn = SELECT_BITS(instruction, REG_RN_OFFSET, REG_RN_SIZE);
    uint8_t rd = SELECT_BITS(instruction, REG_RD_OFFSET, REG_RD_SIZE);
    uint8_t shift = SELECT_BITS(opr, SHIFT_OFFSET, SHIFT_SIZE);
    uint64_t op2;
    switch (shift) {
        case LSL_VALUE:
            // op2 = lsl(cpu_state, rm, operand, sf);
            break;
        case LSR_VALUE:
            // op2 = lsr(cpu_state, rm, operand, sf);
            break;
        case ASR_VALUE:
            // op2 = asr(cpu_state, rm, operand, sf);
            break;
        default:
            fprintf(stderr, "Illegal instruction: invalid shift value\n");
            fprintf(stderr, "Exiting!\n");
            exit(EXIT_FAILURE);
    }
    switch (opc) {
        case ADD_OPC:
            // add_imm(fp, rd, rn, op2, sf);
            break;
        case SUB_OPC:
            // sub_imm(fp, rd, rn, op2, sf);
            break;
        case ADDS_OPC:
            // adds_imm(fp, rd, rn, op2, sf);
            break;
        case SUBS_OPC:
            // subs_imm(fp, rd, rn, op2, sf);
            break;
        default:
            fprintf(stderr, "Illegal instruction: invalid opc value\n");
            fprintf(stderr, "Exiting!\n");
            exit(EXIT_FAILURE);
    }
}

void disassemble_bit_logic_instruction(FILE* fp, uint32_t instruction) {
    // printf("disassemble_bit_logic_instruction\n");
    uint8_t sf = SELECT_BITS(instruction, REG_SF_OFFSET, REG_SF_SIZE);
    uint8_t opc = SELECT_BITS(instruction, REG_OPC_OFFSET, REG_OPC_SIZE);
    uint8_t opr = SELECT_BITS(instruction, REG_OPR_OFFSET, REG_OPR_SIZE);
    uint8_t rm = SELECT_BITS(instruction, REG_RM_OFFSET, REG_RM_SIZE);
    uint8_t operand = SELECT_BITS(instruction, REG_OPERAND_OFFSET, REG_OPERAND_SIZE);
    uint8_t rn = SELECT_BITS(instruction, REG_RN_OFFSET, REG_RN_SIZE);
    uint8_t rd = SELECT_BITS(instruction, REG_RD_OFFSET, REG_RD_SIZE);
    uint8_t shift = SELECT_BITS(opr, SHIFT_OFFSET, SHIFT_SIZE);
    uint8_t N = SELECT_BITS(opr, N_OFFSET, N_SIZE);
    // uint64_t op2;
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
            strcpy(op_str, "eon");
                break;
            case ANDS_OPC:
            strcpy(op_str, "ands");
                break;
            default:
                fprintf(stderr, "Illegal instruction: invalid opc value\n");
                fprintf(stderr, "Exiting!\n");
                exit(EXIT_FAILURE);
        }
        handle_bit_logic(fp, op_str, rd, rn, rm, shift_str, operand, sf);

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
        handle_bit_logic(fp, op_str, rd, rn, rm, shift_str, operand, sf);
    }
}

void disassemble_multiply_instruction(FILE* fp, uint32_t instruction) {
    uint8_t sf = SELECT_BITS(instruction, REG_SF_OFFSET, REG_SF_SIZE);
    uint8_t rm = SELECT_BITS(instruction, REG_RM_OFFSET, REG_RM_SIZE);
    uint8_t rn = SELECT_BITS(instruction, REG_RN_OFFSET, REG_RN_SIZE);
    uint8_t rd = SELECT_BITS(instruction, REG_RD_OFFSET, REG_RD_SIZE);
    uint8_t x = SELECT_BITS(instruction, REG_X_OFFSET, REG_M_SIZE);
    uint8_t ra = SELECT_BITS(instruction, REG_RA_OFFSET, REG_RA_SIZE);
    if (x == MADD_X) {
        madd(fp, rd, ra, rn, rm, sf);
    }
    if (x == MSUB_X) {
        msub(fp, rd, ra, rn, rm, sf);
    }

}

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
