#include <stdint.h>
#include <stdbool.h>

#include "emulate.h"
#include "branch.h"

void branch_instruction(state_t *cpu_state, uint32_t instruction) {
  uint32_t opcode = instruction >> OP0_OFFSET;
  int64_t simm19;
  int64_t simm26;
  uint8_t register_index;
  uint8_t cond;

  switch (opcode)
  {
    case UNCON_BRANCH:
      simm26 = SELECT_BITS(instruction, 0, 26) * 4;
      simm26 = SIGN_EXT(simm26, 26, 64);
      cpu_state->PC.X += simm26;
      break;
    case REGISTER_BRANCH:
      register_index = SELECT_BITS(instruction, 5, 5);
      cpu_state->PC.X = cpu_state->R[register_index].X;
      break;
    case CON_BRANCH:
      simm19 = SELECT_BITS(instruction, 5, 19) * 4;
      simm19 = SIGN_EXT(simm19, 19, 64);
      cond = SELECT_BITS(instruction, 0, 4);
      conditional_branch(cpu_state, simm19, cond);
      break;
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
      jump = cpu_state->PSTATE.N == 1;
      break;
    case COND_LT:
      jump = cpu_state->PSTATE.N != 1;
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
}