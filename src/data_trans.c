#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "emulate.h"
#include "data_trans.h"

void sdt_uimm_32(state_t *cpu_state, uint64_t imm12, uint8_t xn, uint8_t rt, uint8_t l) {
  assert(l == LOAD_L || l == STORE_L);
  uint64_t xn_value = get_register_value_64(cpu_state, xn);
  uint32_t rt_value = get_register_value_32(cpu_state, rt);
  if (l == LOAD_L) {
    set_register_value_32(cpu_state, rt, fetch_word(xn_value + imm12));
  }
  else if (l == STORE_L) {
    write_word(xn_value + imm12, rt_value);
  }
}

void sdt_uimm_64(state_t *cpu_state, uint64_t imm12, uint8_t xn, uint8_t rt, uint8_t l) {
  assert(l == LOAD_L || l == STORE_L);
  uint64_t xn_value = get_register_value_64(cpu_state, xn);
  uint64_t rt_value = get_register_value_64(cpu_state, rt);
  if (l == LOAD_L) {
    set_register_value_64(cpu_state, rt, fetch_word_64(xn_value + imm12));
  }
  else if (l == STORE_L) {
    write_word_64(xn_value + imm12, rt_value);
  }
}


void sdt_regoffset_32(state_t *cpu_state, uint8_t xm, uint8_t xn, uint8_t rt, uint8_t l) {
  assert(l == LOAD_L || l == STORE_L);
  uint64_t xn_value = get_register_value_64(cpu_state, xn);
  uint64_t xm_value = get_register_value_64(cpu_state, xm);
  uint32_t rt_value = get_register_value_32(cpu_state, rt);
  if (l == LOAD_L) {
    set_register_value_32(cpu_state, rt, fetch_word(xn_value + xm_value));
  }
  else if (l == STORE_L) {
    write_word(xn_value + xm_value, rt_value);
  }
}

void sdt_regoffset_64(state_t *cpu_state, uint8_t xm, uint8_t xn, uint8_t rt, uint8_t l) {
  assert(l == LOAD_L || l == STORE_L);
  uint64_t xn_value = get_register_value_64(cpu_state, xn);
  uint64_t xm_value = get_register_value_64(cpu_state, xm);
  uint64_t rt_value = get_register_value_64(cpu_state, rt);
  if (l == LOAD_L) {
    set_register_value_64(cpu_state, rt, fetch_word_64(xn_value + xm_value));
  }
  else if (l == STORE_L) {
    write_word_64(xn_value + xm_value, rt_value);
  }
}

void sdt_preind_32(state_t *cpu_state, int64_t simm9, uint8_t xn, uint8_t rt, uint8_t l) {
  assert(l == LOAD_L || l == STORE_L);
  uint64_t xn_value = get_register_value_64(cpu_state, xn) + simm9;
  uint32_t rt_value = get_register_value_32(cpu_state, rt);
  set_register_value_64(cpu_state, xn, xn_value);
  if (l == LOAD_L) {
    set_register_value_32(cpu_state, rt, fetch_word(xn_value));
  }
  else if (l == STORE_L) {
    write_word(xn_value, rt_value);
  }
}

void sdt_preind_64(state_t *cpu_state, int64_t simm9, uint8_t xn, uint8_t rt, uint8_t l) {
  assert(l == LOAD_L || l == STORE_L);
  uint64_t xn_value = get_register_value_64(cpu_state, xn);
  uint64_t rt_value = get_register_value_64(cpu_state, rt);
  set_register_value_64(cpu_state, xn, xn_value + simm9);
  if (l == LOAD_L) {
    set_register_value_64(cpu_state, rt, fetch_word_64(xn_value + simm9));
  }
  else if (l == STORE_L) {
    write_word_64(xn_value + simm9, rt_value);
  }
}

void sdt_postind_32(state_t *cpu_state, int64_t simm9, uint8_t xn, uint8_t rt, uint8_t l) {
  assert(l == LOAD_L || l == STORE_L);
  uint64_t xn_value = get_register_value_64(cpu_state, xn);
  uint32_t rt_value = get_register_value_32(cpu_state, rt);
  if (l == LOAD_L) {
    set_register_value_32(cpu_state, rt, fetch_word(xn_value));
  }
  else if (l == STORE_L) {
    write_word(xn_value, rt_value);
  }
  set_register_value_64(cpu_state, xn, xn_value + simm9);
}

void sdt_postind_64(state_t *cpu_state, int64_t simm9, uint8_t xn, uint8_t rt, uint8_t l) {
  assert(l == LOAD_L || l == STORE_L);
  uint64_t xn_value = get_register_value_64(cpu_state, xn);
  uint64_t rt_value = get_register_value_64(cpu_state, rt);
  if (l == LOAD_L) {
    set_register_value_64(cpu_state, rt, fetch_word_64(xn_value));
  }
  else if (l == STORE_L) {
    write_word_64(xn_value, rt_value);
  }
  set_register_value_64(cpu_state, xn, xn_value + simm9);
}

void execute_load_literal_32(state_t *cpu_state, uint32_t instruction) {
    uint8_t rt = SELECT_BITS(instruction, DT_RT_OFFSET, DT_RT_SIZE);
    int64_t simm19 = SELECT_BITS(instruction, DT_SIMM19_OFFSET, DT_SIMM19_SIZE);
    simm19 = SIGN_EXT(simm19, 19, 64) * 4;
    set_register_value_32(cpu_state, rt, cpu_state->PC.X + simm19);
}

void execute_load_literal_64(state_t *cpu_state, uint32_t instruction) {
    uint8_t rt = SELECT_BITS(instruction, DT_RT_OFFSET, DT_RT_SIZE);
    int64_t simm19 = SELECT_BITS(instruction, DT_SIMM19_OFFSET, DT_SIMM19_SIZE);
    simm19 = SIGN_EXT(simm19, 19, 64) * 4;
    set_register_value_64(cpu_state, rt, fetch_word_64(cpu_state->PC.X + simm19));
}

void execute_sdt_32(state_t *cpu_state, uint32_t instruction) {
  uint8_t rt = SELECT_BITS(instruction, DT_RT_OFFSET, DT_RT_SIZE);
  uint8_t xn = SELECT_BITS(instruction, SDT_XN_OFFSET, SDT_XN_SIZE);
  uint8_t l = SELECT_BITS(instruction, SDT_L_OFFSET, SDT_L_SIZE);
  uint8_t u = SELECT_BITS(instruction, SDT_U_OFFSET, SDT_U_SIZE);
  uint32_t offset = SELECT_BITS(instruction, SDT_IMM12_OFFSET, SDT_IMM12_SIZE);

  if (u == UNSIGNED_OFFSET_U) {
      uint64_t imm12 = offset * 4;
      sdt_uimm_32(cpu_state, imm12, xn, rt, l);
  } else if (CHECK_BITS(offset, REG_OFFSET_MASK, REG_OFFSET_VALUE)) {
      uint8_t xm = SELECT_BITS(instruction, REG_OFFSET_XM_OFFSET, REG_OFFSET_XM_SIZE);
      sdt_regoffset_32(cpu_state, xm, xn, rt, l);
  } else if (CHECK_BITS(offset, PRE_POST_MASK, PRE_POST_VALUE)) {
      int64_t simm9 = SELECT_BITS(instruction, PRE_POST_SIMM9_OFFSET, PRE_POST_SIMM9_SIZE);
      simm9 = SIGN_EXT(simm9, 9, 64);
      uint8_t i = SELECT_BITS(instruction, PRE_POST_I_OFFSET, PRE_POST_I_SIZE);

      if (i == PRE_INDEX_I) {
          sdt_preind_32(cpu_state, simm9, xn, rt, l);
      } else if (i == POST_INDEX_I) {
          sdt_postind_32(cpu_state, simm9, xn, rt, l);
      }
  }
  else {
    fprintf(stderr, "Illegal state: invalid single data transfer instruction\n");
    fprintf(stderr, "Exiting!\n");
    exit(EXIT_FAILURE);
  }
}

void execute_sdt_64(state_t *cpu_state, uint32_t instruction) {
  uint8_t rt = SELECT_BITS(instruction, DT_RT_OFFSET, DT_RT_SIZE);
  uint8_t xn = SELECT_BITS(instruction, SDT_XN_OFFSET, SDT_XN_SIZE);
  uint8_t l = SELECT_BITS(instruction, SDT_L_OFFSET, SDT_L_SIZE);
  uint8_t u = SELECT_BITS(instruction, SDT_U_OFFSET, SDT_U_SIZE);
  uint16_t offset = SELECT_BITS(instruction, SDT_OFFSET_OFFSET, SDT_OFFSET_SIZE);

  if (u == UNSIGNED_OFFSET_U) {
    uint64_t imm12 = offset * 8;
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
    } else if (i == POST_INDEX_I) {
        sdt_postind_64(cpu_state, simm9, xn, rt, l);
    }
  }
  else {
    fprintf(stderr, "Illegal state: invalid single data transfer instruction\n");
    fprintf(stderr, "Exiting!\n");
    exit(EXIT_FAILURE);
  } 
}
