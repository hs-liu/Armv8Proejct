#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "emulate.h"
#include "branch.h"

#define SELECT_BITS(value, offset, size) ((value >> offset) & ((1 << size) - 1))
#define CHECK_BITS(value, mask, target_value) ((value & mask) == target_value) 

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
