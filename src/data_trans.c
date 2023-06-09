#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "emulate.h"
#include "data_trans.h"

void sdt_uimm(state_t *cpu_state, uint64_t imm12, uint8_t xn, uint8_t rt, uint8_t l, uint8_t sf) {
  assert(l == LOAD_L || l == STORE_L);
  assert(sf == SF_32 || sf == SF_64);
  uint64_t xn_value = get_register_value(cpu_state, xn, SF_64);
  uint64_t rt_value = get_register_value(cpu_state, rt, SF_64);
  if (l == LOAD_L) {
    set_register_value(cpu_state, rt, fetch_word(xn_value + imm12, sf), sf);
  }
  else if (l == STORE_L) {
    write_word(xn_value + imm12, rt_value, sf);
  }
}

void sdt_regoffset(state_t *cpu_state, uint8_t xm, uint8_t xn, uint8_t rt, uint8_t l, uint8_t sf) {
  assert(l == LOAD_L || l == STORE_L);
  uint64_t xn_value = get_register_value(cpu_state, xn, SF_64);
  uint64_t xm_value = get_register_value(cpu_state, xm, SF_64);
  uint64_t rt_value = get_register_value(cpu_state, rt, sf);
  if (l == LOAD_L) {
    set_register_value(cpu_state, rt, fetch_word(xn_value + xm_value, sf), sf);
  }
  else if (l == STORE_L) {
    write_word(xn_value + xm_value, rt_value, sf);
  }
}

void sdt_preind(state_t *cpu_state, int64_t simm9, uint8_t xn, uint8_t rt, uint8_t l, uint8_t sf) {
  assert(l == LOAD_L || l == STORE_L);
  uint64_t xn_value = get_register_value(cpu_state, xn, SF_64);
  uint64_t rt_value = get_register_value(cpu_state, rt, sf);
  set_register_value(cpu_state, xn, xn_value + simm9, sf);
  if (l == LOAD_L) {
    set_register_value(cpu_state, rt, fetch_word(xn_value + simm9, sf), sf);
  }
  else if (l == STORE_L) {
    write_word(xn_value + simm9, rt_value, sf);
  }
}

void sdt_postind(state_t *cpu_state, int64_t simm9, uint8_t xn, uint8_t rt, uint8_t l, uint8_t sf) {
  assert(l == LOAD_L || l == STORE_L);
  uint64_t xn_value = get_register_value(cpu_state, xn, SF_64);
  uint64_t rt_value = get_register_value(cpu_state, rt, sf);
  if (l == LOAD_L) {
    set_register_value(cpu_state, rt, fetch_word(xn_value, sf), sf);
  }
  else if (l == STORE_L) {
    write_word(xn_value, rt_value, sf);
  }
  set_register_value(cpu_state, xn, xn_value + simm9, sf);
}

void execute_load_literal(state_t *cpu_state, uint32_t instruction, uint8_t sf) {
    assert(sf == SF_32 || sf == SF_64);
    uint8_t rt = SELECT_BITS(instruction, DT_RT_OFFSET, DT_RT_SIZE);
    int64_t simm19 = SELECT_BITS(instruction, DT_SIMM19_OFFSET, DT_SIMM19_SIZE);
    simm19 = SIGN_EXT(simm19, 19, 64) * 4;
    uint64_t value = fetch_word(cpu_state->PC.X + simm19, sf);
    set_register_value(cpu_state, rt, value, sf);
}

void execute_sdt(state_t *cpu_state, uint32_t instruction, uint8_t sf) {
  uint8_t rt = SELECT_BITS(instruction, DT_RT_OFFSET, DT_RT_SIZE);
  uint8_t xn = SELECT_BITS(instruction, SDT_XN_OFFSET, SDT_XN_SIZE);
  uint8_t l = SELECT_BITS(instruction, SDT_L_OFFSET, SDT_L_SIZE);
  uint8_t u = SELECT_BITS(instruction, SDT_U_OFFSET, SDT_U_SIZE);
  uint16_t offset = SELECT_BITS(instruction, SDT_OFFSET_OFFSET, SDT_OFFSET_SIZE);

  if (u == UNSIGNED_OFFSET_U) {
    uint64_t imm12 = offset * 8;
    sdt_uimm(cpu_state, imm12, xn, rt, l, sf);
  }
  else if (CHECK_BITS(offset, REG_OFFSET_MASK, REG_OFFSET_VALUE)) {
    uint8_t xm = SELECT_BITS(instruction, REG_OFFSET_XM_OFFSET, REG_OFFSET_XM_SIZE);
    sdt_regoffset(cpu_state, xm, xn, rt, l, sf);
  }
  else if (CHECK_BITS(offset, PRE_POST_MASK, PRE_POST_VALUE)) {
    int64_t simm9 = SELECT_BITS(instruction, PRE_POST_SIMM9_OFFSET, PRE_POST_SIMM9_SIZE);
    simm9 = SIGN_EXT(simm9, 9, 64);
    uint8_t i = SELECT_BITS(instruction, PRE_POST_I_OFFSET, PRE_POST_I_SIZE);

    if (i == PRE_INDEX_I) {
        sdt_preind(cpu_state, simm9, xn, rt, l, sf);
    } else if (i == POST_INDEX_I) {
        sdt_postind(cpu_state, simm9, xn, rt, l, sf);
    }
  }
  else {
    fprintf(stderr, "Illegal state: invalid single data transfer instruction\n");
    fprintf(stderr, "Exiting!\n");
    exit(EXIT_FAILURE);
  } 
}
