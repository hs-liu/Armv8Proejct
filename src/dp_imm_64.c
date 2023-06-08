#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "emulate.h"
#include "dp_imm.h"


//TOOO: double check flags, especially V
//TODO: if register is not regular register, but stack pointer


void add_64_imm(state_t *state, uint8_t dest, uint8_t src1, uint64_t imm) {
    state->R[dest].X = state->R[src1].X + imm;
}

void adds_64_imm(state_t *state, uint8_t dest, uint8_t src1, uint64_t imm) {
    uint64_t result = state->R[src1].X + imm;
    if (dest != ZR_REG) {
        state->R[dest].X = result;
    }
    set_NV_flags_64(state, result);
    state->PSTATE.C = (result < state->R[src1].X);
    state->PSTATE.V = (state->R[src1].X >> 63 == imm >> 63) && (result >> 63 != imm >> 63);
}

void sub_64_imm(state_t *state, uint8_t dest, uint8_t src1, uint64_t imm) {
    state->R[dest].X = state->R[src1].X - imm;
}

void subs_64_imm(state_t *state, uint8_t dest, uint8_t src1, uint64_t imm) {
    uint64_t result = state->R[src1].X - imm;
    if (dest != ZR_REG) {
        state->R[dest].X = result;
    }
    set_NV_flags_64(state, result);
    state->PSTATE.C = !(result > state->R[src1].X);
    state->PSTATE.V = (state->R[src1].X >> 63 != imm >> 63) && (result >> 63 == imm >> 63);
}

void movn_64_imm(state_t *state, uint8_t dest, uint64_t imm) {
    state->R[dest].X = ~imm;
}

void movz_64_imm(state_t *state, uint8_t dest, uint64_t imm) {
    state->R[dest].X = imm;
}

void movk_64_imm(state_t *state, uint8_t dest, uint8_t hw, uint64_t imm) {
    uint64_t mask = 0xFFFF;
    mask = mask << (hw * 16);
    state->R[dest].X = (state->R[dest].X & ~mask) | (imm << (hw * 16));
}

void execute_dpimm_instruction_64(state_t *state, uint32_t instruction) {
    uint8_t sf = SELECT_BITS(instruction, IMM_SF_OFFSET, IMM_SF_SIZE);
    uint8_t opc = SELECT_BITS(instruction, IMM_OPC_OFFSET, IMM_OPC_SIZE);
    uint8_t opi = SELECT_BITS(instruction, IMM_OPI_OFFSET, IMM_OPI_SIZE);
    uint8_t rd = SELECT_BITS(instruction, IMM_RD_OFFSET, IMM_RD_SIZE);

    assert(SELECT_BITS(instruction, IMM_OFFSET, IMM_SIZE) == IMM_VALUE);

    assert(sf == SF_64);

    if (opi == IMM_ARITHMETIC_OPI) {
        uint8_t sh = SELECT_BITS(instruction, IMM_SH_OFFSET, IMM_SH_SIZE);
        uint32_t imm12 = SELECT_BITS(instruction, IMM_IMM12_OFFSET, IMM_IMM12_SIZE);
        uint8_t rn = SELECT_BITS(instruction, IMM_RN_OFFSET, IMM_RN_SIZE);
        if (sh == 1) {
            imm12 = imm12 << 12;
        }
        switch (opc) {
            case ADD_OPC:
                add_64_imm(state, rd, rn, imm12);
                break;
            case ADDS_OPC:
                adds_64_imm(state, rd, rn, imm12);
                break;
            case SUB_OPC:
                sub_64_imm(state, rd, rn, imm12);
                break;
            case SUBS_OPC:
                subs_64_imm(state, rd, rn, imm12);
                break;
        }
    }

    if (opi == IMM_WIDE_MOVE_OPI) {
        uint8_t hw = SELECT_BITS(instruction, IMM_HW_OFFSET, IMM_HW_SIZE);
        uint64_t imm16 = SELECT_BITS(instruction, IMM_IMM16_OFFSET, IMM_IMM16_SIZE);
        uint64_t op = imm16 << (16 * hw);

        switch (opc) {
            case MOVN_OPC:
                movn_64_imm(state, rd, op);
                break;
            case MOVZ_OPC:
                movz_64_imm(state, rd, op);
                break;
            case MOVK_OPC:
                movk_64_imm(state, rd, hw, imm16);
                break;
        }
    }
}