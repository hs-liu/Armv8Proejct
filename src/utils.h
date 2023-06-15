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



#define SELECT_BITS(value, offset, size) (((value) >> (offset)) & ((1ll << (size)) - 1))
#define CHECK_BITS(value, mask, target_value) (((value) & (mask)) == (target_value)) 
#define SIGN_EXT(value, in_size, out_size) ((value) << ((out_size) - (in_size)) >> ((out_size) - (in_size)))

#define SET_BITS(target, offset, size, value) ((target) |= (((value) & ((1ll << (size)) - 1)) << (offset)))


char *strip_line(char *line, int *res_len);
bool is_symbol(char *symbol, int len);
bool is_symbol_declaration(char *line, int len);
void build_symbol_table(char *line, void *data);
bool is_data_processing_opcode(char *opcode);
bool is_branch_opcode(char *opcode);
bool is_load_store_opcode(char *opcode);
bool is_special_instruction(char *opcode);
uint8_t get_register(char *reg_str, uint8_t *sf);
