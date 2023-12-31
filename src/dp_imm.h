#define IMM_SF_OFFSET 31
#define IMM_SF_SIZE 1

#define IMM_OPC_OFFSET 29
#define IMM_OPC_SIZE 2

#define IMM_OFFSET 26
#define IMM_SIZE 3

#define IMM_OPI_OFFSET 23
#define IMM_OPI_SIZE 3
#define IMM_VALUE 0x4

#define IMM_OPERAND_OFFSET 5
#define IMM_OPERAND_SIZE 18

#define IMM_RD_OFFSET 0
#define IMM_RD_SIZE 5

#define SF_32 0
#define SF_64 1

#define IMM_ARITHMETIC_OPI 0x2
#define IMM_WIDE_MOVE_OPI 0x5

#define IMM_SH_OFFSET 22
#define IMM_SH_SIZE 1

#define IMM_IMM12_OFFSET 10
#define IMM_IMM12_SIZE 12

#define IMM_RN_OFFSET 5
#define IMM_RN_SIZE 5

#define ADD_OPC 0x0
#define ADDS_OPC 0x1
#define SUB_OPC 0x2
#define SUBS_OPC 0x3


#define IMM_HW_OFFSET 21
#define IMM_HW_SIZE 2

#define IMM_IMM16_OFFSET 5
#define IMM_IMM16_SIZE 16

#define MOVN_OPC 0x0
#define MOVZ_OPC 0x2
#define MOVK_OPC 0x3

void add_imm(state_t *cpu_state, uint8_t dest, uint8_t src1, uint64_t imm, uint8_t sf);
void adds_imm(state_t *cpu_state, uint8_t dest, uint8_t src1, uint64_t imm, uint8_t sf);
void sub_imm(state_t *cpu_state, uint8_t dest, uint8_t src1, uint64_t imm, uint8_t sf);
void subs_imm(state_t *cpu_state, uint8_t dest, uint8_t src1, uint64_t imm, uint8_t sf);
void movz_imm(state_t *cpu_state, uint8_t dest, uint64_t imm, uint8_t sf);
void movk_imm(state_t *cpu_state, uint8_t dest, uint8_t hw, uint64_t imm, uint8_t sf);
void execute_dpimm_instruction(state_t *cpu_state, uint32_t instruction);
