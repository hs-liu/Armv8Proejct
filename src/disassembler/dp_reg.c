#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "disassemble.h"
#include "dp_reg.h"
#include "dp_imm.h"

void and (FILE* fp, uint8_t dest, uint8_t src1, uint64_t op2, uint8_t sf) {
}

void bic(FILE* fp, uint8_t dest, uint8_t src1, uint64_t op2, uint8_t sf) {
}

void orr(FILE* fp, uint8_t dest, uint8_t src1, uint64_t op2, uint8_t sf) {
}


void orn(FILE* fp, uint8_t dest, uint8_t src1, uint64_t op2, uint8_t sf) {
}

void eon(FILE* fp, uint8_t dest, uint8_t src1, uint64_t op2, uint8_t sf) {
}

void eor(FILE* fp, uint8_t dest, uint8_t src1, uint64_t op2, uint8_t sf) {
}

void ands(FILE* fp, uint8_t dest, uint8_t src1, uint64_t op2, uint8_t sf) {
}

void bics(FILE* fp, uint8_t dest, uint8_t src1, uint64_t op2, uint8_t sf) {
}

void madd(FILE* fp, uint8_t dest, uint8_t src, uint8_t rn, uint8_t rm, uint8_t sf) {
}


void msub(FILE* fp, uint8_t dest, uint8_t src, uint8_t rn, uint8_t rm, uint8_t sf) {
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
            add_imm(fp, rd, rn, op2, sf);
            break;
        case SUB_OPC:
            sub_imm(fp, rd, rn, op2, sf);
            break;
        case ADDS_OPC:
            adds_imm(fp, rd, rn, op2, sf);
            break;
        case SUBS_OPC:
            subs_imm(fp, rd, rn, op2, sf);
            break;
        default:
            fprintf(stderr, "Illegal instruction: invalid opc value\n");
            fprintf(stderr, "Exiting!\n");
            exit(EXIT_FAILURE);
    }
}

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
    // uint64_t op2;
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
        case ROR_VALUE:
            // op2 = ror(cpu_state, rm, operand, sf);
            break;
        default:
            fprintf(stderr, "Illegal instruction: invalid shift value\n");
            fprintf(stderr, "Exiting!\n");
            exit(EXIT_FAILURE);
    }
    if (N == 0) {
        switch (opc) {
            case AND_OPC:
                and (fp, rd, rn, rm, sf);
                break;
            case ORR_OPC:
                orr(fp, rd, rn, op2, sf);
                break;
            case EON_OPC:
                eor(fp, rd, rn, op2, sf);
                break;
            case ANDS_OPC:
                ands(fp, rd, rn, op2, sf);
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
                bic(fp, rd, rn, op2, sf);
                break;
            case ORR_OPC:
                orn(fp, rd, rn, op2, sf);
                break;
            case EON_OPC:
                eon(fp, rd, rn, op2, sf);
                break;
            case ANDS_OPC:
                bics(fp, rd, rn, op2, sf);
                break;
            default:
                fprintf(stderr, "Illegal instruction: invalid opc value\n");
                fprintf(stderr, "Exiting!\n");
                exit(EXIT_FAILURE);
        }
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
