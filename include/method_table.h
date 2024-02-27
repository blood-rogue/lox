#ifndef lox_method_table_h
#define lox_method_table_h

#include "common.h"

typedef struct {
    Obj *value;
    char *error;
} BuiltinResult;

typedef BuiltinResult (*BuiltinFn)(int, Obj **, Obj *);

typedef struct {
    uint32_t hash;
    char *key;
    BuiltinFn value;
} BuiltinEntry;

typedef struct {
    int count;
    int capacity;
    BuiltinEntry *entries;
} BuiltinTable;

void init_method_table(BuiltinTable *, int);
void free_method_table(BuiltinTable *);

bool method_table_get(BuiltinTable *, uint32_t, BuiltinFn *);
bool method_table_set(BuiltinTable *, char *, uint32_t, BuiltinFn);
bool method_table_delete(BuiltinTable *, uint32_t);

void method_table_add_all(BuiltinTable *, BuiltinTable *);

#endif // lox_method_table_h
