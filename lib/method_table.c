#include "method_table.h"
#include "memory.h"
#include "object.h"

void init_method_table(NativeTable *table, int methods) {
    table->count = 0;
    table->capacity = methods;
    table->entries = calloc(methods, sizeof(NativeEntry));
}

void free_method_table(NativeTable *table) { free(table->entries); }

static NativeEntry *find_method_entry(NativeEntry *entries, int capacity, uint32_t hash) {
    uint32_t index = hash & (capacity - 1);

    for (;;) {
        NativeEntry *entry = &entries[index];

        if (entry->key == NULL || entry->hash == hash) {
            return entry;
        }

        index = (index + 1) & (capacity - 1);
    }
}

bool method_table_get(NativeTable *table, uint32_t hash, NativeFn *value) {
    if (table->count == 0)
        return false;

    NativeEntry *entry = find_method_entry(table->entries, table->capacity, hash);

    if (entry->key != NULL) {
        *value = entry->value;
        return true;
    }

    return false;
}

bool method_table_set(NativeTable *table, char *key, uint32_t hash, NativeFn fn) {
    NativeEntry *entry = find_method_entry(table->entries, table->capacity, hash);
    bool is_new_key = entry->key == NULL;

    if (is_new_key && entry->value == NULL)
        table->count++;

    entry->key = key;
    entry->value = fn;
    entry->hash = hash;

    return is_new_key;
}

bool method_table_delete(NativeTable *table, uint32_t hash) {
    if (table->count == 0)
        return false;

    NativeEntry *entry = find_method_entry(table->entries, table->capacity, hash);
    if (entry->key == NULL)
        return false;

    entry->key = NULL;
    entry->value = NULL;
    entry->hash = 0;

    return true;
}

void method_table_add_all(NativeTable *from, NativeTable *to) {
    for (int i = 0; i < from->capacity; i++) {
        NativeEntry *entry = &from->entries[i];
        if (entry->key != NULL) {
            method_table_set(to, entry->key, entry->hash, entry->value);
        }
    }
}
