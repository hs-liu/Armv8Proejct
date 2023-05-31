#include <stdlib.h>
#DEFINE MEMORY_CAPACITY (2 * 1024 * 1024)

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

void ldrLiteral32(state_t *state, int rt, int val) {
  state->[rt].W = val;
  state->[rt].X &= 0x00000000FFFFFFFF;
}

void ldrLiteral64(state_t *state, int rt, int val) {
  
}

int main(int argc, char **argv) {
  return EXIT_SUCCESS;
}
