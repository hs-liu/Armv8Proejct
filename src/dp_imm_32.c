#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "emulate.h"
#include "dp_imm.h"

void add_32_imm(state_t *state, uint8_t dest, uint8_t src1, uint32_t imm) {
    uint32_t result = get_register_value_32(state, src1) + imm;
    set_register_value_32(state, dest, result);
}

void adds_32_imm(state_t *state, uint8_t dest, uint8_t src1, uint32_t imm) {
    uint32_t reg_value = get_register_value_32(state, src1); 
    uint32_t result = reg_value + imm;
    set_register_value_32(state, dest, result);
    set_NV_flags_32(state, result);
    state->PSTATE.C = (result < reg_value);
    state->PSTATE.V = (reg_value >> 31 == imm >> 31) && (result >> 31 != imm >> 31);
}

void sub_32_imm(state_t *state, uint8_t dest, uint8_t src1, uint32_t imm) {
    uint32_t result = get_register_value_32(state, src1) - imm;
    set_register_value_32(state, dest, result);
}

void subs_32_imm(state_t *state, uint8_t dest, uint8_t src1, uint32_t imm) {
    uint32_t reg_value = get_register_value_32(state, src1);
    uint32_t result = reg_value - imm;
    set_register_value_32(state, dest, result);
    set_NV_flags_32(state, result);
    state->PSTATE.C = !(result > reg_value);
    state->PSTATE.V = (reg_value >> 31 != imm >> 31) && (result >> 31 == imm >> 31);
}

void movn_32_imm(state_t *state, uint8_t dest, uint32_t imm) {
    set_register_value_32(state, dest, ~imm);
}

void movz_32_imm(state_t *state, uint8_t dest, uint32_t imm) {
    set_register_value_32(state, dest, imm);
}

void movk_32_imm(state_t *state, uint8_t dest, uint8_t hw, uint32_t imm) {
    uint32_t mask = 0xFFFF;
    mask = mask << (hw * 16);
    uint32_t reg_value = get_register_value_32(state, dest);
    uint32_t result = (reg_value & ~mask) | (imm << (hw * 16));
    set_register_value_32(state, dest, result);
}

void execute_dpimm_instruction_32(state_t *state, uint32_t instruction) {
    uint8_t sf = SELECT_BITS(instruction, IMM_SF_OFFSET, IMM_SF_SIZE);
    uint8_t opc = SELECT_BITS(instruction, IMM_OPC_OFFSET, IMM_OPC_SIZE);
    uint8_t opi = SELECT_BITS(instruction, IMM_OPI_OFFSET, IMM_OPI_SIZE);
    uint8_t rd = SELECT_BITS(instruction, IMM_RD_OFFSET, IMM_RD_SIZE);

    assert(SELECT_BITS(instruction, IMM_OFFSET, IMM_SIZE) == IMM_VALUE);
    assert(sf == SF_32);

    if (opi == IMM_ARITHMETIC_OPI) {
        uint8_t sh = SELECT_BITS(instruction, IMM_SH_OFFSET, IMM_SH_SIZE);
        uint32_t imm12 = SELECT_BITS(instruction, IMM_IMM12_OFFSET, IMM_IMM12_SIZE);
        uint8_t rn = SELECT_BITS(instruction, IMM_RN_OFFSET, IMM_RN_SIZE);
        if (sh == 1) {
            imm12 = imm12 << 12;
        }
        switch (opc) {
            case ADD_OPC:
                add_32_imm(state, rd, rn, imm12);
                break;
            case ADDS_OPC:
                adds_32_imm(state, rd, rn, imm12);
                break;
            case SUB_OPC:
                sub_32_imm(state, rd, rn, imm12);
                break;
            case SUBS_OPC:
                subs_32_imm(state, rd, rn, imm12);
                break;
        }
    }

    if (opi == IMM_WIDE_MOVE_OPI) {
        uint8_t hw = SELECT_BITS(instruction, IMM_HW_OFFSET, IMM_HW_SIZE);
        uint64_t imm16 = SELECT_BITS(instruction, IMM_IMM16_OFFSET, IMM_IMM16_SIZE);

        uint64_t op = imm16 << (16 * hw);

        switch (opc) {
            case MOVN_OPC:
                movn_32_imm(state, rd, op);
                break;
            case MOVZ_OPC:
                movz_32_imm(state, rd, op);
                break;
            case MOVK_OPC:
                movk_32_imm(state, rd, hw, imm16);
                break;
        }
    }
}