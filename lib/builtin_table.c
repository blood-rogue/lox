#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "builtins.h"

#define TABLE_MAX_LOAD 0.75

void init_builtin_table(BuiltinTable *table)
{
    table->count = 0;
    table->capacity = 0;
    table->entries = NULL;
}

void free_builtin_table(BuiltinTable *table)
{
    FREE_ARRAY(BuiltinEntry, table->entries, table->capacity);
    init_builtin_table(table);
}

static BuiltinEntry *find_builtin_entry(BuiltinEntry *entries, int capacity, uint32_t hash)
{
    uint32_t index = hash & (capacity - 1);
    BuiltinEntry *tombstone = NULL;

    for (;;)
    {
        BuiltinEntry *entry = &entries[index];

        if (entry->key == NULL)
        {
            if (entry->value == NULL)
            {
                return tombstone != NULL ? tombstone : entry;
            }
            else
            {
                if (tombstone == NULL)
                    tombstone = entry;
            }
        }
        else if (entry->hash == hash)
        {
            return entry;
        }

        index = (index + 1) & (capacity - 1);
    }
}

static void adjust_builtin_capacity(BuiltinTable *table, int capacity)
{
    BuiltinEntry *entries = ALLOCATE(BuiltinEntry, capacity);
    for (int i = 0; i < capacity; i++)
    {
        entries[i].key = NULL;
        entries[i].value = NULL;
    }

    table->count = 0;
    for (int i = 0; i < table->capacity; i++)
    {
        BuiltinEntry *entry = &table->entries[i];
        if (entry->key == NULL)
            continue;

        BuiltinEntry *dest = find_builtin_entry(entries, capacity, entry->hash);
        dest->key = entry->key;
        dest->value = entry->value;
        table->count++;
    }

    FREE_ARRAY(BuiltinEntry, table->entries, table->capacity);
    table->entries = entries;
    table->capacity = capacity;
}

bool builtin_table_get(BuiltinTable *table, uint32_t hash, BuiltinFn *value)
{
    if (table->count == 0)
        return false;

    BuiltinEntry *entry = find_builtin_entry(table->entries, table->capacity, hash);
    if (entry->key == NULL)
        return false;

    *value = entry->value;
    return true;
}

bool builtin_table_set(BuiltinTable *table, char *key, uint32_t hash, BuiltinFn fn)
{
    if (table->count + 1 > table->capacity * TABLE_MAX_LOAD)
    {
        int capacity = GROW_CAPACITY(table->capacity);
        adjust_builtin_capacity(table, capacity);
    }

    BuiltinEntry *entry = find_builtin_entry(table->entries, table->capacity, hash);
    bool is_new_key = entry->key == NULL;

    if (is_new_key && entry->value == NULL)
        table->count++;

    entry->key = key;
    entry->value = fn;
    entry->hash = hash;

    return is_new_key;
}

bool builtin_table_delete(BuiltinTable *table, uint32_t hash)
{
    if (table->count == 0)
        return false;

    BuiltinEntry *entry = find_builtin_entry(table->entries, table->capacity, hash);
    if (entry->key == NULL)
        return false;

    entry->key = NULL;
    entry->value = NULL;
    return true;
}

void builtin_table_add_all(BuiltinTable *from, BuiltinTable *to)
{
    for (int i = 0; i < from->capacity; i++)
    {
        BuiltinEntry *entry = &from->entries[i];
        if (entry->key != NULL)
        {
            builtin_table_set(to, entry->key, entry->hash, entry->value);
        }
    }
}