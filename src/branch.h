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

void branch_instruction(state_t *cpu_state, uint32_t instruction);
void conditional_branch(state_t *cpu_state, int64_t offset, uint8_t cond);