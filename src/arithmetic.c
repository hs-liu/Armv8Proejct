#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "emulate.h"
#include "arithmetic.h"

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

void and_64(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
  state->R[dest].X = state->R[src1].X & state->R[src2].X;  
}

void and_32(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
  state->R[dest].W = state->R[src1].W & state->R[src2].W;  
}

void bic_64(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
  state->R[dest].X = state->R[src1].X & ~state->R[src2].X;  
}

void bic_32(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
  state->R[dest].W = state->R[src1].W & ~state->R[src2].W;  
}

void orr_64(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
  state->R[dest].X = state->R[src1].X | state->R[src2].X;
}

void orr_32(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
  state->R[dest].W = state->R[src1].W | state->R[src2].W;  
}

void orn_64(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
  state->R[dest].X = state->R[src1].X | ~state->R[src2].X;  
}

void orn_32(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
  state->R[dest].W = state->R[src1].W | ~state->R[src2].W;
}

void eon_64(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
  state->R[dest].X = state->R[src1].X ^ ~state->R[src2].X;  
}

void eon_32(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
  state->R[dest].W = state->R[src1].W ^ ~state->R[src2].W;  
}

void ands_64(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
  state->R[dest].X = state->R[src1].X & state->R[src2].X;  
  set_NV_flags_64(state, state->R[dest].X);
  state->PSTATE.C = 0;
  state->PSTATE.V = 0;
}

void ands_32(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
  state->R[dest].W = state->R[src1].W & state->R[src2].W;  
  set_NV_flags_32(state, state->R[dest].W);
  state->PSTATE.C = 0;
  state->PSTATE.V = 0;
}

void bics_64(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
  state->R[dest].X = state->R[src1].X & ~state->R[src2].X;  
  set_NV_flags_64(state, state->R[dest].X);
  state->PSTATE.C = 0;
  state->PSTATE.V = 0;
}

void bics_32(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
  state->R[dest].W = state->R[src1].W & ~state->R[src2].W;  
  set_NV_flags_32(state, state->R[dest].W);
  state->PSTATE.C = 0;
  state->PSTATE.V = 0;
}

void madd_64(state_t *state, uint8_t dest, uint8_t src, uint8_t rn, uint8_t rm) {
  state->R[dest].X = state->R[src].X + state->R[rn].X * state->R[rm].X;
}

void madd_32(state_t *state, uint8_t dest, uint8_t src, uint8_t rn, uint8_t rm) {
  state->R[dest].W = state->R[src].W + state->R[rn].W * state->R[rm].W;
}

void msub_64(state_t *state, uint8_t dest, uint8_t src, uint8_t rn, uint8_t rm) {
  state->R[dest].X = state->R[src].X - state->R[rn].X * state->R[rm].X;
}

void msub_32(state_t *state, uint8_t dest, uint8_t src, uint8_t rn, uint8_t rm) {
  state->R[dest].W = state->R[src].W - state->R[rn].W * state->R[rm].W;
}

void lsl_64(state_t *state, uint8_t operand_reg, uint8_t shift_amount) {
    state->R[operand_reg].X <<= shift_amount;
}

void lsl_32(state_t *state, uint8_t operand_reg, uint8_t shift_amount) {
    state->R[operand_reg].W <<= shift_amount;
    state->R[operand_reg].X &= 0x00000000FFFFFFFF;
}

void lsr_64(state_t *state, uint8_t operand_reg, uint8_t shift_amount) {
    state->R[operand_reg].X >>=shift_amount;
}

void lsr_32(state_t *state, uint8_t operand_reg, uint8_t shift_amount) {
    state->R[operand_reg].W >>= shift_amount;
    state->R[operand_reg].X &= 0x00000000FFFFFFFF;
}

void asr_64(state_t *state, uint8_t operand_reg, uint8_t shift_amount) {
    uint64_t sign_bit = state->R[operand_reg].X >> 63;
    // printf("sign-bit = %llx", sign_bit);
    state->R[operand_reg].X >>= shift_amount;
    uint64_t mask = ((sign_bit << (shift_amount+1)) - 1) << (64-shift_amount);
    if (sign_bit == 1) {
        state->R[operand_reg].X |= mask;
    }
}

void asr_32(state_t *state, uint8_t operand_reg, uint8_t shift_amount) {
    uint32_t sign_bit = state->R[operand_reg].W >> 31;
    state->R[operand_reg].X &= 0x00000000FFFFFFFF;
    state->R[operand_reg].W >>= shift_amount;
    uint32_t mask = ((sign_bit << (shift_amount+1))-1) << (32-shift_amount);
    if (sign_bit == 1) {
        state->R[operand_reg].X |= mask;
    }
}

void ror_64(state_t *state, uint8_t operand_reg, uint8_t shift_amount) {
    uint64_t mask = ((1 << (shift_amount + 1 )) -1);
    uint64_t lower_bits = state->R[operand_reg].X & mask;
    lower_bits <<= (64-shift_amount);
    state->R[operand_reg].X >>= shift_amount;
    state->R[operand_reg].X |= lower_bits;
}

void ror_32(state_t *state, uint8_t operand_reg, uint8_t shift_amount) {
    uint32_t mask = ((1 << (shift_amount + 1))-1);
    uint32_t lower_bits = state->R[operand_reg].X & mask;
    lower_bits <<= (32-shift_amount);
    state->R[operand_reg].X &= 0x00000000FFFFFFFF;
    state->R[operand_reg].W >>= shift_amount;
    state->R[operand_reg].W |= lower_bits;
}


void execute_dpreg_instruction(state_t *state, uint32_t instruction) {
  assert(SELECT_BITS(instruction, DPREG_OFFSET, DPREG_SIZE) == 0x5);
  uint8_t sf = SELECT_BITS(instruction, SF_OFFSET, SF_SIZE);
  uint8_t opc = SELECT_BITS(instruction, OPC_OFFSET, OPC_SIZE);
  uint8_t m = SELECT_BITS(instruction, M_OFFSET, M_SIZE);
  uint8_t opr = SELECT_BITS(instruction, OPR_OFFSET, OPR_SIZE);
  uint8_t rm = SELECT_BITS(instruction, RM_OFFSET, RM_SIZE);
  uint8_t operand = SELECT_BITS(instruction, OPERAND_OFFSET, OPERAND_SIZE);
  uint8_t rn = SELECT_BITS(instruction, RN_OFFSET, RN_SIZE);
  uint8_t rd = SELECT_BITS(instruction, RD_OFFSET, RD_SIZE);

  // Check if arithmetic
  if (m == ARITHMETIC_M && CHECK_BITS(opr, ARITHMETIC_MASK, ARITHMETIC_VALUE)) {
    uint8_t shift = SELECT_BITS(opr, SHIFT_OFFSET, SHIFT_SIZE);
    if (sf = SF_32) {
      assert(operand < 32);
    switch (shift)
    {
      case LSL_VALUE:
        lsl_32(state, rm, operand);
        break;
      case LSR_VALUE:
        lsr_32(state, rm, operand);
        break;
      case ASR_VALUE:
        asr_32(state, rm, operand);
        break;
    }
    //TODO ARITHMETIC 32 (SAME AS IMMEDIATE)
    }
    if (sf = SF_64) {
      assert(operand < 64);
    switch (shift)
    {
      case LSL_VALUE:
        lsl_64(state, rm, operand);
        break;
      case LSR_VALUE:
        lsr_64(state, rm, operand);
        break;
      case ASR_VALUE:
        asr_64(state, rm, operand);
        break;
    }
    //TODO ARITHMETIC 64 (SAME AS IMMEDIATE)
    }
  }



  // Check if bit-logic
  if (m == BIT_LOGIC_M && CHECK_BITS(opr, BIT_LOGIC_MASK, BIT_LOGIC_VALUE)) {
    uint8_t shift = SELECT_BITS(opr, SHIFT_OFFSET, SHIFT_SIZE);
    uint8_t N = SELECT_BITS(opr, N_OFFSET, N_SIZE);
    assert (shift == ROR_VALUE);
    if (sf = SF_32) {
    assert(operand < 32);
    ror_32(state, rm, operand);
    if (N == 0) {
      switch(opc) {
        case AND_OPC:
          and_32(state, rd, rn, rm);
          break;
        case ORR_OPC:
          orr_32(state, rd, rn, rm);
          break;
        case EON_OPC:
          eon_32(state, rd, rn, rm);
          break;
        case ANDS_OPC:
          ands_32(state, rd, rn, rm);
          break;
      }
    }
    if (N == 1) {
      switch(opc) {
        case AND_OPC:
          bic_32(state, rd, rn, rm);
          break;
        case ORR_OPC:
          orn_32(state, rd, rn, rm);
          break;
        case EON_OPC:
          eor_32(state, rd, rn, rm);
          break;
        case ANDS_OPC:
          bics_32(state, rd, rn, rm);
          break;
      }
    }


  }

  // Check if Multiply
  if (m == MULTIPLY_M && CHECK_BITS(opr, MULTIPLY_MASK, MULTIPLY_VALUE)) {
      uint8_t x = SELECT_BITS(operand, 5, 1);
      uint8_t ra = SELECT_BITS(operand, 0, 5);
      if (sf == SF_32 && x == MADD_X) {
          madd_32(&state, rd, ra, rn, rm);
      }
      if (sf == SF_32 && x == MSUB_X) {
          msub_32(&state, rd, ra, rn, rm);
      }
      if (sf == SF_64 && x == MADD_X) {
          madd_64(&state, rd, ra, rn, rm);
      }
      if (sf == SF_64 && x == MSUB_X){
          msub_64(&state, rd, ra, rn, rm);
        }
  }

  }
}


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
