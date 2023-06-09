#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "emulate.h"
#include "dp_reg.h"
#include "dp_imm.h"

void and(state_t *state, uint8_t dest, uint8_t src1, uint64_t op2, uint8_t sf) {
    uint64_t result = get_register_value(state, src1, sf) & op2;
    set_register_value(state, dest, result, sf);
}

void bic(state_t *state, uint8_t dest, uint8_t src1, uint64_t op2, uint8_t sf) {
    uint64_t result = get_register_value(state, src1, sf) & ~op2;
    set_register_value(state, dest, result, sf);
}

void orr(state_t *state, uint8_t dest, uint8_t src1, uint64_t op2, uint8_t sf) {
    uint64_t result = get_register_value(state, src1, sf) | op2;
    set_register_value(state, dest, result, sf);
}


void orn(state_t *state, uint8_t dest, uint8_t src1, uint64_t op2, uint8_t sf) {
    uint64_t result = get_register_value(state, src1, sf) | ~op2;
    set_register_value(state, dest, result, sf);
}

void eon(state_t *state, uint8_t dest, uint8_t src1, uint64_t op2, uint8_t sf) {
    uint64_t result = get_register_value(state, src1, sf) ^ ~op2;
    set_register_value(state, dest, result, sf);
}


void eor(state_t *state, uint8_t dest, uint8_t src1, uint64_t op2, uint8_t sf) {
    uint64_t result = get_register_value(state, src1, sf) ^ op2;
    set_register_value(state, dest, result, sf);
}


void ands(state_t *state, uint8_t dest, uint8_t src1, uint64_t op2, uint8_t sf) {
    uint64_t result = get_register_value(state, src1, sf) & op2;
    set_register_value(state, dest, result, sf);
    set_NV_flags(state, result, sf);
    state->PSTATE.C = 0;
    state->PSTATE.V = 0;
}

void bics(state_t *state, uint8_t dest, uint8_t src1, uint64_t op2, uint8_t sf) {
    uint64_t result = get_register_value(state, src1, sf) & ~op2;
    set_register_value(state, dest, result, sf);
    set_NV_flags(state, result, sf);
    state->PSTATE.C = 0;
    state->PSTATE.V = 0;
}

void madd(state_t *state, uint8_t dest, uint8_t src, uint8_t rn, uint8_t rm, uint8_t sf) {
    uint64_t result = get_register_value(state, src, sf);
    result += get_register_value(state, rn, sf) * get_register_value(state, rm, sf);
    set_register_value(state, dest, result, sf);
}


void msub(state_t *state, uint8_t dest, uint8_t src, uint8_t rn, uint8_t rm, uint8_t sf) {
    uint64_t result = get_register_value(state, src, sf);
    result -= get_register_value(state, rn, sf) * get_register_value(state, rm, sf);
    set_register_value(state, dest, result, sf);
}

uint64_t lsl(state_t *state, uint8_t operand_reg, uint8_t shift_amount, uint8_t sf) {
    return get_register_value(state, operand_reg, sf) << shift_amount;
}

uint64_t lsr(state_t *state, uint8_t operand_reg, uint8_t shift_amount, uint8_t sf) {
    return get_register_value(state, operand_reg, sf) >> shift_amount;
}

uint64_t asr(state_t *state, uint8_t operand_reg, uint8_t shift_amount, uint8_t sf) {
    int64_t result = get_register_value(state, operand_reg, sf);
    if (sf == SF_32) {
        result = SIGN_EXT(result, 32, 64);
    }
    result >>= shift_amount;
    return result;
}

uint64_t ror(state_t *state, uint8_t operand_reg, uint8_t shift_amount, uint8_t sf) {
    int num_bits = (sf == SF_32) ? 32 : 64;
    uint64_t operand = get_register_value(state, operand_reg, sf);
    uint64_t lower_bits = SELECT_BITS(operand, 0, shift_amount);
    uint64_t upper_bits = SELECT_BITS(operand, shift_amount, num_bits - shift_amount);
    lower_bits <<= (num_bits - shift_amount);
    upper_bits |= lower_bits;
    return upper_bits;
}

void execute_dpreg_instruction(state_t *state, uint32_t instruction) {
    assert(SELECT_BITS(instruction, DPREG_OFFSET, DPREG_SIZE) == 0x5);
    uint8_t sf = SELECT_BITS(instruction, REG_SF_OFFSET, REG_SF_SIZE);
    uint8_t opc = SELECT_BITS(instruction, REG_OPC_OFFSET, REG_OPC_SIZE);
    uint8_t m = SELECT_BITS(instruction, REG_M_OFFSET, REG_M_SIZE);
    uint8_t opr = SELECT_BITS(instruction, REG_OPR_OFFSET, REG_OPR_SIZE);
    uint8_t rm = SELECT_BITS(instruction, REG_RM_OFFSET, REG_RM_SIZE);
    uint8_t operand = SELECT_BITS(instruction, REG_OPERAND_OFFSET, REG_OPERAND_SIZE);
    uint8_t rn = SELECT_BITS(instruction, REG_RN_OFFSET, REG_RN_SIZE);
    uint8_t rd = SELECT_BITS(instruction, REG_RD_OFFSET, REG_RD_SIZE);
    assert(sf == SF_32 || sf == SF_64);

    // Check if arithmetic
    if (m == ARITHMETIC_M && CHECK_BITS(opr, ARITHMETIC_MASK, ARITHMETIC_VALUE)) {
        uint8_t shift = SELECT_BITS(opr, SHIFT_OFFSET, SHIFT_SIZE);
        uint64_t op2;
        switch (shift) {
            case LSL_VALUE:
                op2 = lsl(state, rm, operand, sf);
                break;
            case LSR_VALUE:
                op2 = lsr(state, rm, operand, sf);
                break;
            case ASR_VALUE:
                op2 = asr(state, rm, operand, sf);
                break;
            default:
                fprintf(stderr, "Illegal instruction: invalid shift value\n");
                fprintf(stderr, "Exiting!\n");
                exit(EXIT_FAILURE);
        }
        switch (opc) {
            case ADD_OPC:
                add_imm(state, rd, rn, op2, sf);
                break;
            case SUB_OPC:
                sub_imm(state, rd, rn, op2, sf);
                break;
            case ADDS_OPC:
                adds_imm(state, rd, rn, op2, sf);
                break;
            case SUBS_OPC:
                subs_imm(state, rd, rn, op2, sf);
                break;
            default:
                fprintf(stderr, "Illegal instruction: invalid opc value\n");
                fprintf(stderr, "Exiting!\n");
                exit(EXIT_FAILURE);
        }
    }

    // Check if bit-logic
    if (m == BIT_LOGIC_M && CHECK_BITS(opr, BIT_LOGIC_MASK, BIT_LOGIC_VALUE)) {
        uint8_t shift = SELECT_BITS(opr, SHIFT_OFFSET, SHIFT_SIZE);
        uint8_t N = SELECT_BITS(opr, N_OFFSET, N_SIZE);
        uint64_t op2;
        switch (shift) {
            case LSL_VALUE:
                op2 = lsl(state, rm, operand, sf);
                break;
            case LSR_VALUE:
                op2 = lsr(state, rm, operand, sf);
                break;
            case ASR_VALUE:
                op2 = asr(state, rm, operand, sf);
                break;
            case ROR_VALUE:
                op2 = ror(state, rm, operand, sf);
                break;
            default:
                assert(false); // Should never happen
        }
        if (N == 0) {
            switch (opc) {
                case AND_OPC:
                    and (state, rd, rn, op2, sf);
                    break;
                case ORR_OPC:
                    orr(state, rd, rn, op2, sf);
                    break;
                case EON_OPC:
                    eor(state, rd, rn, op2, sf);
                    break;
                case ANDS_OPC:
                    ands(state, rd, rn, op2, sf);
                    break;
                default:
                    assert(false); // Should never happen
            }
        }
        if (N == 1) {
            switch (opc) {
                case AND_OPC:
                    bic(state, rd, rn, op2, sf);
                    break;
                case ORR_OPC:
                    orn(state, rd, rn, op2, sf);
                    break;
                case EON_OPC:
                    eon(state, rd, rn, op2, sf);
                    break;
                case ANDS_OPC:
                    bics(state, rd, rn, op2, sf);
                    break;
                default:
                    assert(false); // Should never happen
            }
        }
    }
    // Check if Multiply
    if (m == MULTIPLY_M && CHECK_BITS(opr, MULTIPLY_MASK, MULTIPLY_VALUE)) {
        uint8_t x = SELECT_BITS(instruction, REG_X_OFFSET, REG_M_SIZE);
        uint8_t ra = SELECT_BITS(instruction, REG_RA_OFFSET, REG_RA_SIZE);
        if (x == MADD_X) {
            madd(state, rd, ra, rn, rm, sf);
        }
        if (x == MSUB_X) {
            msub(state, rd, ra, rn, rm, sf);
        }
    }
}
