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

void write_to_reg(state_t *state, uint32_t val, uint8_t reg) {
  uint64_t rev_val = 0;
  for (int i = 0; i < BYTES_IN_32; i++) {
    uint8_t byte = (val >> (i * 8)) & 0xFF;
    rev_val |= byte << ((3 - i) * 8);
  }
  state->R[reg].X = rev_val;
}

//check: I don't think rt and all reg inputs actually need to be type uint8?
void ldr_litr_32(state_t *state, int64_t simm19, uint8_t rt) {
  int64_t offset = SIGN_EXT(simm19 * 4, 19, 64);
  //check: adding uint54 and int64, possibly need casting
  write_to_reg(state, read_from_mem(state->PC.X + offset), rt);
}

void ldr_uimm_32(state_t *state, uint64_t imm12, uint8_t xn, uint8_t rt) {
  write_to_reg(state, read_from_mem(state->R[xn].X + imm12), rt);
  //imm12 must be in range 0 to 2^14 (16380), and multiple of 4
}

void ldr_preind_32(state_t *state, int64_t simm9, uint8_t xn, uint8_t rt) {
  //simm9 must be in range -256 to 255;
  int64_t offset = SIGNEXT(simm9, 9, 64);
  write_to_reg(state, read_from_mem(state->R[xn].X + offset), xn);
  write_to_reg(state, read_from_mem(state->R[xn].X), rt)
}

void ldr_postind_32(state_t *state, int64_t simm9, uint8_t xn, uint8_t rt) {
  //simm9 must be in range -256 to 255;
  int64_t offset = SIGNEXT(simm9, 9, 64);
  write_to_reg(state, read_from_mem(state->R[xn].X), rt);
  write_to_reg(state, read_from_mem(state->R[xn].X + offset), xn);
}

void ldr_regOffset_32(state_t *state, uint8_t xm, uint8_t xn, uint8_t rt) {
  write_to_reg(state, read_from_mem(state->R[xn].X + state->R[xm].X), rt)
}

void execute_load_32(state_t *state, uint32_t instruction, uint8_t *mem) {
  main_memory = mem;
}
