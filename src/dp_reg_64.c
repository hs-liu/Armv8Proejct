#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "emulate.h"
#include "dp_reg.h"


void and_64(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
  state->R[dest].X = state->R[src1].X & state->R[src2].X;  
}

void bic_64(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
  state->R[dest].X = state->R[src1].X & ~state->R[src2].X;  
}

void orr_64(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
  state->R[dest].X = state->R[src1].X | state->R[src2].X;
}

void orn_64(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
  state->R[dest].X = state->R[src1].X | ~state->R[src2].X;  
}

void eon_64(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
  state->R[dest].X = state->R[src1].X ^ ~state->R[src2].X;  
}

void eor_64(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
  state->R[dest].X = state->R[src1].X ^ state->R[src2].X;  
}

void ands_64(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
  state->R[dest].X = state->R[src1].X & state->R[src2].X;  
  set_NV_flags_64(state, state->R[dest].X);
  state->PSTATE.C = 0;
  state->PSTATE.V = 0;
}

void bics_64(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
  state->R[dest].X = state->R[src1].X & ~state->R[src2].X;  
  set_NV_flags_64(state, state->R[dest].X);
  state->PSTATE.C = 0;
  state->PSTATE.V = 0;
}

void madd_64(state_t *state, uint8_t dest, uint8_t src, uint8_t rn, uint8_t rm) {
  state->R[dest].X = state->R[src].X + state->R[rn].X * state->R[rm].X;
}

void msub_64(state_t *state, uint8_t dest, uint8_t src, uint8_t rn, uint8_t rm) {
  state->R[dest].X = state->R[src].X - state->R[rn].X * state->R[rm].X;
}

void lsl_64(state_t *state, uint8_t operand_reg, uint8_t shift_amount) {
    state->R[operand_reg].X <<= shift_amount;
}

void lsr_64(state_t *state, uint8_t operand_reg, uint8_t shift_amount) {
    state->R[operand_reg].X >>=shift_amount;
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

void ror_64(state_t *state, uint8_t operand_reg, uint8_t shift_amount) {
    uint64_t mask = ((1 << (shift_amount + 1 )) -1);
    uint64_t lower_bits = state->R[operand_reg].X & mask;
    lower_bits <<= (64-shift_amount);
    state->R[operand_reg].X >>= shift_amount;
    state->R[operand_reg].X |= lower_bits;
}

void add_64(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
  state->R[dest].X = state->R[src1].X + state->R[src2].X;
}

void adds_64(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
 uint64_t result = state->R[src1].X + state->R[src2].X;
  if (dest != ZR_REG) {
    state->R[dest].X = result;
  }
  set_NV_flags_64(state, result);
  state->PSTATE.C = (result < state->R[src1].X);
  state->PSTATE.V = ((state->R[src1].X >> 63) == (state->R[src2].X >> 63)) 
                    && (result >> 63 != (state->R[src2].X >> 63));
}

void sub_64(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
  state->R[dest].X = state->R[src1].X - state->R[src2].X;
}

void subs_64(state_t *state, uint8_t dest, uint8_t src1, uint8_t src2) {
  uint64_t result = state->R[src1].X - state->R[src2].X;
  if (dest != ZR_REG) {
    state->R[dest].X = result;
  }
  set_NV_flags_64(state, result);
  state->PSTATE.C = (state->R[src1].X < state->R[src2].X);
  state->PSTATE.V = ((state->R[src1].X >> 63) != (state->R[src2].X >> 63)) 
                    && (result >> 63 != (state->R[src2].X >> 63));
}


void execute_dpreg_instruction_64(state_t *state, uint32_t instruction) {
  assert(SELECT_BITS(instruction, DPREG_OFFSET, DPREG_SIZE) == 0x5);
  uint8_t sf = SELECT_BITS(instruction, REG_SF_OFFSET, REG_SF_SIZE);
  uint8_t opc = SELECT_BITS(instruction, REG_OPC_OFFSET, REG_OPC_SIZE);
  uint8_t m = SELECT_BITS(instruction, REG_M_OFFSET, REG_M_SIZE);
  uint8_t opr = SELECT_BITS(instruction, REG_OPR_OFFSET, REG_OPR_SIZE);
  uint8_t rm = SELECT_BITS(instruction, REG_RM_OFFSET, REG_RM_SIZE);
  uint8_t operand = SELECT_BITS(instruction, REG_OPERAND_OFFSET, REG_OPERAND_SIZE);
  uint8_t rn = SELECT_BITS(instruction, REG_RN_OFFSET, REG_RN_SIZE);
  uint8_t rd = SELECT_BITS(instruction, REG_RD_OFFSET, REG_RD_SIZE);

  // Check if arithmetic
  if (m == ARITHMETIC_M && CHECK_BITS(opr, ARITHMETIC_MASK, ARITHMETIC_VALUE)) {
    uint8_t shift = SELECT_BITS(opr, SHIFT_OFFSET, SHIFT_SIZE);
    assert(sf == SF_64);
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
    switch(opc) {
        case ADD_OPC:
          add_64(state, rd, rn, rm);
          break;
        case SUB_OPC:  
          sub_64(state, rd, rn, rm);
          break;
        case ADDS_OPC:
          adds_64(state, rd, rn, rm);
          break;
        case SUBS_OPC:
          subs_64(state, rd, rn, rm);
          break;

    }
  }

  // Check if bit-logic
  if (m == BIT_LOGIC_M && CHECK_BITS(opr, BIT_LOGIC_MASK, BIT_LOGIC_VALUE)) {
    uint8_t shift = SELECT_BITS(opr, SHIFT_OFFSET, SHIFT_SIZE);
    uint8_t N = SELECT_BITS(opr, N_OFFSET, N_SIZE);
    assert (shift == ROR_VALUE);
    assert(operand < 32);
    ror_64(state, rm, operand);
    if (N == 0) {
      switch(opc) {
        case AND_OPC:
          and_64(state, rd, rn, rm);
          break;
        case ORR_OPC:
          orr_64(state, rd, rn, rm);
          break;
        case EON_OPC:
          eon_64(state, rd, rn, rm);
          break;
        case ANDS_OPC:
          ands_64(state, rd, rn, rm);
          break;
      }
    }
    if (N == 1) {
      switch(opc) {
        case AND_OPC:
          bic_64(state, rd, rn, rm);
          break;
        case ORR_OPC:
          orn_64(state, rd, rn, rm);
          break;
        case EON_OPC:
          eor_64(state, rd, rn, rm);
          break;
        case ANDS_OPC:
          bics_64(state, rd, rn, rm);
          break;
    }
  }

  // Check if Multiply
  if (m == MULTIPLY_M && CHECK_BITS(opr, MULTIPLY_MASK, MULTIPLY_VALUE)) {
      uint8_t x = SELECT_BITS(operand, 5, 1);
      uint8_t ra = SELECT_BITS(operand, 0, 5);
      assert (sf == SF_64);
      if (x == MADD_X) {
          madd_64(state, rd, ra, rn, rm);
      }
      if (x == MSUB_X){
          msub_64(state, rd, ra, rn, rm);
        }
  }

  }
}


// int main(int argc, char **argv) {

//     state_t DUM_STATE = {
//         .R = {
//             [0].X = 0xFFFF00000000FFFF,
//             [1].X = 0xFFFF00000000FFFF,
//             [2].X = 0xFFFF00000000FFFF,
//             [3].X = 0xFFFF00000000FFFF,
//             [4].X = 0x0FFFF0000000FFFF,
//             [5].X = 0xFFFF00000000FFFF,
//             [6].X = 0x00FF00000000FFFF,
//             [7].X = 0xFFFF00000000FFF0,
//         },
//         .ZR = {0},
//         .PC = {0},
//         .SP = {0},
//         .PSTATE = {0}
//     };

//     lsl_64(&DUM_STATE, 0, 4);
//     lsl_32(&DUM_STATE, 1, 4);
//     lsr_64(&DUM_STATE, 2, 4);
//     asr_64(&DUM_STATE, 3, 4);
//     asr_64(&DUM_STATE, 4, 4);
//     asr_32(&DUM_STATE, 5, 4);
//     ror_64(&DUM_STATE, 6, 8);
//     ror_32(&DUM_STATE, 7, 8);

//     for (int i = 0; i<8; i++) {
//         printf("register %d = %016lX\n", i, DUM_STATE.R[i].X);
//     }

//   return EXIT_SUCCESS;
// }
