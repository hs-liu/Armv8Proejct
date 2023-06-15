#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "symbol_table.h"
#include "assemble.h"
#include "assemble_dt.h"
#include "utils.h"

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

void test_get_register() {
    uint8_t sf = 0;
    assert(get_register("x0", &sf) == 0);
    assert(sf == SF_64);
    assert(get_register("x1", &sf) == 1);
    assert(sf == SF_64);
    assert(get_register("w2", &sf) == 2);
    assert(sf == SF_32);
    assert(get_register("w3", &sf) == 3);
    assert(sf == SF_32);
    assert(get_register("w30", &sf) == 30);
    assert(sf == SF_32);
    assert(get_register("xzr", &sf) == ZR_REG);
    assert(sf == SF_64);
    assert(get_register("wzr", &sf) == ZR_REG);
    assert(sf == SF_32);
}

int main(void) {
    test_hashmap();
    test_get_register();
    printf("All tests passed!\n");
    return EXIT_SUCCESS;
}