#define DT_SF_OFFSET 30
#define DT_SF_SIZE 1

#define DT_OPCODE_OFFSET 23
#define DT_OPCODE_SIZE 9

#define SDT_MASK 0x17D
#define SDT_VALUE 0x170

#define LOADLIT_MASK 0x17E
#define LOADLIT_VALUE 0x30

#define SDT_L_OFFSET 22
#define SDT_L_SIZE 1

#define SDT_XN_OFFSET 5
#define SDT_XN_SIZE 5

#define SDT_U_OFFSET 24
#define SDT_U_SIZE 1

#define SDT_OFFSET_OFFSET 10
#define SDT_OFFSET_SIZE 12

#define UNSIGNED_OFFSET_U 1

#define LOAD_L 1
#define STORE_L 0

#define SDT_IMM12_OFFSET 10
#define SDT_IMM12_SIZE 12

#define REG_OFFSET_MASK 0x83F
#define REG_OFFSET_VALUE 0x81A

#define PRE_POST_MASK 0x801
#define PRE_POST_VALUE 0x1

#define REG_OFFSET_XM_OFFSET 16
#define REG_OFFSET_XM_SIZE 5

#define PRE_POST_SIMM9_OFFSET 12
#define PRE_POST_SIMM9_SIZE 9

#define PRE_POST_I_OFFSET 11
#define PRE_POST_I_SIZE 1

#define PRE_INDEX_I 1
#define POST_INDEX_I 0

//IMM12 same as offset
#define OFFSET_OFFSET 10
#define OFFSET_SIZE 12

#define XN_OFFSET 5
#define XN_SIZE 5

#define XM_OFFSET 16
#define XM_SIZE 5

#define SIMM9_OFFSET 12
#define SIMM9_SIZE 9

#define I_OFFSET 11
#define I_SIZE 1

#define DT_SIMM19_OFFSET 5
#define DT_SIMM19_SIZE 19

#define DT_RT_OFFSET 0
#define DT_RT_SIZE 5

#define BYTES_IN_32 4
#define BYTES_IN_64 8

#define LDR_LITR_OFFSET 31
#define LDR_LITR_SIZE 1

#define REG_OFF_OFFSET 21
#define REG_OFF_SIZE 1

#define L_IS_LDR 0
#define U_IS_UOFF 1
#define IS_REG_OFF 1
#define I_IS_PRE 1

void execute_load_literal_32(state_t *state, uint32_t instruction);
void execute_load_literal_64(state_t *state, uint32_t instruction);
void execute_load_literal(state_t *, uint32_t, uint8_t);

void execute_load_store_32(state_t *cpu_state, uint32_t instruction);
void execute_load_store_64(state_t *cpu_state, uint32_t instruction);

void execute_sdt(state_t *, uint32_t, uint8_t);