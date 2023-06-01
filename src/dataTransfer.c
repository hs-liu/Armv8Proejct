#include <stdlib.h>
#define MEMORY_CAPACITY (2 * 1024 * 1024)

typedef struct {
  bool N;
  bool Z;
  bool C;
  bool V;
} pstate_t;

typedef union {
  uint64_t X;
  uint32_t W;
} register_t;

typedef struct {
  register_t R[31];
  register_t ZR;
  register_t PC;
  register_t SP;
  pstate_t PSTATE;
} state_t;

//do this in decode:
/* typedef struct {
  int sf; // size of load: 0 target reg 32-bit 1 64-bit
  int L;  // type of data transfer: 1 LDR 0 STR
  int U;  // unsigned offset flag: set => addr mode unsigned offset
  int offset;
  
} singleDataTransfer

typedef struct {
  int sf; // size of load: 0 target reg 32-bit 1 64-bit
  int 
} loadLiteral
*/

uint8_t main_memory[MEMORY_CAPACITY];

void ldrLiteral32(state_t *state, int rt, int simm19) {
  //load into registers
  state->[rt].W = main_memory[state->PC + simm19];
  state->[rt].X &= 0x00000000FFFFFFFF;

}

void ldrLiteral64(state_t *state, int rt, int simm19) {
  state->[rt].X = main_memory[state->PC + simm19];
}

void ldr32(state_t *state, int rt, uint8_t addr) {
  //xn 64 X-reg
  //byte at addr loaded into lowest 8-bits of Rt
  state->[rt].W = main_memory[addr + 3] << 24 + main_memory[addr + 2] << 16 + 
      main_memory[addr + 1] << 8 + main_memory[addr];
}

void ldr64(state_t *state, int rt, uint8_t addr) {
  main_memory[addr + 7] = main_memory[addr];
  state->[rt].W = main_memory[addr] & 0x00000000000000FF;
}

void str32(state_t *state, int rt, uint8_t addr) {
  // load 4-byte from rt to addr in mem
  main_memory[addr] = state->R[rt].W;
}

//Just pass in address based on decode result to ldr32 or ldr64 instead

/*
void ldrUnsignedOffset64(state_t *state, int rt, int xn, int imm12) {
  main_memory[xn + imm12 + 7] = main_memory[xn + imm12]
  //byte at addr loaded into lowest 8-bits of Rt
  state->[rt].X = main_memory[xn + imm12] & 0x00000000000000FF
}

void strUnsignedOffset32(state_t *state, int rt, int xn, int imm12) {
  // xn is a 64 X-reg, addr = xn + imm12, 4-byte word from addr to addr+3
  main_memory[xn + imm12 + 3] = main_memory[xn + imm12]
  //byte at addr loaded into lowest 8-bits of Rt
  state->[rt].W = main_memory[xn + imm12] & 0x00000000000000FF
}
*/


int main(int argc, char **argv) {
  ldr32()
  return EXIT_SUCCESS;
}
