#define DPREG_OFFSET 25
#define DPREG_SIZE 3

#define REG_SF_OFFSET 31
#define REG_SF_SIZE 1

#define REG_OPC_OFFSET 29
#define REG_OPC_SIZE 2

#define REG_M_OFFSET 28
#define REG_M_SIZE 1

#define REG_OPR_OFFSET 21
#define REG_OPR_SIZE 4

#define REG_RM_OFFSET 16
#define REG_RM_SIZE 5

#define REG_OPERAND_OFFSET 10
#define REG_OPERAND_SIZE 6

#define REG_X_OFFSET 15
#define REG_X_SIZE 1

#define REG_RA_OFFSET 10
#define REG_RA_SIZE 5

#define REG_RN_OFFSET 5
#define REG_RN_SIZE 5

#define REG_RD_OFFSET 0
#define REG_RD_SIZE 5

#define ARITHMETIC_M 0
#define ARITHMETIC_MASK 0x9
#define ARITHMETIC_VALUE 0x8

#define BIT_LOGIC_M 0
#define BIT_LOGIC_MASK 0x8
#define BIT_LOGIC_VALUE 0x0

#define MULTIPLY_M 1
#define MULTIPLY_MASK 0xF
#define MULTIPLY_VALUE 0x8

#define SHIFT_OFFSET 1
#define SHIFT_SIZE 2
#define N_OFFSET 0
#define N_SIZE 1

#define LSL_VALUE 0x0
#define LSR_VALUE 0x1
#define ASR_VALUE 0x2
#define ROR_VALUE 0x3

#define AND_OPC 0x0
#define ORR_OPC 0x1
#define EON_OPC 0x2
#define ANDS_OPC 0x3

#define MADD_X 0
#define MSUB_X 1

#define ADD_OPC 0x0
#define ADDS_OPC 0x1
#define SUB_OPC 0x2
#define SUBS_OPC 0x3

void and (state_t *cpu_state, uint8_t dest, uint8_t src1, uint64_t op2, uint8_t sf);
void bic(state_t *cpu_state, uint8_t dest, uint8_t src1, uint64_t op2, uint8_t sf);
void orr(state_t *cpu_state, uint8_t dest, uint8_t src1, uint64_t op2, uint8_t sf);
void orn(state_t *cpu_state, uint8_t dest, uint8_t src1, uint64_t op2, uint8_t sf);
void eon(state_t *cpu_state, uint8_t dest, uint8_t src1, uint64_t op2, uint8_t sf);
void eor(state_t *cpu_state, uint8_t dest, uint8_t src1, uint64_t op2, uint8_t sf);
void ands(state_t *cpu_state, uint8_t dest, uint8_t src1, uint64_t op2, uint8_t sf);
void bics(state_t *cpu_state, uint8_t dest, uint8_t src1, uint64_t op2, uint8_t sf);
void madd(state_t *cpu_state, uint8_t dest, uint8_t src, uint8_t rn, uint8_t rm, uint8_t sf);
void msub(state_t *cpu_state, uint8_t dest, uint8_t src, uint8_t rn, uint8_t rm, uint8_t sf);

uint64_t lsl(state_t *cpu_state, uint8_t operand_reg, uint8_t shift_amount, uint8_t sf);
uint64_t lsr(state_t *cpu_state, uint8_t operand_reg, uint8_t shift_amount, uint8_t sf);
uint64_t asr(state_t *cpu_state, uint8_t operand_reg, uint8_t shift_amount, uint8_t sf);
uint64_t ror(state_t *cpu_state, uint8_t operand_reg, uint8_t shift_amount, uint8_t sf);

void execute_arithmetic_instruction(state_t *cpu_state, uint32_t instruction);
void execute_bit_logic_instruction(state_t *cpu_state, uint32_t instruction);
void execute_multiply_instruction(state_t *cpu_state, uint32_t instruction);
void execute_dpreg_instruction(state_t *cpu_state, uint32_t instruction);
