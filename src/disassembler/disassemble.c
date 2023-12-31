#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "disassemble.h"
#include "branch.h"
#include "dp_reg.h"
#include "dp_imm.h"
#include "data_trans.h"

uint8_t main_memory[MEMORY_CAPACITY];


/**
 * Prints the usage of the program to stderr
 */
void print_usage(void) {
    fprintf(stderr, "Usage: ./emulate <bin_file> [<out_file>]\n");
}

/**
 * Loads the binary file into memory
 * 
 * @param file_name the name of the binary file
 */
int load_bin_to_memory(char *file_name) {
    FILE *fp = fopen(file_name, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Couldn't open the file %s\n", file_name);
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }

    int fseek_res = fseek(fp, 0L, SEEK_END);
    int file_size;

    if (fseek_res != 0 || (file_size = (int) ftell(fp)) == -1) {
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
    return file_size;
}

/**
 * Fetches a word from memory
 * 
 * @param address the address to fetch from
 * @param sf the size flag
 * @return the word fetched from memory
 */
uint64_t fetch_word(uint64_t address, uint8_t sf) {
    assert(sf == SF_32 || sf == SF_64);
    if (address >= MEMORY_CAPACITY) {
        fprintf(stderr, "Illegal state: attempted to fetch beyond memory bounds\n");
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }

    if (sf == SF_32) {
        return *(uint32_t *)&main_memory[address];
    }
    else {
        return *(uint64_t *)&main_memory[address];
    }
}

/**
 * Writes a word to memory
 * 
 * @param address the address to write to
 * @param word the word to write
 * @param sf the size flag
 */
void write_word(uint64_t address, uint64_t word, uint8_t sf) {
    if (address >= MEMORY_CAPACITY) {
        fprintf(stderr, "Illegal state: attempted to write beyond memory bounds\n");
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }

    if (sf == SF_32) {
        *(uint32_t *)&main_memory[address] = word;
    }
    else {
        *(uint64_t *)&main_memory[address] = word;
    }
}

/**
 * Disassembles a word into a single instruction 
 * 
 * @param PC the program counter
 * @param fp the file to write to
 */
void disassemble_cycle(reg_t *PC, FILE* fp) {
    // Fetch
    uint32_t instruction = fetch_word(PC->X, SF_32);

    // Decode and execute
    uint8_t op0 = SELECT_BITS(instruction, OP0_OFFSET, OP0_SIZE);

    // fprintf(fp, "0x%lx: ", PC->X);
    if (instruction == NOP_INSTRUCTION) {
        fprintf(fp, "nop\n");
    }
    else if (instruction == HALT_INSTRUCTION) {
        fprintf(fp, "and x0, x0, x0\n");
    }
    else if (CHECK_BITS(op0, OP0_DPIMM_MASK, OP0_DPIMM_VALUE)) {
        disassemble_dpimm_instruction(fp, instruction);
    }
    else if (CHECK_BITS(op0, OP0_DPREG_MASK, OP0_DPREG_VALUE)) {
        disassemble_dpreg_instruction(fp, instruction);
    }
    else if (CHECK_BITS(op0, OP0_LS_MASK, OP0_LS_VALUE)) {
        uint8_t sf = SELECT_BITS(instruction, DT_SF_OFFSET, DT_SF_SIZE);

        uint16_t opcode = SELECT_BITS(instruction, DT_OPCODE_OFFSET, DT_OPCODE_SIZE);
        if (CHECK_BITS(opcode, SDT_MASK, SDT_VALUE)) {
            execute_sdt(fp, instruction, sf);
        }
        if (CHECK_BITS(opcode, LOADLIT_MASK, LOADLIT_VALUE)) {
            execute_load_literal(fp, PC, instruction, sf);
        }
    }
    else if (CHECK_BITS(op0, OP0_BRANCH_MASK, OP0_BRANCH_VALUE)) {
        branch_instruction(fp, PC, instruction);
    }
    else {
        fprintf(fp, ".int 0x%x\n", instruction);
    }
    PC->X += WORD_SIZE_BYTES;
}

int main(int argc, char **argv) {
    FILE *output_fp;
    if (argc == 2) {
        output_fp = stdout;
    }
    else if (argc == 3) {
        char *file_name = argv[2];
        output_fp = fopen(file_name, "w");

        if (output_fp == NULL) {
            fprintf(stderr, "Error writing to output file %s\n", file_name);
            return EXIT_FAILURE;
        }
    }
    else {
        print_usage();
        return EXIT_FAILURE;
    }

    reg_t PC;
    PC.X = 0;
    int file_size = load_bin_to_memory(argv[1]);

    while (PC.X < file_size) {
        disassemble_cycle(&PC, output_fp);
    };

    return EXIT_SUCCESS;
}
