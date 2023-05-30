#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
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

void lsl(state_t *state, int shift_amount) {
  
}

int main(int argc, char **argv) {
  return EXIT_SUCCESS;
}
