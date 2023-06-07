#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "emulate.h"
#include "branch.h"

void branch_instruction(state_t *cpu_state, uint32_t instruction) {
  int64_t simm19;
  int64_t simm26;
  uint8_t register_index;
  uint8_t cond;

  if (CHECK_BITS(instruction, UNCON_BRANCH_MASK, UNCON_BRANCH_VALUE)) {
    simm26 = SELECT_BITS(instruction, BRANCH_SIMM26_OFFSET, BRANCH_SIMM26_SIZE);
    simm26 = SIGN_EXT(simm26, 26, 64);
    simm26 = simm26 * 4;
    cpu_state->PC.X += simm26;
  }
  else if (CHECK_BITS(instruction, REGISTER_BRANCH_MASK, REGISTER_BRANCH_VALUE)) {
    register_index = SELECT_BITS(instruction, BRANCH_REG_OFFSET, BRANCH_REG_SIZE);
    cpu_state->PC.X = cpu_state->R[register_index].X;
  }
  else if (CHECK_BITS(instruction, CON_BRANCH_MASK, CON_BRANCH_VALUE)) {
    simm19 = SELECT_BITS(instruction, BRANCH_SIMM19_OFFSET, BRANCH_SIMM19_SIZE);
    simm19 = SIGN_EXT(simm19, 19, 64);
    simm19 = simm19 * WORD_SIZE_BYTES;
    cond = SELECT_BITS(instruction, 0, 4);
    conditional_branch(cpu_state, simm19, cond);
  }
  else {
    fprintf(stderr, "Illegal state: invalid branch instruction\n");
    fprintf(stderr, "Exiting!\n");
    exit(EXIT_FAILURE);
  }
}

void conditional_branch(state_t *cpu_state, int64_t offset, uint8_t cond) {
  bool jump = false;
  switch(cond) {
    case COND_EQ:
      jump = cpu_state->PSTATE.Z == 1;
      break;
    case COND_NE:
      jump = cpu_state->PSTATE.Z == 0;
      break;
    case COND_GE:
      jump = cpu_state->PSTATE.N != 1;
      break;
    case COND_LT:
      jump = cpu_state->PSTATE.N == 1;
      break;
    case COND_GT:
      jump = cpu_state->PSTATE.Z == 0 && cpu_state->PSTATE.N == cpu_state->PSTATE.V;
      break;
    case COND_LE:
      jump = !(cpu_state->PSTATE.Z == 0 && cpu_state->PSTATE.N == cpu_state->PSTATE.V);
      break;
    case COND_AL:
      jump = true;
      break;
  }
  if (jump) {
      cpu_state->PC.X += offset;
  }
  else {
      cpu_state->PC.X += WORD_SIZE_BYTES;
  }
}