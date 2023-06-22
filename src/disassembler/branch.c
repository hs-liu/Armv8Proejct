#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "disassemble.h"
#include "branch.h"

/**
 * Prints the usage of the program to stderr
 * 
 * @param fp the file to print to
 * @param PC the program counter
 * @param instruction the instruction to disassemble
 */
void branch_instruction(FILE *fp, reg_t *PC, uint32_t instruction) {
    int64_t simm19;
    int64_t simm26;
    uint8_t register_index;
    uint8_t cond;

    if (CHECK_BITS(instruction, UNCON_BRANCH_MASK, UNCON_BRANCH_VALUE)) {
        simm26 = SELECT_BITS(instruction, BRANCH_SIMM26_OFFSET, BRANCH_SIMM26_SIZE);
        simm26 = SIGN_EXT(simm26, 26, 64);
        simm26 = simm26 * 4;
        fprintf(fp, "b 0x%lx\n",PC->X + simm26);

    }
    else if (CHECK_BITS(instruction, REGISTER_BRANCH_MASK, REGISTER_BRANCH_VALUE)) {
        register_index = SELECT_BITS(instruction, BRANCH_REG_OFFSET, BRANCH_REG_SIZE);
        fprintf(fp, "br x%d\n", register_index);
    }
    else if (CHECK_BITS(instruction, CON_BRANCH_MASK, CON_BRANCH_VALUE)) {
        simm19 = SELECT_BITS(instruction, BRANCH_SIMM19_OFFSET, BRANCH_SIMM19_SIZE);
        simm19 = SIGN_EXT(simm19, 19, 64);
        simm19 = simm19 * WORD_SIZE_BYTES;
        cond = SELECT_BITS(instruction, 0, 4);
        char cond_str[3];
        switch(cond) {
            case COND_EQ:
                strcpy(cond_str, "eq");
                break;
            case COND_NE:
                strcpy(cond_str, "ne");
                break;
            case COND_GE:
                strcpy(cond_str, "ge");
                break;
            case COND_LT:
                strcpy(cond_str, "lt");
                break;
            case COND_GT:
                strcpy(cond_str, "gt");
                break;
            case COND_LE:  
                strcpy(cond_str, "le");
                break;
            case COND_AL:
                strcpy(cond_str, "al");
                break;
            default:
                fprintf(stderr, "Illegal state: invalid condition code\n");
                fprintf(stderr, "Exiting!\n");
                exit(EXIT_FAILURE);
        }
        fprintf(fp, "b.%s 0x%lx\n", cond_str, (PC->X + simm19));
    }
    else {
        fprintf(stderr, "Illegal state: invalid branch instruction\n");
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }
}
