#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#define MEMORY_CAPACITY (2 * 1024 * 1024)

typedef struct {
  bool N;
  bool Z;
  bool C;
  bool V;
} pstate_t;

typedef union {
  uint32_t W;
  uint64_t X;
} reg_t;

typedef struct {
  reg_t R[31];
  reg_t ZR;
  reg_t PC;
  reg_t SP;
  pstate_t PSTATE;
} state_t;

uint8_t main_memory[MEMORY_CAPACITY];

void set_NV_flags_64(state_t *state, uint64_t result) {
  state->PSTATE.N = (result >> 63) & 1;
  state->PSTATE.Z = (result == 0);
}

void set_NV_flags_32(state_t *state, uint32_t result) {
  state->PSTATE.N = (result >> 31) & 1;
  state->PSTATE.Z = (result == 0);
}

void and_64(state_t *state, int dest, int src1, int src2) {
  state->R[dest].X = state->R[src1].X & state->R[src2].X;  
}

void and_32(state_t *state, int dest, int src1, int src2) {
  state->R[dest].W = state->R[src1].W & state->R[src2].W;  
}

void bic_64(state_t *state, int dest, int src1, int src2) {
  state->R[dest].X = state->R[src1].X & ~state->R[src2].X;  
}

void bic_32(state_t *state, int dest, int src1, int src2) {
  state->R[dest].W = state->R[src1].W & ~state->R[src2].W;  
}

void orr_64(state_t *state, int dest, int src1, int src2) {
  state->R[dest].X = state->R[src1].X | state->R[src2].X;
}

void orr_32(state_t *state, int dest, int src1, int src2) {
  state->R[dest].W = state->R[src1].W | state->R[src2].W;  
}

void orn_64(state_t *state, int dest, int src1, int src2) {
  state->R[dest].X = state->R[src1].X | ~state->R[src2].X;  
}

void orn_32(state_t *state, int dest, int src1, int src2) {
  state->R[dest].W = state->R[src1].W | ~state->R[src2].W;
}

void eon_64(state_t *state, int dest, int src1, int src2) {
  state->R[dest].X = state->R[src1].X ^ ~state->R[src2].X;  
}

void eon_32(state_t *state, int dest, int src1, int src2) {
  state->R[dest].W = state->R[src1].W ^ ~state->R[src2].W;  
}

void ands_64(state_t *state, int dest, int src1, int src2) {
  state->R[dest].X = state->R[src1].X & state->R[src2].X;  
  set_NV_flags_64(state, state->R[dest].X);
  state->PSTATE.C = 0;
  state->PSTATE.V = 0;
}

void ands_32(state_t *state, int dest, int src1, int src2) {
  state->R[dest].W = state->R[src1].W & state->R[src2].W;  
  set_NV_flags_32(state, state->R[dest].W);
  state->PSTATE.C = 0;
  state->PSTATE.V = 0;
}

void bics_64(state_t *state, int dest, int src1, int src2) {
  state->R[dest].X = state->R[src1].X & ~state->R[src2].X;  
  set_NV_flags_64(state, state->R[dest].X);
  state->PSTATE.C = 0;
  state->PSTATE.V = 0;
}

void bics_32(state_t *state, int dest, int src1, int src2) {
  state->R[dest].W = state->R[src1].W & ~state->R[src2].W;  
  set_NV_flags_32(state, state->R[dest].W);
  state->PSTATE.C = 0;
  state->PSTATE.V = 0;
}