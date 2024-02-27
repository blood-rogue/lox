#include "method_table.h"
#include "memory.h"
#include "object.h"

void init_method_table(BuiltinTable *table, int methods) {
    table->count = 0;
    table->capacity = methods;
    table->entries = calloc(methods, sizeof(BuiltinEntry));
}

void free_method_table(BuiltinTable *table) { free(table->entries); }

static BuiltinEntry *
find_method_entry(BuiltinEntry *entries, int capacity, uint32_t hash) {
    uint32_t index = hash & (capacity - 1);

    for (;;) {
        BuiltinEntry *entry = &entries[index];

        if (entry->key == NULL || entry->hash == hash) {
            return entry;
        }

        index = (index + 1) & (capacity - 1);
    }
}

bool method_table_get(BuiltinTable *table, uint32_t hash, BuiltinFn *value) {
    if (table->count == 0)
        return false;

    BuiltinEntry *entry =
        find_method_entry(table->entries, table->capacity, hash);

    if (entry->key != NULL) {
        *value = entry->value;
        return true;
    }

    return false;
}

bool method_table_set(
    BuiltinTable *table,
    char *key,
    uint32_t hash,
    BuiltinFn fn) {
    BuiltinEntry *entry =
        find_method_entry(table->entries, table->capacity, hash);
    bool is_new_key = entry->key == NULL;

    if (is_new_key && entry->value == NULL)
        table->count++;

    entry->key = key;
    entry->value = fn;
    entry->hash = hash;

    return is_new_key;
}

bool method_table_delete(BuiltinTable *table, uint32_t hash) {
    if (table->count == 0)
        return false;

    BuiltinEntry *entry =
        find_method_entry(table->entries, table->capacity, hash);
    if (entry->key == NULL)
        return false;

    entry->key = NULL;
    entry->value = NULL;
    entry->hash = 0;

    return true;
}

void method_table_add_all(BuiltinTable *from, BuiltinTable *to) {
    for (int i = 0; i < from->capacity; i++) {
        BuiltinEntry *entry = &from->entries[i];
        if (entry->key != NULL) {
            method_table_set(to, entry->key, entry->hash, entry->value);
        }
    }
}
