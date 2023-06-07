#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "emulate.h"
#include "branch.h"
#include "dp_reg.h"
#include "dp_imm.h"
#include "data_trans.h"

void setup(state_t *cpu_state) {
    memset(cpu_state, 0, sizeof(state_t));
    memset(main_memory, 0, sizeof main_memory);
    cpu_state->PSTATE.Z = 1;
}

void print_usage(void) {
    fprintf(stderr, "Usage: ./emulate <bin_file> [<out_file>]\n");
}


void set_NV_flags_32(state_t *state, uint32_t result) {
    state->PSTATE.N = result >> 31;
    state->PSTATE.Z = result == 0;
}

void set_NV_flags_64(state_t *state, uint64_t result) {
    state->PSTATE.N = result >> 63;
    state->PSTATE.Z = result == 0;
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

    return *(uint32_t *) &main_memory[address];
}

void write_word(uint64_t address, uint32_t word) {
    if (address >= MEMORY_CAPACITY) {
        fprintf(stderr, "Illegal state: attempted to write beyond memory bounds\n");
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }

    *(uint32_t *) &main_memory[address] = word;
}

uint64_t fetch_word_64(uint64_t address) {
    if (address >= MEMORY_CAPACITY) {
        fprintf(stderr, "Illegal state: attempted to fetch beyond memory bounds\n");
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }

    return *(uint64_t *) &main_memory[address];
}

void write_word_64(uint64_t address, uint64_t word) {
    if (address >= MEMORY_CAPACITY) {
        fprintf(stderr, "Illegal state: attempted to write beyond memory bounds\n");
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }

    *(uint64_t *) &main_memory[address] = word;
}

void output_result(state_t *cpu_state, FILE *fp) {
    fprintf(fp, "Registers:\n");
    for (int i = 0; i < 31; i++) {
        fprintf(fp, "X%02d    = %016lx\n", i, cpu_state->R[i].X);
    }
    fprintf(fp, "PC     = %016lx\n", cpu_state->PC.X);
    char n_flag = cpu_state->PSTATE.N ? 'N' : '-';
    char z_flag = cpu_state->PSTATE.Z ? 'Z' : '-';
    char c_flag = cpu_state->PSTATE.C ? 'C' : '-';
    char v_flag = cpu_state->PSTATE.V ? 'V' : '-';
    fprintf(fp, "PSTATE : %c%c%c%c\n", n_flag, z_flag, c_flag, v_flag);

    fprintf(fp, "Non-Zero Memory:\n");
    for (uint64_t addr = 0; addr < MEMORY_CAPACITY; addr += WORD_SIZE_BYTES) {
        uint32_t word = fetch_word(addr);
        if (word) {
            fprintf(fp, "0x%08lx : %08x\n", addr, word);
        }
    }
}

/**
 * Return value is true iff the program is to continue running
*/
bool emulate_cycle(state_t *cpu_state) {
    // Fetch
    uint32_t instruction = fetch_word(cpu_state->PC.X);

    // Decode and execute
    uint8_t op0 = SELECT_BITS(instruction, OP0_OFFSET, OP0_SIZE);

    if (instruction == NOP_INSTRUCTION) {
        cpu_state->PC.X += WORD_SIZE_BYTES;
        return true;
    }

    if (instruction == HALT_INSTRUCTION) {
        return false;
    }

    if (CHECK_BITS(op0, OP0_DPIMM_MASK, OP0_DPIMM_VALUE)) {
        uint8_t sf = SELECT_BITS(instruction, IMM_SF_OFFSET, IMM_SF_SIZE);
        if (sf == SF_32) {
            execute_dpimm_instruction_32(cpu_state, instruction);
        } else if (sf == SF_64) {
            execute_dpimm_instruction_64(cpu_state, instruction);
        } else {
            fprintf(stderr, "Illegal state: invalid sf value\n");
            fprintf(stderr, "Exiting!\n");
            exit(EXIT_FAILURE);
        }
        cpu_state->PC.X += WORD_SIZE_BYTES;
    }

    if (CHECK_BITS(op0, OP0_DPREG_MASK, OP0_DPREG_VALUE)) {
        uint8_t sf = SELECT_BITS(instruction, IMM_SF_OFFSET, IMM_SF_SIZE);
        if (sf == SF_32) {
            execute_dpreg_instruction_32(cpu_state, instruction);
        } else if (sf == SF_64) {
            execute_dpreg_instruction_64(cpu_state, instruction);
        } else {
            fprintf(stderr, "Illegal state: invalid sf value\n");
            fprintf(stderr, "Exiting!\n");
            exit(EXIT_FAILURE);
        }
        cpu_state->PC.X += WORD_SIZE_BYTES;
    }

    if (CHECK_BITS(op0, OP0_LS_MASK, OP0_LS_VALUE)) {
        uint8_t sf = SELECT_BITS(instruction, DT_SF_OFFSET, DT_SF_SIZE);

        uint16_t opcode = SELECT_BITS(instruction, DT_OPCODE_OFFSET, DT_OPCODE_SIZE);
        if (CHECK_BITS(opcode, SDT_MASK, SDT_VALUE)) {
            if (sf == SF_32) {
                execute_sdt_32(cpu_state, instruction);
            } else {
                execute_sdt_64(cpu_state, instruction);
            }
        }
        if (CHECK_BITS(opcode, LOADLIT_MASK, LOADLIT_VALUE)) {
            if (sf == SF_32) {
                execute_load_literal_32(cpu_state, instruction);
            } else {
                execute_load_literal_64(cpu_state, instruction);
            }

        }
        cpu_state->PC.X += WORD_SIZE_BYTES;
    }

    if (CHECK_BITS(op0, OP0_BRANCH_MASK, OP0_BRANCH_VALUE)) {
        branch_instruction(cpu_state, instruction);
    }

    return true;
}

int main(int argc, char **argv) {
    FILE *output_fp;
    if (argc == 2) {
        output_fp = stdout;
    } else if (argc == 3) {
        char *file_name = argv[2];
        output_fp = fopen(file_name, "w");

        if (output_fp == NULL) {
            fprintf(stderr, "Error writing to output file %s\n", file_name);
            return EXIT_FAILURE;
        }
    } else {
        print_usage();
        return EXIT_FAILURE;
    }

    state_t cpu_state;
    setup(&cpu_state);
    load_bin_to_memory(argv[1]);

    bool continue_running = true;
    do {
        continue_running = emulate_cycle(&cpu_state);
    } while (continue_running);

    output_result(&cpu_state, output_fp);

    return EXIT_SUCCESS;
}
