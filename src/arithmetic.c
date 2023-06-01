#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "emulate.h"
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

void madd_64(state_t *state, int dest, int src, int rn, int rm) {
  state->R[dest].X = state->R[src].X + state->R[rn].X * state->R[rm].X;
}

void madd_32(state_t *state, int dest, int src, int rn, int rm) {
  state->R[dest].W = state->R[src].W + state->R[rn].W * state->R[rm].W;
}

void msub_64(state_t *state, int dest, int src, int rn, int rm) {
  state->R[dest].X = state->R[src].X - state->R[rn].X * state->R[rm].X;
}

void msub_32(state_t *state, int dest, int src, int rn, int rm) {
  state->R[dest].W = state->R[src].W - state->R[rn].W * state->R[rm].W;
}

void lsl_64(state_t *state, int operand_reg, int shift_amount) {
    state->R[operand_reg].X <<= shift_amount;
}

void lsl_32(state_t *state, int operand_reg, int shift_amount) {
    state->R[operand_reg].W <<= shift_amount;
    state->R[operand_reg].X &= 0x00000000FFFFFFFF;
}

void lsr_64(state_t *state, int operand_reg, int shift_amount) {
    state->R[operand_reg].X >>=shift_amount;
}

void lsr_32(state_t *state, int operand_reg, int shift_amount) {
    state->R[operand_reg].W >>= shift_amount;
    state->R[operand_reg].X &= 0x00000000FFFFFFFF;
}

void asr_64(state_t *state, int operand_reg, int shift_amount) {
    uint64_t sign_bit = state->R[operand_reg].X >> 63;
    // printf("sign-bit = %llx", sign_bit);
    state->R[operand_reg].X >>= shift_amount;
    uint64_t mask = ((sign_bit << (shift_amount+1)) - 1) << (64-shift_amount);
    if (sign_bit == 1) {
        state->R[operand_reg].X |= mask;
    }
}

void asr_32(state_t *state, int operand_reg, int shift_amount) {
    uint32_t sign_bit = state->R[operand_reg].W >> 31;
    state->R[operand_reg].X &= 0x00000000FFFFFFFF;
    state->R[operand_reg].W >>= shift_amount;
    uint32_t mask = ((sign_bit << (shift_amount+1))-1) << (32-shift_amount);
    if (sign_bit == 1) {
        state->R[operand_reg].X |= mask;
    }
}

void ror_64(state_t *state, int operand_reg, int shift_amount) {
    uint64_t mask = ((1 << (shift_amount + 1 )) -1);
    uint64_t lower_bits = state->R[operand_reg].X & mask;
    lower_bits <<= (64-shift_amount);
    state->R[operand_reg].X >>= shift_amount;
    state->R[operand_reg].X |= lower_bits;
}

void ror_32(state_t *state, int operand_reg, int shift_amount) {
    uint32_t mask = ((1 << (shift_amount + 1))-1);
    uint32_t lower_bits = state->R[operand_reg].X & mask;
    lower_bits <<= (32-shift_amount);
    state->R[operand_reg].X &= 0x00000000FFFFFFFF;
    state->R[operand_reg].W >>= shift_amount;
    state->R[operand_reg].W |= lower_bits;
}

// void execute_dpreg_instruction(state_t *state, uint32_t instruction) {
     
// }


int main(int argc, char **argv) {

    state_t DUM_STATE = {
        .R = {
            [0].X = 0xFFFF00000000FFFF,
            [1].X = 0xFFFF00000000FFFF,
            [2].X = 0xFFFF00000000FFFF,
            [3].X = 0xFFFF00000000FFFF,
            [4].X = 0x0FFFF0000000FFFF,
            [5].X = 0xFFFF00000000FFFF,
            [6].X = 0x00FF00000000FFFF,
            [7].X = 0xFFFF00000000FFF0,
        },
        .ZR = {0},
        .PC = {0},
        .SP = {0},
        .PSTATE = {0}
    };

    lsl_64(&DUM_STATE, 0, 4);
    lsl_32(&DUM_STATE, 1, 4);
    lsr_64(&DUM_STATE, 2, 4);
    asr_64(&DUM_STATE, 3, 4);
    asr_64(&DUM_STATE, 4, 4);
    asr_32(&DUM_STATE, 5, 4);
    ror_64(&DUM_STATE, 6, 8);
    ror_32(&DUM_STATE, 7, 8);

    for (int i = 0; i<8; i++) {
        printf("register %d = %016lX\n", i, DUM_STATE.R[i].X);
    }

  return EXIT_SUCCESS;
}
