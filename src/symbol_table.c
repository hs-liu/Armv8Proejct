#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "symbol_table.h"

static int hash(char *key, int len) {
    int hash = 0;
    for (int i = 0; i < len; i++) {
        hash = (hash * 31 + key[i]) % HASHMAP_SIZE;
    }
    return hash;
}

hashmap_t *hashmap_create(void) {
    hashmap_t *map = malloc(sizeof(hashmap_t));
    map->size = 0;
    map->entries = calloc(HASHMAP_SIZE, sizeof(hashmap_entry_t));
    return map;
}

hashmap_entry_t *hashmap_find_entry(hashmap_t *map, char *key) {
    int keylen = strlen(key);
    int index = hash(key, keylen);
    hashmap_entry_t *entry = map->entries[index];
    while (entry != NULL) {
        if (strncmp(entry->key, key, keylen) == 0) {
            return entry;
        }
        entry = entry->next;
    }
    return NULL;
}

void hashmap_set(hashmap_t *map, char *key, uint64_t value) {
    hashmap_entry_t *entry = hashmap_find_entry(map, key);
    if (entry != NULL) {
        entry->value = value;
        return;
    }
    int index = hash(key, strlen(key));
    hashmap_entry_t *new_entry = malloc(sizeof(hashmap_entry_t));
    char *new_key = strdup(key);
    if (new_key == NULL) {
        fprintf(stderr, "Could not allocate memory in heap!\n");
        fprintf(stderr, "Exiting!\n");
        exit(EXIT_FAILURE);
    }
    new_entry->key = new_key;
    new_entry->value = value;
    new_entry->next = NULL;

    if (entry == NULL) {
        map->entries[index] = new_entry;
    } else {
        while (entry->next != NULL) {
            entry = entry->next;
        }
        entry->next = new_entry;
    }
    map->size++;
}

void hashmap_free(hashmap_t *map) {
    for (int i = 0; i < HASHMAP_SIZE; i++) {
        hashmap_entry_t *entry = map->entries[i];
        while (entry != NULL) {
            hashmap_entry_t *next = entry->next;
            free(entry->key);
            free(entry);
            entry = next;
        }
    }
    free(map->entries);
    free(map);
}

