#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MEMORY_CAPACITY (2 * 1024 * 1024)
#define WORD_SIZE_BYTES 4

#define NOP_INSTRUCTION 0xD503201F
#define HALT_INSTRUCTION 0x8A000000

#define UNCON_BRANCH 0x00000005
#define REGISTER_BRANCH 0x00000035
#define CON_BRANCH 0x00000015

#define EQ 0x0
#define NE 0x1
#define GE 0xA
#define LT 0xB
#define GT 0xC
#define LE 0xD
#define AL 0xE

#define OP0_OFFSET 25
#define OP0_SIZE 4

#define SELECT_BITS(value, offset, size) ((value >> offset) & ((1 << size) - 1))
#define CHECK_BITS(value, mask, target_value) ((value & mask) == target_value) 

#define OP0_DPIMM_MASK 0xE
#define OP0_DPIMM_VALUE 0x8

#define OP0_DPREG_MASK 0x7
#define OP0_DPREG_VALUE 0x5

#define OP0_LS_MASK 0x5
#define OP0_LS_VALUE 0x4

#define OP0_BRANCH_MASK 0xE
#define OP0_BRANCH_VALUE 0xA


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


void setup(state_t *cpu_state) {
  memset(cpu_state, 0, sizeof(state_t));
  memset(main_memory, 0, sizeof main_memory);
}

void print_usage(void) {
  fprintf(stderr, "Usage: ./emulate input_file\n");
  exit(EXIT_FAILURE);
}

void load_bin_to_memory(char *file_name) {
  FILE *fp = fopen(file_name, "rb");
  if (fp == NULL) {
    fprintf(stderr, "Couldn't open the file %s\n", file_name);
    fprintf(stderr, "Exiting!\n");
    exit(EXIT_FAILURE);
  }

  int fseek_res = fseek(fp, 0L, SEEK_END);
  long file_size;

  if (fseek_res != 0 || (file_size = ftell(fp)) == -1) {
    fprintf(stderr, "Couldn't check the size of the file %s\n", file_name);
    fprintf(stderr, "Exiting!\n");
    exit(EXIT_FAILURE);
  }

  fseek_res = fseek(fp, 0L, SEEK_SET);
  if (fseek_res != 0) {
    fprintf(stderr, "Encountered error while checking size of %s\n", file_name);
    fprintf(stderr, "Exiting!\n");
    exit(EXIT_FAILURE);
  }

  if (file_size > MEMORY_CAPACITY) {
    fprintf(stdout, "Warning: file size exceeds memory capacity\n");
    fprintf(stdout, "Truncating file within memory\n");
  }

  fread(main_memory, sizeof(char), MEMORY_CAPACITY, fp);
}

uint32_t fetch_word(uint64_t address) {
  if (address >= MEMORY_CAPACITY) {
    fprintf(stderr, "Illegal state: attempted to fetch beyond memory bounds\n");
    fprintf(stderr, "Exiting!\n");
    exit(EXIT_FAILURE);
  }

  return *(uint32_t *)&main_memory[address];
}

void emulate_cycle(state_t *cpu_state) {
  // Fetch
  uint32_t instruction = fetch_word(cpu_state->PC.X);

  // Decode and execute
  uint8_t op0 = SELECT_BITS(instruction, OP0_OFFSET, OP0_SIZE);

  cpu_state->PC.X += WORD_SIZE_BYTES; // TODO: Remove this
  printf("0x%08lx: %08x\n", cpu_state->PC.X, instruction);

  if (instruction == NOP_INSTRUCTION) {
    return;
  }

  if (instruction == HALT_INSTRUCTION) {
    exit(EXIT_SUCCESS);
  }

  if (CHECK_BITS(op0, OP0_DPIMM_MASK, OP0_DPIMM_VALUE)) {
    // TODO: implement data processing (immediate) instructions
    printf(": implement data processing (immediate) instructions\n");

  }

  if (CHECK_BITS(op0, OP0_DPREG_MASK, OP0_DPREG_VALUE)) {
    // TODO: implement data processing (register) instructions
    printf(": implement data processing (register) instructions\n");

  }

  if (CHECK_BITS(op0, OP0_LS_MASK, OP0_LS_VALUE)) {
    // TODO: implement load and store instructions
    printf(": implement load and store instructions\n");
  }

  if (CHECK_BITS(op0, OP0_BRANCH_MASK, OP0_BRANCH_VALUE)) {
    // TODO: implement branch instructions
    branch_instruction(cpu_state, instruction);
    printf(": implement branch instructions\n");
  }

}

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
  bool jump;
  switch(cond) {
    case EQ:
      jump = cpu_state->PSTATE.Z == 1;
    case NE:
      jump = cpu_state->PSTATE.Z == 0;
    case GE:
      jump = cpu_state->PSTATE.N == 1;
    case LT:
      jump = cpu_state->PSTATE.N != 1;
    case GT:
      jump = cpu_state->PSTATE.Z == 0 && cpu_state->PSTATE.N == cpu_state->PSTATE.V;
    case LE:
      jump = !(cpu_state->PSTATE.Z == 0 && cpu_state->PSTATE.N == cpu_state->PSTATE.V);
    case AL:
    default:
      cpu_state->PC.X += offset;
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    print_usage();
  }
  state_t cpu_state;
  setup(&cpu_state);
  load_bin_to_memory(argv[1]);

  
  while (true) {
    emulate_cycle(&cpu_state);
  }

  return EXIT_SUCCESS;
}
