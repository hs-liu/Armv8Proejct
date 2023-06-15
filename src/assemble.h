
#define MAX_LINE_LENGTH 1024

uint8_t get_register(char *reg_str, uint8_t *sf);

typedef void (*process_line_fn)(char *line, void *data);

typedef struct {
  uint64_t address;
  hashmap_t *symbol_table;
  uint8_t *memory;
} assembler_state_t;

