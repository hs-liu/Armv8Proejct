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

uint8_t main_memory[MEMORY_CAPACITY];

void lsl(state_t *state, int shift_amount) {
  
}

int main(int argc, char **argv) {
  return EXIT_SUCCESS;
}
