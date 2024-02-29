#include "table.h"
#include "memory.h"
#include "object.h"
#include <stdbool.h>

#define TABLE_MAX_LOAD 0.75

void init_table(Table *table) {
    table->count = 0;
    table->capacity = 0;
    table->entries = NULL;
}

void free_table(Table *table) {
    FREE_ARRAY(Entry, table->entries, table->capacity);
    init_table(table);
}

static Entry *find_entry(Entry *entries, int capacity, Obj *key) {
    uint32_t index = get_hash(key) & (capacity - 1);
    Entry *tombstone = NULL;

    for (;;) {
        Entry *entry = &entries[index];

        if (entry->key == NULL) {
            if (IS_NIL(entry->value)) {
                return tombstone != NULL ? tombstone : entry;
            } else {
                if (tombstone == NULL)
                    tombstone = entry;
            }
        } else if (entry->key == key) {
            return entry;
        }

        index = (index + 1) & (capacity - 1);
    }
}

static void adjust_capacity(Table *table, int capacity) {
    Entry *entries = ALLOCATE(Entry, capacity);
    for (int i = 0; i < capacity; i++) {
        entries[i].key = NULL;
        entries[i].value = AS_OBJ(new_nil());
    }

    table->count = 0;
    for (int i = 0; i < table->capacity; i++) {
        Entry *entry = &table->entries[i];
        if (entry->key == NULL)
            continue;

        Entry *dest = find_entry(entries, capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
        table->count++;
    }

    FREE_ARRAY(Entry, table->entries, table->capacity);
    table->entries = entries;
    table->capacity = capacity;
}

bool table_get(Table *table, Obj *key, Obj **value) {
    if (table->count == 0)
        return false;

    Entry *entry = find_entry(table->entries, table->capacity, key);
    if (entry->key == NULL)
        return false;

    *value = entry->value;
    return true;
}

bool table_set(Table *table, Obj *key, Obj *value) {
    if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
        int capacity = GROW_CAPACITY(table->capacity);
        adjust_capacity(table, capacity);
    }

    Entry *entry = find_entry(table->entries, table->capacity, key);
    bool is_new_key = entry->key == NULL;

    if (is_new_key && IS_NIL(entry->value))
        table->count++;

    entry->key = key;
    entry->value = value;
    return is_new_key;
}

bool table_delete(Table *table, Obj *key) {
    if (table->count == 0)
        return false;

    Entry *entry = find_entry(table->entries, table->capacity, key);
    if (entry->key == NULL)
        return false;

    entry->key = NULL;
    entry->value = AS_OBJ(new_bool(true));
    return true;
}

void table_add_all(Table *from, Table *to) {
    for (int i = 0; i < from->capacity; i++) {
        Entry *entry = &from->entries[i];
        if (entry->key != NULL) {
            table_set(to, entry->key, entry->value);
        }
    }
}

Obj *table_find_string(Table *table, uint32_t hash) {
    if (table->count == 0)
        return NULL;

    uint32_t index = hash & (table->capacity - 1);
    for (;;) {
        Entry *entry = &table->entries[index];
        if (entry->key == NULL) {
            if (IS_NIL(entry->value))
                return NULL;
        } else if (get_hash(entry->key) == hash) {
            return entry->key;
        }

        index = (index + 1) & (table->capacity - 1);
    }
}

void table_remove_white(Table *table) {
    for (int i = 0; i < table->capacity; i++) {
        Entry *entry = &table->entries[i];
        if (entry->key != NULL && !entry->key->is_marked) {
            table_delete(table, entry->key);
        }
    }
}

void mark_table(Table *table) {
    for (int i = 0; i < table->capacity; i++) {
        Entry *entry = &table->entries[i];
        mark_object(entry->key);
        mark_object(entry->value);
    }
}
