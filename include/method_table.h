#ifndef clox_method_table_h
#define clox_method_table_h

#include "common.h"

typedef struct {
    Obj *value;
    char *error;
} BuiltinResult;

typedef BuiltinResult (*BuiltinMethodFn)(int, Obj **, Obj *);

typedef struct {
    uint32_t hash;
    char *key;
    BuiltinMethodFn value;
} BuiltinMethodEntry;

typedef struct {
    int count;
    int capacity;
    BuiltinMethodEntry *entries;
} BuiltinMethodTable;

void init_method_table(BuiltinMethodTable *, int);
void free_method_table(BuiltinMethodTable *);

bool method_table_get(BuiltinMethodTable *, uint32_t, BuiltinMethodFn *);
bool method_table_set(BuiltinMethodTable *, char *, uint32_t, BuiltinMethodFn);
bool method_table_delete(BuiltinMethodTable *, uint32_t);

void method_table_add_all(BuiltinMethodTable *, BuiltinMethodTable *);

#endif // clox_method_table_h