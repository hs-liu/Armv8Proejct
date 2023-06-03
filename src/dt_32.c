#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "emulate.h"
#include "data_trans.h"

uint8_t *main_memory;

uint32_t read_from_mem(uint64_t addr) {
  uint32_t value = 0;
  for (int i = 0; i < BYTES_IN_32; i++) {
    value |= (uint32_t)main_memory[addr + i] << (i * 8);
  }
  return value;
}

void write_to_mem(uint64_t addr, uint64_t val) {
  for (int i = 0; i < BYTES_IN_32; i++) {
    main_memory[addr + i] = val >> (i * 8) & 0xFF;
}

//check: I don't think rt and all reg inputs actually need to be type uint8?
void ldr_litr_32(state_t *state, int64_t simm19, uint8_t rt) {
  //int64_t offset = SIGN_EXT(simm19 * 4, 19, 64);
  //check: adding uint54 and int64, possibly need casting
  state->R[rt].W = read_from_mem(state->PC.X + SIGN_EXT(simm19 * 4, 19, 64));
}

void ldr_uimm_32(state_t *state, uint64_t imm12, uint8_t xn, uint8_t rt) {
  state->R[rt].W = read_from_mem(state->R[xn].X + imm12);
  //imm12 must be in range 0 to 2^14 (16380), and multiple of 4
}

void str_uimm_32(state_t *state, uint64_t imm12, uint8_t xn, uint8_t rt) {
  write_to_mem(state->R[xn].X + imm12, state->R[rt].X);
}

void ldr_preind_32(state_t *state, int64_t simm9, uint8_t xn, uint8_t rt) {
  //simm9 must be in range -256 to 255;
  //int64_t offset = SIGNEXT(simm9, 9, 64);
  state->R[xn].X = (uint64_t) read_from_mem(state->R[xn].X + SIGNEXT(simm9, 9, 64));
  state->R[rt].W = read_from_mem(state->R[xn].X);
}

void str_preind_32(state_t *state, int64_t simm9, uint8_t xn, uint8_t rt) {
  state->R[xn].X = (uint64_t) read_from_mem(state->R[xn].X + SIGNEXT(simm9, 9, 64));
  write_to_mem((state->R[xn].X, state->R[rt].X);
}

void ldr_postind_32(state_t *state, int64_t simm9, uint8_t xn, uint8_t rt) {
  //simm9 must be in range -256 to 255;
  //int64_t offset = SIGNEXT(simm9, 9, 64);
  state->R[rt].W = read_from_mem(state->R[xn].X);
  state->R[xn].X = read_from_mem(state->R[xn].X + SIGNEXT(simm9, 9, 64));
}

void str_postind_32(state_t *state, int64_t simm9, uint8_t xn, uint8_t rt) {
  write_to_mem(state->R[xn].X, state->R[rt].X);
  state->R[xn].X = read_from_mem(state->R[xn].X + SIGNEXT(simm9, 9, 64));
}

void ldr_regOffset_32(state_t *state, uint8_t xm, uint8_t xn, uint8_t rt) {
  state->R[rt].W = read_from_mem(state->R[xn].X + state->R[xm].X);
}

void str_regOffset_32(state_t *state, uint8_t xm, uint8_t xn, uint8_t rt) {
  write_to_mem(state->R[xn].X + state->R[xm].X, state->R[rt].X);
}

void execute_load_32(state_t *state, uint32_t instruction, uint8_t *mem) {
  main_memory = mem;
  uint8_t sf = SELECT_BITS(instruction, SF_OFFSET, SF_SIZE);
  //from spec: l defines ldr/str, u defines unsigned offset mode
  uint8_t l = SELECT_BITS(instruction, L_OFFSET, L_SIZE);
  uint8_t u = SELECT_BITS(instruction, U_OFFSET, U_SIZE);
  uint8_t offset = SELECT_BITS(instruction, OFFSET_OFFSET, OFFSET_SIZE);
  uint8_t rt = SELECT_BITS(instruction, RT_OFFSET, RT_SIZE);

  assert(sf == SF_32);

  if (SELECT_BITS(instruction, LDR_LITR_OFFSET, LDR_LITR_SIZE) == 0) {
    uint64_t simm19 = SELECT_BITS(instruction, SIMM19_OFFSET, SIMM19_SIZE);
    ldr_litr_32(state, simm19, rt);
  } else {
    uint8_t xn = SELECT_BITS(instruction, XN_OFFSET, XN_SIZE);
    uint8_t xm = SELECT_BITS(instruction, XM_OFFSET, XM_SIZE);
    uint8_t i = SELECT_BITS(instruction, I_OFFSET, I_SIZE);
    if (l == L_IS_LDR) {
      if (u == U_IS_UOFF) {
        ldr_uimm_32(state, imm12, xn, rt);
      } else {
        if (SELECT_BITS(instruction, REG_OFF_OFFSET, REG_OFF_SIZE) == IS_REG_OFF) {
          ldr_regOffset_32(state, xm, xn, rt);
        } else {
          if (i == I_IS_PRE) {
            ldr_preind_32(state, simm9, xn, rt);
          } else {
            ldr_postind_32(state, simm9, xn, rt);
          }
        }
      }
    } else {
      if (u == U_IS_UOFF) {
        str_uimm_32(state, imm12, xn, rt);
      } else {
        if (SELECT_BITS(instruction, REG_OFF_OFFSET, REG_OFF_SIZE) == IS_REG_OFF) {
          str_regOffset_32(state, xm, xn, rt);
        } else {
          if (i == I_IS_PRE) {
            str_preind_32(state, simm9, xn, rt);
          } else {
            str_postind_32(state, simm9, xn, rt);
          }
        }
      }
    }
  }
}


/*uint64_t reverse_64(uint64_t val) {
  uint64_t rev_val = 0;
  for (int i = 0; i < BYTES_IN_32; i++) {
    uint8_t byte = (val >> (i * 8)) & 0xFF;
    rev_val |= byte << ((3 - i) * 8);
  }
  return rev_val;
}*/

/*
void write_to_reg(state_t *state, uint64_t val, uint8_t reg) {
  uint64_t rev_val = reverse_64(val);
  state->R[reg].X = rev_val;
}*/
