#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "emulate.h"
#include "dp_reg.h"
#include "dp_imm.h"


void and_64(state_t *state, uint8_t dest, uint8_t src1, uint64_t op2) {
    uint64_t result = get_register_value_64(state, src1) & op2;
    set_register_value_64(state, dest, result);
}

void bic_64(state_t *state, uint8_t dest, uint8_t src1, uint64_t op2) {
    uint64_t result = get_register_value_64(state, src1) & ~op2;
    set_register_value_64(state, dest, result);
}

void orr_64(state_t *state, uint8_t dest, uint8_t src1, uint64_t op2) {
    uint64_t result = get_register_value_64(state, src1) | op2;
    set_register_value_64(state, dest, result);
}

void orn_64(state_t *state, uint8_t dest, uint8_t src1, uint64_t op2) {
    uint64_t result = get_register_value_64(state, src1) | ~op2;
    set_register_value_64(state, dest, result);
}

void eon_64(state_t *state, uint8_t dest, uint8_t src1, uint64_t op2) {
    uint64_t result = get_register_value_64(state, src1) ^ ~op2;
    set_register_value_64(state, dest, result);
}

void eor_64(state_t *state, uint8_t dest, uint8_t src1, uint64_t op2) {
    uint64_t result = get_register_value_64(state, src1) ^ op2;
    set_register_value_64(state, dest, result);
}

void ands_64(state_t *state, uint8_t dest, uint8_t src1, uint64_t op2) {
    uint64_t result = get_register_value_64(state, src1) & op2;
    set_register_value_64(state, dest, result);
    set_NV_flags_64(state, result);
    state->PSTATE.C = 0;
    state->PSTATE.V = 0;
}

void bics_64(state_t *state, uint8_t dest, uint8_t src1, uint64_t op2) {
    uint64_t result = get_register_value_64(state, src1) & ~op2;
    set_register_value_64(state, dest, result);
    set_NV_flags_64(state, result);
    state->PSTATE.C = 0;
    state->PSTATE.V = 0;
}

void madd_64(state_t *state, uint8_t dest, uint8_t src, uint8_t rn, uint8_t rm) {
    uint64_t result = get_register_value_64(state, src);
    result += get_register_value_64(state, rn) * get_register_value_64(state, rm);
    set_register_value_64(state, dest, result);
}

void msub_64(state_t *state, uint8_t dest, uint8_t src, uint8_t rn, uint8_t rm) {
    uint64_t result = get_register_value_64(state, src);
    result -= get_register_value_64(state, rn) * get_register_value_64(state, rm);
    set_register_value_64(state, dest, result);
}

uint64_t lsl_64(state_t *state, uint8_t operand_reg, uint8_t shift_amount) {
    return get_register_value_64(state, operand_reg) << shift_amount;
}

uint64_t lsr_64(state_t *state, uint8_t operand_reg, uint8_t shift_amount) {
    return get_register_value_64(state, operand_reg) >> shift_amount;
}

uint64_t asr_64(state_t *state, uint8_t operand_reg, uint8_t shift_amount) {
    int64_t result = get_register_value_64(state, operand_reg);
    result >>= shift_amount;
    return (uint64_t) result;
}

uint64_t ror_64(state_t *state, uint8_t operand_reg, uint8_t shift_amount) {
    uint64_t operand = get_register_value_64(state, operand_reg);
    uint64_t lower_bits = SELECT_BITS(operand, 0, shift_amount);
    uint64_t upper_bits = SELECT_BITS(operand, shift_amount, 64 - shift_amount);
    lower_bits <<= (64 - shift_amount);
    upper_bits |= lower_bits;
    return upper_bits;
}

void execute_dpreg_instruction_64(state_t *state, uint32_t instruction) {
    assert(SELECT_BITS(instruction, DPREG_OFFSET, DPREG_SIZE) == 0x5);
    uint8_t sf = SELECT_BITS(instruction, REG_SF_OFFSET, REG_SF_SIZE);
    uint8_t opc = SELECT_BITS(instruction, REG_OPC_OFFSET, REG_OPC_SIZE);
    uint8_t m = SELECT_BITS(instruction, REG_M_OFFSET, REG_M_SIZE);
    uint8_t opr = SELECT_BITS(instruction, REG_OPR_OFFSET, REG_OPR_SIZE);
    uint8_t rm = SELECT_BITS(instruction, REG_RM_OFFSET, REG_RM_SIZE);
    uint8_t operand = SELECT_BITS(instruction, REG_OPERAND_OFFSET, REG_OPERAND_SIZE);
    uint8_t rn = SELECT_BITS(instruction, REG_RN_OFFSET, REG_RN_SIZE);
    uint8_t rd = SELECT_BITS(instruction, REG_RD_OFFSET, REG_RD_SIZE);

    // Check if arithmetic
    if (m == ARITHMETIC_M && CHECK_BITS(opr, ARITHMETIC_MASK, ARITHMETIC_VALUE)) {
        uint8_t shift = SELECT_BITS(opr, SHIFT_OFFSET, SHIFT_SIZE);
        assert(sf == SF_64);
        assert(operand < 64);
        uint64_t op2;
        switch (shift) {
            case LSL_VALUE:
                op2 = lsl_64(state, rm, operand);
                break;
            case LSR_VALUE:
                op2 = lsr_64(state, rm, operand);
                break;
            case ASR_VALUE:
                op2 = asr_64(state, rm, operand);
                break;
            default:
                fprintf(stderr, "Illegal instruction: invalid shift value\n");
                fprintf(stderr, "Exiting!\n");
                exit(EXIT_FAILURE);
        }
        switch (opc) {
            case ADD_OPC:
                add_64_imm(state, rd, rn, op2);
                break;
            case SUB_OPC:
                sub_64_imm(state, rd, rn, op2);
                break;
            case ADDS_OPC:
                adds_64_imm(state, rd, rn, op2);
                break;
            case SUBS_OPC:
                subs_64_imm(state, rd, rn, op2);
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
        assert(operand < 64);
        uint64_t op2;
        switch (shift) {
            case LSL_VALUE:
                op2 = lsl_64(state, rm, operand);
                break;
            case LSR_VALUE:
                op2 = lsr_64(state, rm, operand);
                break;
            case ASR_VALUE:
                op2 = asr_64(state, rm, operand);
                break;
            case ROR_VALUE:
                op2 = ror_64(state, rm, operand);
                break;
            default:
                assert(false); // Should never happen
        }
        if (N == 0) {
            switch (opc) {
                case AND_OPC:
                    and_64(state, rd, rn, op2);
                    break;
                case ORR_OPC:
                    orr_64(state, rd, rn, op2);
                    break;
                case EON_OPC:
                    eor_64(state, rd, rn, op2);
                    break;
                case ANDS_OPC:
                    ands_64(state, rd, rn, op2);
                    break;
                default:
                    assert(false); // Should never happen
            }
        }
        if (N == 1) {
            switch (opc) {
                case AND_OPC:
                    bic_64(state, rd, rn, op2);
                    break;
                case ORR_OPC:
                    orn_64(state, rd, rn, op2);
                    break;
                case EON_OPC:
                    eon_64(state, rd, rn, op2);
                    break;
                case ANDS_OPC:
                    bics_64(state, rd, rn, op2);
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
        assert (sf == SF_64);
        if (x == MADD_X) {
            madd_64(state, rd, ra, rn, rm);
        }
        if (x == MSUB_X) {
            msub_64(state, rd, ra, rn, rm);
        }
    }
}