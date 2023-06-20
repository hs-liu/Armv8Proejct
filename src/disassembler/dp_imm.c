#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "disassemble.h"
#include "dp_imm.h"


void handle_imm_arithmetic(FILE* fp, char* op_string, uint8_t dest, uint8_t src1, uint64_t imm, bool sh, uint8_t sf) {
    assert(sf == SF_32 || sf == SF_64);
    if (sf == SF_32) {
        fprintf(fp, "%s w%d, w%d, 0x%lx, lsl #%d\n", op_string, dest, src1, imm, sh ? 12 : 0);
    } else {
        fprintf(fp, "%s x%d, x%d, 0x%lx, lsl #%d\n", op_string, dest, src1, imm, sh ? 12 : 0);
    }
}

void handle_mov(FILE* fp, char* op_string, uint8_t rd, uint64_t imm, char *shift_string, uint8_t hw,uint8_t sf) {
    assert(sf == SF_32 || sf == SF_64);
    if (strcmp(op_string,"movz") == 0 && hw == 0) {
        if (sf == SF_32) {
            fprintf(fp, "mov w%d, #0x%lx\n", rd, imm);
            return;
        } else {
            fprintf(fp, "mov x%d, #0x%lx\n", rd, imm);
            return;
        }
    }
    if (sf == SF_32) {
        fprintf(fp, "%s w%d, 0x%lx, %s #%d\n", op_string, rd, imm, shift_string, 16 * hw);
    } else {
        fprintf(fp, "%s x%d, 0x%lx, %s #%d\n", op_string, rd, imm, shift_string, 16 * hw);
    }

}

void disassemble_dpimm_instruction(FILE* fp, uint32_t instruction) {
    uint8_t sf = SELECT_BITS(instruction, IMM_SF_OFFSET, IMM_SF_SIZE);
    uint8_t opc = SELECT_BITS(instruction, IMM_OPC_OFFSET, IMM_OPC_SIZE);
    uint8_t opi = SELECT_BITS(instruction, IMM_OPI_OFFSET, IMM_OPI_SIZE);
    uint8_t rd = SELECT_BITS(instruction, IMM_RD_OFFSET, IMM_RD_SIZE);

    assert(SELECT_BITS(instruction, IMM_OFFSET, IMM_SIZE) == IMM_VALUE);
    assert(sf == SF_32 || sf == SF_64);

    if (opi == IMM_ARITHMETIC_OPI) {
        uint8_t sh = SELECT_BITS(instruction, IMM_SH_OFFSET, IMM_SH_SIZE);
        uint32_t imm12 = SELECT_BITS(instruction, IMM_IMM12_OFFSET, IMM_IMM12_SIZE);
        uint8_t rn = SELECT_BITS(instruction, IMM_RN_OFFSET, IMM_RN_SIZE);
        char op_str[5];
        switch (opc) {
            case ADD_OPC:
                strcpy(op_str, "add");
                break;
            case ADDS_OPC:
                strcpy(op_str, "adds");
                break;
            case SUB_OPC:
                strcpy(op_str, "sub");
                break;
            case SUBS_OPC:
                strcpy(op_str, "subs");
                break;
        }
        handle_imm_arithmetic(fp, op_str, rd, rn, imm12, sh, sf);
    }

    if (opi == IMM_WIDE_MOVE_OPI) {
        uint8_t hw = SELECT_BITS(instruction, IMM_HW_OFFSET, IMM_HW_SIZE);
        uint64_t imm16 = SELECT_BITS(instruction, IMM_IMM16_OFFSET, IMM_IMM16_SIZE);
        // uint64_t op = imm16 << (16 * hw);

        char op_str[5];
        switch (opc) {
            case MOVN_OPC:
            strcpy(op_str, "movn");
                break;
            case MOVZ_OPC:
            strcpy(op_str, "movz");
                break;
            case MOVK_OPC:
            strcpy(op_str, "movk");
                break;
        }
        handle_mov(fp, op_str, rd, imm16, "lsl", hw, sf);
    }
}
