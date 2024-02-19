#ifndef clox_table_h
#define clox_table_h

#include "common.h"
#include "value.h"

typedef struct
{
    Obj *key;
    Value value;
} Entry;

typedef struct
{
    int count;
    int capacity;
    Entry *entries;
} Table;

void initTable(Table *);
void freeTable(Table *);

bool tableGet(Table *, Obj *, Value *);
bool tableSet(Table *, Obj *, Value);
bool tableDelete(Table *, Obj *);

void tableAddAll(Table *, Table *);
Obj *tableFindString(Table *, const char *, int, uint32_t);
void tableRemoveWhite(Table *);

void markTable(Table *);

#endif // clox_table_h
