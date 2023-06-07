#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "emulate.h"
#include "data_trans.h"

void sdt_uimm_64(state_t *cpu_state, uint64_t imm12, uint8_t xn, uint8_t rt, uint8_t l) {
  if (l == LOAD_L) {
    cpu_state->R[rt].X = fetch_word_64(cpu_state->R[xn].X + imm12);
  }
  if (l == STORE_L) {
    write_word_64(cpu_state->R[xn].X + imm12, cpu_state->R[rt].X);
  }
}

void sdt_regoffset_64(state_t *cpu_state, uint8_t xm, uint8_t xn, uint8_t rt, uint8_t l) {
  if (l == LOAD_L) {
    cpu_state->R[rt].X = fetch_word_64(cpu_state->R[xn].X + cpu_state->R[xm].X);
  }
  if (l == STORE_L) {
    write_word_64(cpu_state->R[xn].X + cpu_state->R[xm].X, cpu_state->R[rt].X);
  }
}

void sdt_preind_64(state_t *cpu_state, int64_t simm9, uint8_t xn, uint8_t rt, uint8_t l) {
  cpu_state->R[xn].X = cpu_state->R[xn].X + simm9;
  if (l == LOAD_L) {
    cpu_state->R[rt].X = fetch_word_64(cpu_state->R[xn].X);
  }
  if (l == STORE_L) {
    write_word_64(cpu_state->R[xn].X, cpu_state->R[rt].X);
  }
}

void sdt_postind_64(state_t *cpu_state, int64_t simm9, uint8_t xn, uint8_t rt, uint8_t l) {
  if (l == LOAD_L) {
    cpu_state->R[rt].X = fetch_word_64(cpu_state->R[xn].X);
  }
  if (l == STORE_L) {
    // printf("write_word_64(%x, %x)\n", cpu_state->R[xn].X, cpu_state->R[rt].X);
    write_word_64(cpu_state->R[xn].X, cpu_state->R[rt].X);
  }
  cpu_state->R[xn].X = cpu_state->R[xn].X + simm9;
}

void execute_load_literal_64(state_t *cpu_state, uint32_t instruction) {
  uint8_t rt = SELECT_BITS(instruction, DT_RT_OFFSET, DT_RT_SIZE);
  int64_t simm19 = SELECT_BITS(instruction, DT_SIMM19_OFFSET, DT_SIMM19_SIZE);
  simm19 = SIGN_EXT(simm19, 19, 64) * 4;
  cpu_state->R[rt].X = fetch_word_64(cpu_state->PC.X + simm19);
}

void execute_sdt_64(state_t *cpu_state, uint32_t instruction) {
  uint8_t rt = SELECT_BITS(instruction, DT_RT_OFFSET, DT_RT_SIZE);
  uint8_t xn = SELECT_BITS(instruction, SDT_XN_OFFSET, SDT_XN_SIZE);
  uint8_t l = SELECT_BITS(instruction, SDT_L_OFFSET, SDT_L_SIZE);
  uint8_t u = SELECT_BITS(instruction, SDT_U_OFFSET, SDT_U_SIZE);
  uint16_t offset = SELECT_BITS(instruction, SDT_OFFSET_OFFSET, SDT_OFFSET_SIZE);

  if (u == UNSIGNED_OFFSET_U) {
    uint64_t imm12 = SELECT_BITS(instruction, SDT_IMM12_OFFSET, SDT_IMM12_SIZE);
    imm12 *= 8;
    sdt_uimm_64(cpu_state, imm12, xn, rt, l);
  }
  else if (CHECK_BITS(offset, REG_OFFSET_MASK, REG_OFFSET_VALUE)) {
    uint8_t xm = SELECT_BITS(instruction, REG_OFFSET_XM_OFFSET, REG_OFFSET_XM_SIZE);
    sdt_regoffset_64(cpu_state, xm, xn, rt, l);
  }
  else if (CHECK_BITS(offset, PRE_POST_MASK, PRE_POST_VALUE)) {
    int64_t simm9 = SELECT_BITS(instruction, PRE_POST_SIMM9_OFFSET, PRE_POST_SIMM9_SIZE);
    simm9 = SIGN_EXT(simm9, 9, 64);
    uint8_t i = SELECT_BITS(instruction, PRE_POST_I_OFFSET, PRE_POST_I_SIZE);

    if (i == PRE_INDEX_I) {
      sdt_preind_64(cpu_state, simm9, xn, rt, l);
    }
    else if (i == POST_INDEX_I) {
      sdt_postind_64(cpu_state, simm9, xn, rt, l);
    }
  }
}
