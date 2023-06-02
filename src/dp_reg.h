#define SF_32 0
#define SF_64 1

#define DPREG_OFFSET 25
#define DPREG_SIZE 3

#define SF_OFFSET 31
#define SF_SIZE 1

#define OPC_OFFSET 29
#define OPC_SIZE 2

#define M_OFFSET 28
#define M_SIZE 1

#define OPR_OFFSET 21
#define OPR_SIZE 4

#define RM_OFFSET 16
#define RM_SIZE 5

#define OPERAND_OFFSET 10
#define OPERAND_SIZE 6

#define RN_OFFSET 5
#define RN_SIZE 5

#define RD_OFFSET 0
#define RD_SIZE 5

#define ARITHMETIC_M 0
#define ARITHMETIC_MASK 0x9
#define ARITHMETIC_VALUE 0x8

#define BIT_LOGIC_M 0
#define BIT_LOGIC_MASK 0x8
#define BIT_LOGIC_VALUE 0x8

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

//TODO: function prototypes