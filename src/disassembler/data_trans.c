#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "disassemble.h"
#include "data_trans.h"

void sdt_uimm(FILE *fp, uint64_t imm12, uint8_t xn, uint8_t rt, uint8_t l, uint8_t sf) {
    assert(l == LOAD_L || l == STORE_L);
    assert(sf == SF_32 || sf == SF_64);
    char rt_type = (sf == SF_32) ? 'w' : 'x';

    if (l == LOAD_L) {
        if (imm12 == 0) {
            fprintf(fp, "ldr %c%d, [x%d]\n", rt_type, rt, xn);
            return;
        }
        fprintf(fp, "ldr %c%d, [x%d, #%lu]\n", rt_type, rt, xn, imm12);
    }
    else if (l == STORE_L) {
        if (imm12 == 0) {
            fprintf(fp, "str %c%d, [x%d]\n", rt_type, rt, xn);
            return;
        }
        fprintf(fp, "str %c%d, [x%d, #%lu]\n", rt_type, rt, xn, imm12);
    }
}

void sdt_regoffset(FILE *fp, uint8_t xm, uint8_t xn, uint8_t rt, uint8_t l, uint8_t sf) {
    assert(l == LOAD_L || l == STORE_L);
    assert(sf == SF_32 || sf == SF_64);
    char rt_type = (sf == SF_32) ? 'w' : 'x';
    if (l == LOAD_L) {
        fprintf(fp, "ldr %c%d, [x%d, x%d]\n", rt_type, rt, xn, xm);
    }
    else if (l == STORE_L) {
        fprintf(fp, "str %c%d, [x%d, x%d]\n", rt_type, rt, xn, xm);
    }
}

void sdt_preind(FILE *fp, int64_t simm9, uint8_t xn, uint8_t rt, uint8_t l, uint8_t sf) {
    assert(l == LOAD_L || l == STORE_L);
    assert(sf == SF_32 || sf == SF_64);
    char rt_type = (sf == SF_32) ? 'w' : 'x';

    if (l == LOAD_L) {
        fprintf(fp, "ldr %c%d, [x%d, #%ld]!\n", rt_type, rt, xn, simm9);
    }
    else if (l == STORE_L) {
        fprintf(fp, "str %c%d, [x%d, #%ld]!\n", rt_type, rt, xn, simm9);
    }
}

void sdt_postind(FILE *fp, int64_t simm9, uint8_t xn, uint8_t rt, uint8_t l, uint8_t sf) {
    assert(l == LOAD_L || l == STORE_L);
    assert(sf == SF_32 || sf == SF_64);
    char rt_type = (sf == SF_32) ? 'w' : 'x';

    if (l == LOAD_L) {
        fprintf(fp, "ldr %c%d, [x%d], #%ld\n", rt_type, rt, xn, simm9);
    }
    else if (l == STORE_L) {
        fprintf(fp, "str %c%d, [x%d], #%ld\n", rt_type, rt, xn, simm9);
    }
}

void execute_load_literal(FILE *fp, reg_t *PC, uint32_t instruction, uint8_t sf) {
    assert(sf == SF_32 || sf == SF_64);
    char rt_type = (sf == SF_32) ? 'w' : 'x';

    uint8_t rt = SELECT_BITS(instruction, DT_RT_OFFSET, DT_RT_SIZE);
    int64_t simm19 = SELECT_BITS(instruction, DT_SIMM19_OFFSET, DT_SIMM19_SIZE);
    simm19 = SIGN_EXT(simm19, 19, 64) * 4;
    fprintf(fp, "ldr %c%d, #%ld\n", rt_type, rt, PC->X + simm19);
}

void execute_sdt(FILE *fp,  uint32_t instruction, uint8_t sf) {
    uint8_t rt = SELECT_BITS(instruction, DT_RT_OFFSET, DT_RT_SIZE);
    uint8_t xn = SELECT_BITS(instruction, SDT_XN_OFFSET, SDT_XN_SIZE);
    uint8_t l = SELECT_BITS(instruction, SDT_L_OFFSET, SDT_L_SIZE);
    uint8_t u = SELECT_BITS(instruction, SDT_U_OFFSET, SDT_U_SIZE);
    uint16_t offset = SELECT_BITS(instruction, SDT_OFFSET_OFFSET, SDT_OFFSET_SIZE);

    if (u == UNSIGNED_OFFSET_U) {
        uint64_t imm12 = offset * 8;
        sdt_uimm(fp, imm12, xn, rt, l, sf);
    }
    else if (CHECK_BITS(offset, REG_OFFSET_MASK, REG_OFFSET_VALUE)) {
        uint8_t xm = SELECT_BITS(instruction, REG_OFFSET_XM_OFFSET, REG_OFFSET_XM_SIZE);
        sdt_regoffset(fp, xm, xn, rt, l, sf);
    }
    else if (CHECK_BITS(offset, PRE_POST_MASK, PRE_POST_VALUE)) {
        int64_t simm9 = SELECT_BITS(instruction, PRE_POST_SIMM9_OFFSET, PRE_POST_SIMM9_SIZE);
        simm9 = SIGN_EXT(simm9, 9, 64);
        uint8_t i = SELECT_BITS(instruction, PRE_POST_I_OFFSET, PRE_POST_I_SIZE);
        if (i == PRE_INDEX_I) {
            sdt_preind(fp, simm9, xn, rt, l, sf);
        }
        else if (i == POST_INDEX_I) {
            sdt_postind(fp, simm9, xn, rt, l, sf);
        }
    }
    else {
        fprintf(stderr, "Illegal state: invalid single data transfer instruction\n");
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }
}
