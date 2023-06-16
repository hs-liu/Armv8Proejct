#define MEMORY_CAPACITY (2 * 1024 * 1024)
#define WORD_SIZE_BYTES 4

#define MB_SIZE 1024 * 1024

#define ENCODING_OFFSET 0
#define ENCODING_SIZE 32

#define NOP_INSTRUCTION 0xD503201F
#define HALT_INSTRUCTION 0x8A000000

#define SF_32 0
#define SF_64 1

#define ZR_REG 0x1f

#define OP0_OFFSET 25
#define OP0_SIZE 4

#define OP0_DPIMM_MASK 0xE
#define OP0_DPIMM_VALUE 0x8

#define OP0_DPREG_MASK 0x7
#define OP0_DPREG_VALUE 0x5

#define OP0_LS_MASK 0x5
#define OP0_LS_VALUE 0x4

#define OP0_BRANCH_MASK 0xE
#define OP0_BRANCH_VALUE 0xA

// Data Transfer Constants
#define DT_SF_OFFSET 30
#define DT_SF_SIZE 1

#define DT_OPCODE_OFFSET 23
#define DT_OPCODE_SIZE 9

#define SDT_MASK 0x17D
#define SDT_VALUE 0x170

#define LOADLIT_MASK 0x17E
#define LOADLIT_VALUE 0x30

#define LOAD_LIT_ENCODING 0x18000000

#define SDT_L_OFFSET 22
#define SDT_L_SIZE 1

#define SDT_XN_OFFSET 5
#define SDT_XN_SIZE 5

#define SDT_U_OFFSET 24
#define SDT_U_SIZE 1

#define SDT_UNSIGNED_OFFSET_U 1

#define SDT_OFFSET_OFFSET 10
#define SDT_OFFSET_SIZE 12

#define UNSIGNED_OFFSET_U 1

#define LOAD_L 1
#define STORE_L 0

#define SDT_UNSIGNED_OFFSET_ENCODING 0xb8000000

#define SDT_IMM12_OFFSET 10
#define SDT_IMM12_SIZE 12

#define MAX_UIMM12 ((1 << 12) - 1)

#define REG_OFFSET_ENCODING 0xb8206800

#define REG_OFFSET_MASK 0x83F
#define REG_OFFSET_VALUE 0x81A

#define PRE_POST_MASK 0x801
#define PRE_POST_VALUE 0x1

#define REG_OFFSET_XM_OFFSET 16
#define REG_OFFSET_XM_SIZE 5

#define PRE_POST_SIMM9_OFFSET 12
#define PRE_POST_SIMM9_SIZE 9

#define MIN_SIMM9 -256
#define MAX_SIMM9 255

#define PRE_POST_ENCODING 0xb8000400

#define PRE_POST_I_OFFSET 11
#define PRE_POST_I_SIZE 1

#define PRE_INDEX_I 1
#define POST_INDEX_I 0

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

// Branch Constants
#define UNCON_BRANCH_MASK 0xFC000000
#define UNCON_BRANCH_VALUE 0x14000000

#define REGISTER_BRANCH_MASK 0xFFFFFC1F
#define REGISTER_BRANCH_VALUE 0xD61F0000

#define CON_BRANCH_MASK 0xFF000010
#define CON_BRANCH_VALUE 0x54000000

#define BRANCH_SIMM26_OFFSET 0
#define BRANCH_SIMM26_SIZE 26

#define BRANCH_REG_OFFSET 5
#define BRANCH_REG_SIZE 5

#define BRANCH_SIMM19_OFFSET 5
#define BRANCH_SIMM19_SIZE 19

#define BRANCH_COND_OFFSET 0
#define BRANCH_COND_SIZE 4

#define UNCON_BRANCH 0x00000005
#define REGISTER_BRANCH 0x00000035
#define CON_BRANCH 0x00000015

#define COND_EQ 0x0
#define COND_NE 0x1
#define COND_GE 0xA
#define COND_LT 0xB
#define COND_GT 0xC
#define COND_LE 0xD
#define COND_AL 0xE

// Data Processing (Register) Constants
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

#define MULTIPLY_ENCODING 0x1b000000

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




#define SELECT_BITS(value, offset, size) (((value) >> (offset)) & ((1ll << (size)) - 1))
#define CHECK_BITS(value, mask, target_value) (((value) & (mask)) == (target_value)) 
#define SIGN_EXT(value, in_size, out_size) ((value) << ((out_size) - (in_size)) >> ((out_size) - (in_size)))

#define SET_BITS(target, offset, size, value) ((target) |= (((value) & ((1ll << (size)) - 1)) << (offset)))


char *strip_line(char *line, int *res_len);
bool is_symbol(char *symbol, int len);
bool is_symbol_declaration(char *line, int len);
void build_symbol_table(char *line, void *data);
bool is_data_processing_multiply_opcode(char *opcode);
bool is_data_processing_two_op_opcode(char *opcode);
bool is_data_processing_single_op_opcode(char *opcode);
bool is_data_processing_two_op_no_dest_opcode(char *opcode);
bool is_data_processing_opcode(char *opcode);
bool is_branch_opcode(char *opcode);
bool is_load_store_opcode(char *opcode);
bool is_special_instruction(char *opcode);
uint8_t get_register(char *reg_str, uint8_t *sf);
