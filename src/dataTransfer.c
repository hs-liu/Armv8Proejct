#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
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
} reg_t;

typedef struct {
  reg_t R[31];
  reg_t ZR;
  reg_t PC;
  reg_t SP;
  pstate_t PSTATE;
} state_t;

//do this in decode:
 typedef struct {
  int sf; // size of load: 0 target reg 32-bit 1 64-bit
  int L;  // type of data transfer: 1 LDR 0 STR
  int U;  // unsigned offset flag: set => addr mode unsigned offset
  int offset;
  
} instruction

uint8_t main_memory[MEMORY_CAPACITY];

void ldrLiteral32(state_t *state, int rt, int32_t simm19) {
  //load into registers
  state->R[rt].W = main_memory[state->PC.W + ((simm19 << 13) >> 13)];
  state->R[rt].X &= 0x00000000FFFFFFFF;

}

void ldrLiteral64(state_t *state, int rt, int64_t simm19) {
  state->R[rt].X = main_memory[state->PC.X + ((simm19 << 45) >> 45)];
}

void ldr32(state_t *state, int rt, uint8_t addr) {
  //xn 64 X-reg
  //byte at addr loaded into lowest 8-bits of Rt
  state->R[rt].W = (main_memory[addr + 3] << 24) + (main_memory[addr + 2] << 16) + 
      (main_memory[addr + 1] << 8) + main_memory[addr];
}

void ldr64(state_t *state, int rt, uint8_t addr) {
  main_memory[addr + 7] = main_memory[addr];
  state->R[rt].W = main_memory[addr] & 0x00000000000000FF;
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
  state_t DUM_STATE = {
    .R = {
        [0].X = 0xFFFF00000000FFFF,
        [1].X = 0xFFFF00000000FFFF,
        [2].X = 0xFFFF00000000FFFF,
        [3].X = 0xFFFF00000000FFFF,
        [4].X = 0x0FFFF0000000FFFF,
        [5].X = 0xFFFF00000000FFFF,
        [6].X = 0xFFFF00000000FFFF,
        [7].X = 0xFFFF00000000FFFF,
    },
    .ZR = {0},
    .PC = {0},
    .SP = {0},
    .PSTATE = {0}
  };
  printf("before %lx\n", DUM_STATE.R[0].X);
  ldr32(&DUM_STATE, 0, 3);
  printf("after load %lx\n", DUM_STATE.R[0].X);
  return EXIT_SUCCESS;
}
