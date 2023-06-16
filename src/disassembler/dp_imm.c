#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "disassemble.h"
#include "dp_imm.h"

void add_imm(FILE* fp, uint8_t rd, uint8_t rn, uint64_t imm, bool sh, uint8_t sf) {
    // 32 bit: "add w{dest} w{src1} #imm" into a output file
    // 64 bit: "add x{dest} x{src1} #imm" into a output file
    assert(sf == SF_32 || sf == SF_64);
    if (sf == SF_32) {
        fprintf(fp, "add w%d, w%d, 0x%llx, lsl #%d\n", rd, rn, imm, sh ? 12 : 0);
    } else {
        fprintf(fp, "add x%d, x%d, 0x%llx, lsl #%d\n", rd, rn, imm, sh ? 12 : 0);
    }
}



void adds_imm(FILE* fp, uint8_t dest, uint8_t src1, uint64_t imm, bool sh, uint8_t sf) {
    assert(sf == SF_32 || sf == SF_64);
    if (sf == SF_32) {
        fprintf(fp, "adds w%d, w%d, %llx, lsl #%d\n", dest, src1, imm, sh ? 12 : 0);
    } else {
        fprintf(fp, "adds x%d, x%d, %llx, lsl #%d\n", dest, src1, imm, sh ? 12 : 0);
    }
}


void sub_imm(FILE* fp, uint8_t dest, uint8_t src1, uint64_t imm, bool sh, uint8_t sf) {
    // 32 bit: "sub w{dest} w{src1} #imm" into a output file
    // 64 bit: "sub x{dest} x{src1} #imm" into a output file
    assert(sf == SF_32 || sf == SF_64);
    if (sf == SF_32) {
        fprintf(fp, "sub w%d, w%d, %llx, lsl #%d\n", dest, src1, imm, sh ? 12 : 0);
    } else {
        fprintf(fp, "sub x%d, x%d, %llx, lsl #%d\n", dest, src1, imm, sh ? 12 : 0);
    }
}


void subs_imm(FILE* fp, uint8_t dest, uint8_t src1, uint64_t imm, bool sh, uint8_t sf) {
    assert (sf == SF_32 || sf == SF_64);
    if (sf == SF_32) {
        fprintf(fp, "subs w%d, w%d, %llx, lsl #%d\n", dest, src1, imm, sh ? 12 : 0);
    } else {
        fprintf(fp, "subs x%d, x%d, %llx, lsl #%d\n", dest, src1, imm, sh ? 12 : 0);
    }
}

void movn_imm(FILE* fp, uint8_t dest, uint64_t imm, uint8_t sf, uint8_t hw) {
    assert(sf == SF_32 || sf == SF_64);
}

void movz_imm(FILE* fp, uint8_t dest, uint64_t imm, uint8_t sf) {
    assert(sf == SF_32 || sf == SF_64);
}

void movk_imm(FILE* fp, uint8_t dest, uint8_t hw, uint64_t imm, uint8_t sf) {
    assert(sf == SF_32 || sf == SF_64);
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
        switch (opc) {
            case ADD_OPC:
                add_imm(fp, rd, rn, imm12, sh, sf);
                break;
            case ADDS_OPC:
                adds_imm(fp, rd, rn, imm12,sh, sf);
                break;
            case SUB_OPC:
                sub_imm(fp, rd, rn, imm12, sh,sf);
                break;
            case SUBS_OPC:
                subs_imm(fp, rd, rn, imm12, sh, sf);
                break;
        }
    }

    if (opi == IMM_WIDE_MOVE_OPI) {
        uint8_t hw = SELECT_BITS(instruction, IMM_HW_OFFSET, IMM_HW_SIZE);
        uint64_t imm16 = SELECT_BITS(instruction, IMM_IMM16_OFFSET, IMM_IMM16_SIZE);
        uint64_t op = imm16 << (16 * hw);

        switch (opc) {
            case MOVN_OPC:
                movn_imm(fp, rd, op, sf, hw);
                break;
            case MOVZ_OPC:
                movz_imm(fp, rd, op, sf);
                break;
            case MOVK_OPC:
                movk_imm(fp, rd, hw, imm16, sf);
                break;
        }
    }
}
