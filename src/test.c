#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "symbol_table.h"

void test_hashmap(void);

void test_hashmap(void) {
    // Create new symbol table
    hashmap_t *symbol_table = hashmap_create();

    assert(symbol_table->size == 0);

    // Add symbols to symbol table
    hashmap_set(symbol_table, "foo", 0x1234);
    hashmap_set(symbol_table, "bar", 0x5678);
    hashmap_set(symbol_table, "baz", 0x9abc);

    assert(symbol_table->size == 3);
    assert(hashmap_find_entry(symbol_table, "foo")->value == 0x1234);
    assert(hashmap_find_entry(symbol_table, "bar")->value == 0x5678);
    assert(hashmap_find_entry(symbol_table, "baz")->value == 0x9abc);


    // Update symbols in symbol table
    hashmap_set(symbol_table, "foo", 0x4321);
    hashmap_set(symbol_table, "bar", 0x8765);
    hashmap_set(symbol_table, "baz", 0xcba9);

    assert(symbol_table->size == 3);
    
    assert(hashmap_find_entry(symbol_table, "foo")->value == 0x4321);
    assert(hashmap_find_entry(symbol_table, "bar")->value == 0x8765);
    assert(hashmap_find_entry(symbol_table, "baz")->value == 0xcba9);

    // Free symbol table
    hashmap_free(symbol_table);
}

int main(void) {
    test_hashmap();
    printf("All tests passed!\n");
    return EXIT_SUCCESS;
}