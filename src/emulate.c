#include <stdio.h>
#include <string.h>
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

void setup() {
  printf("%lu\n", sizeof main_memory);
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
    fprintf(stderr, "Encountered an error while checking size of %s\n", file_name);
    fprintf(stderr, "Exiting!\n");
    exit(EXIT_FAILURE);
  }

  if (file_size > MEMORY_CAPACITY) {
    fprintf(stdout, "Warning: file size exceeds memory capacity of processor\n");
    fprintf(stdout, "Truncating file within memory\n");
  }

  fread(main_memory, sizeof(char), MEMORY_CAPACITY, fp);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    print_usage();
  }
  setup();
  load_bin_to_memory(argv[1]);


  return EXIT_SUCCESS;
}
