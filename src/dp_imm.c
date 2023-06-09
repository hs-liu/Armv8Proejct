#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "emulate.h"
#include "dp_imm.h"

void add_imm(state_t *cpu_state, uint8_t dest, uint8_t src1, uint64_t imm, uint8_t sf) {
    uint64_t result = get_register_value(cpu_state, src1, sf) + imm;
    set_register_value(cpu_state, dest, result, sf);
}

void adds_imm(state_t *cpu_state, uint8_t dest, uint8_t src1, uint64_t imm, uint8_t sf) {
    uint64_t reg_value = get_register_value(cpu_state, src1, sf);
    uint64_t result = reg_value + imm;
    set_register_value(cpu_state, dest, result, sf);
    set_NV_flags(cpu_state, result, sf);
    int msb_index = sf == SF_32 ? 31 : 63;
    result = sf == SF_32 ? (uint32_t)result : (uint64_t)result;
    cpu_state->PSTATE.C = (result < reg_value);
    cpu_state->PSTATE.V = (SELECT_BITS(reg_value, msb_index, 1) == SELECT_BITS(imm, msb_index, 1));
    cpu_state->PSTATE.V = cpu_state->PSTATE.V && (SELECT_BITS(result, msb_index, 1) != SELECT_BITS(imm, msb_index, 1));
}

void sub_imm(state_t *cpu_state, uint8_t dest, uint8_t src1, uint64_t imm, uint8_t sf) {
    uint64_t result = get_register_value(cpu_state, src1, sf) - imm;
    set_register_value(cpu_state, dest, result, sf);
}

void subs_imm(state_t *cpu_state, uint8_t dest, uint8_t src1, uint64_t imm, uint8_t sf) {
    uint64_t reg_value = get_register_value(cpu_state, src1, sf);
    uint64_t result = reg_value - imm;
    set_register_value(cpu_state, dest, result, sf);
    set_NV_flags(cpu_state, result, sf);
    int msb_index = sf == SF_32 ? 31 : 63;
    result = sf == SF_32 ? (uint32_t)result : (uint64_t)result;
    cpu_state->PSTATE.C = !(result > reg_value);
    cpu_state->PSTATE.V = (SELECT_BITS(reg_value, msb_index, 1) != SELECT_BITS(imm, msb_index, 1));
    cpu_state->PSTATE.V = cpu_state->PSTATE.V && (SELECT_BITS(result, msb_index, 1) == SELECT_BITS(imm, msb_index, 1));

}

void movn_imm(state_t *cpu_state, uint8_t dest, uint64_t imm, uint8_t sf) {
    set_register_value(cpu_state, dest, ~imm, sf);
}

void movz_imm(state_t *cpu_state, uint8_t dest, uint64_t imm, uint8_t sf) {
    set_register_value(cpu_state, dest, imm, sf);
}

void movk_imm(state_t *cpu_state, uint8_t dest, uint8_t hw, uint64_t imm, uint8_t sf) {
    uint64_t mask = 0xFFFF;
    mask = mask << (hw * 16);
    uint64_t reg_value = get_register_value(cpu_state, dest, sf);
    uint64_t result = (reg_value & ~mask) | (imm << (hw * 16));
    set_register_value(cpu_state, dest, result, sf);
}

void execute_dpimm_instruction(state_t *cpu_state, uint32_t instruction) {
    uint8_t sf = SELECT_BITS(instruction, IMM_SF_OFFSET, IMM_SF_SIZE);
    uint8_t opc = SELECT_BITS(instruction, IMM_OPC_OFFSET, IMM_OPC_SIZE);
    uint8_t opi = SELECT_BITS(instruction, IMM_OPI_OFFSET, IMM_OPI_SIZE);
    uint8_t rd = SELECT_BITS(instruction, IMM_RD_OFFSET, IMM_RD_SIZE);

    assert(SELECT_BITS(instruction, IMM_OFFSET, IMM_SIZE) == IMM_VALUE);
    assert(sf == SF_32 || sf == SF_64);

    if (opi == IMM_ARITHMETIC_OPI) {
        uint8_t sh = SELECT_BITS(instruction, IMM_SH_OFFSET, IMM_SH_SIZE);
        uint32_t imm12 = SELECT_BITS(instruction, IMM_IMM12_OFFSET, IMM_IMM12_SIZE);
        uint8_t rn = SELECT_BITS(instruction, IMM_RN_OFFSET, IMM_RN_SIZE);
        if (sh == 1) {
            imm12 = imm12 << 12;
        }
        switch (opc) {
            case ADD_OPC:
                add_imm(cpu_state, rd, rn, imm12, sf);
                break;
            case ADDS_OPC:
                adds_imm(cpu_state, rd, rn, imm12, sf);
                break;
            case SUB_OPC:
                sub_imm(cpu_state, rd, rn, imm12, sf);
                break;
            case SUBS_OPC:
                subs_imm(cpu_state, rd, rn, imm12, sf);
                break;
        }
    }

    if (opi == IMM_WIDE_MOVE_OPI) {
        uint8_t hw = SELECT_BITS(instruction, IMM_HW_OFFSET, IMM_HW_SIZE);
        uint64_t imm16 = SELECT_BITS(instruction, IMM_IMM16_OFFSET, IMM_IMM16_SIZE);
        uint64_t op = imm16 << (16 * hw);

        switch (opc) {
            case MOVN_OPC:
                movn_imm(cpu_state, rd, op, sf);
                break;
            case MOVZ_OPC:
                movz_imm(cpu_state, rd, op, sf);
                break;
            case MOVK_OPC:
                movk_imm(cpu_state, rd, hw, imm16, sf);
                break;
        }
    }
}
