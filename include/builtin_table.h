#ifndef clox_builtin_table_h
#define clox_builtin_table_h

#include "common.h"

typedef struct
{
    Obj *value;
    char *error;
} BuiltinResult;

typedef BuiltinResult (*BuiltinFn)(int, Obj **);

typedef struct
{
    uint32_t hash;
    char *key;
    BuiltinFn value;
} BuiltinEntry;

typedef struct
{
    int count;
    int capacity;
    BuiltinEntry *entries;
} BuiltinTable;

void init_builtin_table(BuiltinTable *);
void free_builtin_table(BuiltinTable *);

bool builtin_table_get(BuiltinTable *, uint32_t, BuiltinFn *);
bool builtin_table_set(BuiltinTable *, char *, uint32_t, BuiltinFn);
bool builtin_table_delete(BuiltinTable *, uint32_t);

void builtin_table_add_all(BuiltinTable *, BuiltinTable *);

#endif // clox_builtin_table_h