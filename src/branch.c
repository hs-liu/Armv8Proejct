#include <stdint.h>
#include <stdbool.h>

#include "emulate.h"
#include "branch.h"

void branch_instruction(state_t *cpu_state, uint32_t instruction) {
  uint32_t opcode = instruction >> OP0_OFFSET;
  if (opcode == UNCON_BRANCH) {
    int64_t simm26 = (instruction & 0x3FFFFFF) * 4;
    simm26 = simm26 << 38 >> 38;
    cpu_state->PC.X += simm26;
  } else if (opcode == REGISTER_BRANCH) {
    int register_index = (((1 << 5) - 1) & (instruction >> 5));
    cpu_state->PC.X = cpu_state->R[register_index].X;
  } else if (opcode == CON_BRANCH) {
    int64_t simm19 = ((instruction & 0xFFFFE0) >> 5) * 4;
    simm19 = simm19 << 45 >> 45;
    uint8_t cond = instruction & 0xF;
    conditional_branch(cpu_state, simm19, cond);
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