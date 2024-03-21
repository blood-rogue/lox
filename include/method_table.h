#ifndef lox_method_table_h
#define lox_method_table_h

#include "common.h"

typedef struct {
    Obj *value;
    char *error;
} NativeResult;

typedef NativeResult (*NativeFn)(int, Obj **, Obj *);

typedef struct {
    uint32_t hash;
    char *key;
    NativeFn value;
} NativeEntry;

typedef struct {
    int count;
    int capacity;
    NativeEntry *entries;
} NativeTable;

void init_method_table(NativeTable *, int);
void free_method_table(NativeTable *);

bool method_table_get(NativeTable *, uint32_t, NativeFn *);
bool method_table_set(NativeTable *, char *, uint32_t, NativeFn);
bool method_table_delete(NativeTable *, uint32_t);

void method_table_add_all(NativeTable *, NativeTable *);

#endif // lox_method_table_h
