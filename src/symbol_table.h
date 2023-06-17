

#define HASHMAP_SIZE 1024

struct hashmap_entry {
    char *key;
    uint64_t value;
    struct hashmap_entry *next;
};
typedef struct hashmap_entry hashmap_entry_t;

typedef struct {
    int size;
    hashmap_entry_t **entries;
} hashmap_t;


hashmap_t *hashmap_create(void);
hashmap_entry_t *hashmap_find_entry(hashmap_t *map, char *key);
void hashmap_set(hashmap_t *map, char *key, uint64_t value);
void hashmap_free(hashmap_t *map);
