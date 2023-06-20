#define MEMORY_CAPACITY (2 * 1024 * 1024)
#define WORD_SIZE_BYTES 4

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

#define SELECT_BITS(value, offset, size) (((value) >> (offset)) & ((1ll << (size)) - 1))
#define CHECK_BITS(value, mask, target_value) (((value) & (mask)) == (target_value)) 
#define SIGN_EXT(value, in_size, out_size) ((value) << ((out_size) - (in_size)) >> ((out_size) - (in_size)))

typedef struct {
  bool N;
  bool Z;
  bool C;
  bool V;
} pstate_t;

typedef union {
  uint32_t W;
  uint64_t X;
} reg_t;

typedef struct {
  reg_t R[31];
  reg_t PC;
  reg_t SP;
  pstate_t PSTATE;
} state_t;

void setup(state_t *cpu_state);
void print_usage(void);
void set_NZ_flags(state_t *cpu_state, uint64_t result, uint8_t sf);
void load_bin_to_memory(char *file_name);
uint64_t fetch_word(uint64_t address, uint8_t sf);
void write_word(uint64_t address, uint64_t word, uint8_t sf);
bool disassemble_cycle(reg_t *PC, FILE* fp);
