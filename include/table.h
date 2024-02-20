#ifndef clox_table_h
#define clox_table_h

#include "common.h"

typedef struct
{
    Obj *key;
    Obj *value;
} Entry;

typedef struct
{
    int count;
    int capacity;
    Entry *entries;
} Table;

void init_table(Table *);
void free_table(Table *);

bool table_get(Table *, Obj *, Obj **);
bool table_set(Table *, Obj *, Obj *);
bool table_delete(Table *, Obj *);

void table_addAll(Table *, Table *);
Obj *table_findString(Table *, const char *, int, uint32_t);
void table_removeWhite(Table *);

void mark_table(Table *);

#endif // clox_table_h
