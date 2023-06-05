#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "emulate.h"
#include "dp_reg.h"
#include "dp_imm.h"


void and_64(state_t *state, uint8_t dest, uint8_t src1, uint64_t op2) {
  state->R[dest].X = state->R[src1].X & op2;  
}

void bic_64(state_t *state, uint8_t dest, uint8_t src1, uint64_t op2) {
  state->R[dest].X = state->R[src1].X & ~op2;  
}

void orr_64(state_t *state, uint8_t dest, uint8_t src1, uint64_t op2) {
  state->R[dest].X = state->R[src1].X | op2;
}

void orn_64(state_t *state, uint8_t dest, uint8_t src1, uint64_t op2) {
  state->R[dest].X = state->R[src1].X | ~op2;  
}

void eon_64(state_t *state, uint8_t dest, uint8_t src1, uint64_t op2) {
  state->R[dest].X = state->R[src1].X ^ ~op2;  
}

void eor_64(state_t *state, uint8_t dest, uint8_t src1, uint64_t op2) {
  state->R[dest].X = state->R[src1].X ^ op2;  
}

void ands_64(state_t *state, uint8_t dest, uint8_t src1, uint64_t op2) {
  state->R[dest].X = state->R[src1].X & op2;  
  set_NV_flags_64(state, state->R[dest].X);
  state->PSTATE.C = 0;
  state->PSTATE.V = 0;
}

void bics_64(state_t *state, uint8_t dest, uint8_t src1, uint64_t op2) {
  state->R[dest].X = state->R[src1].X & ~op2;  
  set_NV_flags_64(state, state->R[dest].X);
  printf("state->R[dest].X: %ld\n", state->R[dest].X);
  state->PSTATE.C = 0;
  state->PSTATE.V = 0;
}

void madd_64(state_t *state, uint8_t dest, uint8_t src, uint8_t rn, uint8_t rm) {
  state->R[dest].X = state->R[src].X + state->R[rn].X * state->R[rm].X;
}

void msub_64(state_t *state, uint8_t dest, uint8_t src, uint8_t rn, uint8_t rm) {
  state->R[dest].X = state->R[src].X - state->R[rn].X * state->R[rm].X;
}

uint64_t lsl_64(state_t *state, uint8_t operand_reg, uint8_t shift_amount) {
    return state->R[operand_reg].X << shift_amount;
}

uint64_t lsr_64(state_t *state, uint8_t operand_reg, uint8_t shift_amount) {
    return state->R[operand_reg].X >>shift_amount;
}

uint64_t asr_64(state_t *state, uint8_t operand_reg, uint8_t shift_amount) {
  int64_t result = state->R[operand_reg].X;
  result >>= shift_amount;
  return result;
}

uint64_t ror_64(state_t *state, uint8_t operand_reg, uint8_t shift_amount) {
    // uint64_t mask = ((1l << (shift_amount + 1 )) -1);
    printf("state->R[operand_reg].X: %lx\n", state->R[operand_reg].X);
    printf("shift_amount: %d\n", shift_amount);
    uint64_t lower_bits = SELECT_BITS(state->R[operand_reg].X, 0, shift_amount);
    printf("lower_bits: %llx\n", lower_bits);
    uint64_t upper_bits = SELECT_BITS(state->R[operand_reg].X, shift_amount, 64-shift_amount);
    printf("upper_bits: %lx\n", upper_bits);
    lower_bits <<= (64-shift_amount);
    // uint64_t result = state->R[operand_reg].X >> shift_amount;
    upper_bits |= lower_bits;
    return upper_bits;


    
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
  printf("result: %ld\n", result);
  set_NV_flags_64(state, result);
  state->PSTATE.C = (state->R[src1].X < state->R[src2].X);
  state->PSTATE.V = ((state->R[src1].X >> 63) != (state->R[src2].X >> 63)) 
                    && (result >> 63 != (state->R[src2].X >> 63));
}


void execute_dpreg_instruction_64(state_t *state, uint32_t instruction) {
  printf("in function execute dpreg instruction 64\n");
  assert(SELECT_BITS(instruction, DPREG_OFFSET, DPREG_SIZE) == 0x5);
  uint8_t sf = SELECT_BITS(instruction, REG_SF_OFFSET, REG_SF_SIZE);
  uint8_t opc = SELECT_BITS(instruction, REG_OPC_OFFSET, REG_OPC_SIZE);
  uint8_t m = SELECT_BITS(instruction, REG_M_OFFSET, REG_M_SIZE);
  uint8_t opr = SELECT_BITS(instruction, REG_OPR_OFFSET, REG_OPR_SIZE);
  uint8_t rm = SELECT_BITS(instruction, REG_RM_OFFSET, REG_RM_SIZE);
  uint8_t operand = SELECT_BITS(instruction, REG_OPERAND_OFFSET, REG_OPERAND_SIZE);
  uint8_t rn = SELECT_BITS(instruction, REG_RN_OFFSET, REG_RN_SIZE);
  uint8_t rd = SELECT_BITS(instruction, REG_RD_OFFSET, REG_RD_SIZE);
  
  // printf("m = %d, opr = %d\n", m, opr);
  // Check if arithmetic
  if (m == ARITHMETIC_M && CHECK_BITS(opr, ARITHMETIC_MASK, ARITHMETIC_VALUE)) {
    uint8_t shift = SELECT_BITS(opr, SHIFT_OFFSET, SHIFT_SIZE);
    assert(sf == SF_64);
    assert(operand < 64);
    uint64_t op2 = state->R[rm].X;
    switch (shift)
    {
      case LSL_VALUE:
        op2 = lsl_64(state, rm, operand);
        break;
      case LSR_VALUE:
        op2 = lsr_64(state, rm, operand);
        break;
      case ASR_VALUE:
        op2 = asr_64(state, rm, operand);
        break;
    }
    switch(opc) {
        case ADD_OPC:
          add_64_imm(state, rd, rn, op2);
          break;
        case SUB_OPC:  
          sub_64_imm(state, rd, rn, op2);
          break;
        case ADDS_OPC:
          adds_64_imm(state, rd, rn, op2);
          break;
        case SUBS_OPC:
          subs_64_imm(state, rd, rn, op2);
          break;

    }
  }

  // Check if bit-logic
  if (m == BIT_LOGIC_M && CHECK_BITS(opr, BIT_LOGIC_MASK, BIT_LOGIC_VALUE)) {
    uint8_t shift = SELECT_BITS(opr, SHIFT_OFFSET, SHIFT_SIZE);
    uint8_t N = SELECT_BITS(opr, N_OFFSET, N_SIZE);
    // assert (shift == ROR_VALUE);
    assert(operand < 64);
    // ror_64(state, rm, operand);
    uint64_t op2 = state->R[rm].X;

    switch(shift) {
      case LSL_VALUE:
        op2 = lsl_64(state, rm, operand);
        break;
      case LSR_VALUE:
        op2 = lsr_64(state, rm, operand);
        break;
      case ASR_VALUE:
        op2 = asr_64(state, rm, operand);
        break;
      case ROR_VALUE:
        op2 = ror_64(state, rm, operand);
        break;
    }
    if (N == 0) {
      switch(opc) {
        case AND_OPC:
          and_64(state, rd, rn, op2);
          break;
        case ORR_OPC:
          printf("orr running\n");
          orr_64(state, rd, rn, op2);
          break;
        case EON_OPC:
          eor_64(state, rd, rn, op2);
          break;
        case ANDS_OPC:
          ands_64(state, rd, rn, op2);
          break;
      }
    }
    if (N == 1) {
      switch(opc) {
        case AND_OPC:
          bic_64(state, rd, rn, op2);
          break;
        case ORR_OPC:
          orn_64(state, rd, rn, op2);
          break;
        case EON_OPC:
          eon_64(state, rd, rn, op2);
          break;
        case ANDS_OPC:
          bics_64(state, rd, rn, op2);
          break;
    }
  }

  }
  printf("m = %d, opr = %d\n", m, opr);
  // Check if Multiply
  if (m == MULTIPLY_M && CHECK_BITS(opr, MULTIPLY_MASK, MULTIPLY_VALUE)) {
    printf("Multiply ran\n");
      uint8_t x = SELECT_BITS(instruction,REG_X_OFFSET , REG_M_SIZE);
      uint8_t ra = SELECT_BITS(instruction, REG_RA_OFFSET, REG_RA_SIZE);
      assert (sf == SF_64);
      if (x == MADD_X) {
          madd_64(state, rd, ra, rn, rm);
      }
      if (x == MSUB_X){
          msub_64(state, rd, ra, rn, rm);
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
