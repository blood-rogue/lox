#ifndef lox_table_h
#define lox_table_h

#include "common.h"

typedef struct {
    Obj *key;
    Obj *value;
} Entry;

typedef struct {
    int count;
    int capacity;
    Entry *entries;
} Table;

void init_table(Table *);
void free_table(Table *);

bool table_get(Table *, Obj *, Obj **);
bool table_set(Table *, Obj *, Obj *);
bool table_delete(Table *, Obj *);

void table_add_all(Table *, Table *);
Obj *table_find_string(Table *, uint32_t);
void table_remove_white(Table *);

void mark_table(Table *);

#endif // lox_table_h
