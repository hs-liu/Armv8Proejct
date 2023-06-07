#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "emulate.h"
#include "dp_reg.h"
#include "dp_imm.h"

void and_32(state_t *state, uint8_t dest, uint8_t src1, uint32_t op2) {
    state->R[dest].W = state->R[src1].W & op2;
    state->R[dest].X &= 0x00000000FFFFFFFF;
}

void bic_32(state_t *state, uint8_t dest, uint8_t src1, uint32_t op2) {
    state->R[dest].W = state->R[src1].W & ~op2;
    state->R[dest].X &= 0x00000000FFFFFFFF;
}

void orr_32(state_t *state, uint8_t dest, uint8_t src1, uint32_t op2) {
    state->R[dest].W = state->R[src1].W | op2;
    state->R[dest].X &= 0x00000000FFFFFFFF;
}

void orn_32(state_t *state, uint8_t dest, uint8_t src1, uint32_t op2) {
    printf("orn_32\n");
    state->R[dest].W = state->R[src1].W | ~op2;
    state->R[dest].X &= 0x00000000FFFFFFFF;
}

void eon_32(state_t *state, uint8_t dest, uint8_t src1, uint32_t op2) {
    state->R[dest].W = state->R[src1].W ^ ~op2;
    state->R[dest].X &= 0x00000000FFFFFFFF;
}

void eor_32(state_t *state, uint8_t dest, uint8_t src1, uint32_t op2) {
    state->R[dest].W = state->R[src1].W ^ op2;
    state->R[dest].X &= 0x00000000FFFFFFFF;
}

void ands_32(state_t *state, uint8_t dest, uint8_t src1, uint32_t op2) {
    state->R[dest].W = state->R[src1].W & op2;
    state->R[dest].X &= 0x00000000FFFFFFFF;
    set_NV_flags_32(state, state->R[dest].W);
    state->PSTATE.C = 0;
    state->PSTATE.V = 0;
}

void bics_32(state_t *state, uint8_t dest, uint8_t src1, uint32_t op2) {
    state->R[dest].W = state->R[src1].W & ~op2;
    set_NV_flags_32(state, state->R[dest].W);
    state->R[dest].X &= 0x00000000FFFFFFFF;
    state->PSTATE.C = 0;
    state->PSTATE.V = 0;
}

void madd_32(state_t *state, uint8_t dest, uint8_t src, uint8_t rn, uint8_t rm) {
    state->R[dest].W = state->R[src].W + state->R[rn].W * state->R[rm].W;
    state->R[dest].X &= 0x00000000FFFFFFFF;
}

void msub_32(state_t *state, uint8_t dest, uint8_t src, uint8_t rn, uint8_t rm) {
    state->R[dest].W = state->R[src].W - state->R[rn].W * state->R[rm].W;
    state->R[dest].X &= 0x00000000FFFFFFFF;
    printf("MSUB RAN\n");
}

uint32_t lsl_32(state_t *state, uint8_t operand_reg, uint8_t shift_amount) {
    return state->R[operand_reg].W << shift_amount;

}

uint32_t lsr_32(state_t *state, uint8_t operand_reg, uint8_t shift_amount) {
    return state->R[operand_reg].W >> shift_amount;
}

uint32_t asr_32(state_t *state, uint8_t operand_reg, uint8_t shift_amount) {
    int32_t result = state->R[operand_reg].W;
    result >>= shift_amount;
    return result;

}

uint32_t ror_32(state_t *state, uint8_t operand_reg, uint8_t shift_amount) {
    uint32_t mask = ((1 << (shift_amount + 1)) - 1);
    uint32_t lower_bits = state->R[operand_reg].X & mask;
    lower_bits <<= (32 - shift_amount);
    uint32_t result = state->R[operand_reg].W >> shift_amount;
    result |= lower_bits;
    return result;
}

//TODO: NEED TO DEAL WITH 11111 encoding ZERO register
void execute_dpreg_instruction_32(state_t *state, uint32_t instruction) {
    printf("executing dpreg_32 instruction\n");
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
        assert(sf == SF_32);
        assert(operand < 32);
        uint32_t op2;
        switch (shift) {
            case LSL_VALUE:
                op2 = lsl_32(state, rm, operand);
                break;
            case LSR_VALUE:
                op2 = lsr_32(state, rm, operand);
                break;
            case ASR_VALUE:
                op2 = asr_32(state, rm, operand);
                break;
        }

        switch (opc) {
            case ADD_OPC:
                add_32_imm(state, rd, rn, op2);
                break;
            case ADDS_OPC:
                adds_32_imm(state, rd, rn, op2);
                break;
            case SUB_OPC:
                sub_32_imm(state, rd, rn, op2);
                break;
            case SUBS_OPC:
                subs_32_imm(state, rd, rn, op2);
                break;
        }
    }

    // Check if bit-logic
    if (m == BIT_LOGIC_M && CHECK_BITS(opr, BIT_LOGIC_MASK, BIT_LOGIC_VALUE)) {
        printf("bit logic running\n");
        uint8_t shift = SELECT_BITS(opr, SHIFT_OFFSET, SHIFT_SIZE);
        uint8_t N = SELECT_BITS(opr, N_OFFSET, N_SIZE);
        // assert (shift == ROR_VALUE);
        assert(operand < 32);
        assert(sf == SF_32);
        // ror_32(state, rm, operand);
        uint32_t op2;

        switch (shift) {
            case LSL_VALUE:
                op2 = lsl_32(state, rm, operand);
                break;
            case LSR_VALUE:
                op2 = lsr_32(state, rm, operand);
                break;
            case ASR_VALUE:
                op2 = asr_32(state, rm, operand);
                break;
            case ROR_VALUE:
                op2 = ror_32(state, rm, operand);
                break;
        }

        if (N == 0) {
            switch (opc) {
                case AND_OPC:
                    and_32(state, rd, rn, op2);
                    break;
                case ORR_OPC:
                    orr_32(state, rd, rn, op2);
                    break;
                case EON_OPC:
                    eor_32(state, rd, rn, op2);
                    break;
                case ANDS_OPC:
                    ands_32(state, rd, rn, op2);
                    break;
            }
        }
        if (N == 1) {
            switch (opc) {
                case AND_OPC:
                    bic_32(state, rd, rn, op2);
                    break;
                case ORR_OPC:
                    orn_32(state, rd, rn, op2);
                    break;
                case EON_OPC:
                    eon_32(state, rd, rn, op2);
                    break;
                case ANDS_OPC:
                    bics_32(state, rd, rn, op2);
                    break;
            }
        }


    }
    // Check if Multiply
    if (m == MULTIPLY_M && CHECK_BITS(opr, MULTIPLY_MASK, MULTIPLY_VALUE)) {
        printf("Multiply_32 ran\n");
        uint8_t x = SELECT_BITS(instruction, REG_X_OFFSET, REG_X_SIZE);
        uint8_t ra = SELECT_BITS(instruction, REG_RA_OFFSET, REG_RA_SIZE);
        assert(sf == SF_32);
        if (x == MADD_X) {
            madd_32(state, rd, ra, rn, rm);
        }
        if (x == MSUB_X) {
            msub_32(state, rd, ra, rn, rm);
        }
    }
}