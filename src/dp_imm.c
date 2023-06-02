#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "emulate.h"
#include "dp_imm.h"


//TOOO: double check flags, especially V
void set_NV_flags_64(state_t *state, uint64_t result) {
  state->PSTATE.N = (result >> 63) & 1;
  state->PSTATE.Z = (result == 0);
}

void set_NV_flags_32(state_t *state, uint32_t result) {
  state->PSTATE.N = (result >> 31) & 1;
  state->PSTATE.Z = (result == 0);
}

void add_64_imm(state_t *state, uint8_t dest, uint8_t src1, uint32_t imm) {
  state->R[dest].X = state->R[src1].X + imm;
}

void add_32_imm(state_t *state, uint8_t dest, uint8_t src1, uint32_t imm) {
  state->R[dest].W = state->R[src1].W + imm;
}

void adds_64_imm(state_t *state, uint8_t dest, uint8_t src1, uint32_t imm) {
  uint64_t result = state->R[src1].X + imm;
  state->R[dest].X = result;
  set_NV_flags_64(state, result);
  state->PSTATE.C = (result < state->R[src1].X);
  state->PSTATE.V = (state->R[src1].X >> 63 == imm >> 63) && (result >> 63 != imm >> 63);
}

void adds_32_imm(state_t *state, uint8_t dest, uint8_t src1, uint32_t imm) {
  uint32_t result = state->R[src1].W + imm;
  state->R[dest].W = result;
  set_NV_flags_32(state, result);
  state->PSTATE.C = (result < state->R[src1].W);
  state->PSTATE.V = (state->R[src1].W >> 31 == imm >> 31) && (result >> 31 != imm >> 31);
}

void sub_64_imm(state_t *state, uint8_t dest, uint8_t src1, uint32_t imm) {
  state->R[dest].X = state->R[src1].X - imm;
}

void sub_32_imm(state_t *state, uint8_t dest, uint8_t src1, uint32_t imm) {
  state->R[dest].W = state->R[src1].W - imm;
}

void subs_64_imm(state_t *state, uint8_t dest, uint8_t src1, uint32_t imm) {
  uint64_t result = state->R[src1].X - imm;
  state->R[dest].X = result;
  set_NV_flags_64(state, result);
  state->PSTATE.C = (result > state->R[src1].X);
  state->PSTATE.V = (state->R[src1].X >> 63 != imm >> 63) && (result >> 63 == imm >> 63);
}

void subs_32_imm(state_t *state, uint8_t dest, uint8_t src1, uint32_t imm) {
  uint32_t result = state->R[src1].W - imm;
  state->R[dest].W = result;
  set_NV_flags_32(state, result);
  state->PSTATE.C = (result > state->R[src1].W);
  state->PSTATE.V = (state->R[src1].W >> 31 != imm >> 31) && (result >> 31 == imm >> 31);
}


