#include "method_table.h"
#include "memory.h"
#include "object.h"

void init_method_table(BuiltinMethodTable *table, int methods) {
    table->count = 0;
    table->capacity = methods;
    table->entries = calloc(methods, sizeof(BuiltinMethodEntry));
}

void free_method_table(BuiltinMethodTable *table) { free(table->entries); }

static BuiltinMethodEntry *
find_method_entry(BuiltinMethodEntry *entries, int capacity, uint32_t hash) {
    uint32_t index = hash & (capacity - 1);

    for (;;) {
        BuiltinMethodEntry *entry = &entries[index];

        if (entry->key == NULL || entry->hash == hash) {
            return entry;
        }

        index = (index + 1) & (capacity - 1);
    }
}

bool method_table_get(
    BuiltinMethodTable *table,
    uint32_t hash,
    BuiltinMethodFn *value) {
    if (table->count == 0)
        return false;

    BuiltinMethodEntry *entry =
        find_method_entry(table->entries, table->capacity, hash);

    if (entry->key != NULL) {
        *value = entry->value;
        return true;
    }

    return false;
}

bool method_table_set(
    BuiltinMethodTable *table,
    char *key,
    uint32_t hash,
    BuiltinMethodFn fn) {
    BuiltinMethodEntry *entry =
        find_method_entry(table->entries, table->capacity, hash);
    bool is_new_key = entry->key == NULL;

    if (is_new_key && entry->value == NULL)
        table->count++;

    entry->key = key;
    entry->value = fn;
    entry->hash = hash;

    return is_new_key;
}

bool method_table_delete(BuiltinMethodTable *table, uint32_t hash) {
    if (table->count == 0)
        return false;

    BuiltinMethodEntry *entry =
        find_method_entry(table->entries, table->capacity, hash);
    if (entry->key == NULL)
        return false;

    entry->key = NULL;
    entry->value = NULL;
    entry->hash = 0;

    return true;
}

void method_table_add_all(BuiltinMethodTable *from, BuiltinMethodTable *to) {
    for (int i = 0; i < from->capacity; i++) {
        BuiltinMethodEntry *entry = &from->entries[i];
        if (entry->key != NULL) {
            method_table_set(to, entry->key, entry->hash, entry->value);
        }
    }
}
