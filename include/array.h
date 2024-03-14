#ifndef lox_array_h
#define lox_array_h

#include "common.h"

typedef struct {
    int capacity;
    int count;
    Obj **values;
} Array;

void init_array(Array *);
void free_array(Array *);

void write_array(Array *, Obj *);

Obj *remove_at_array(Array *, size_t);
void insert_at_array(Array *, size_t, Obj *);

#endif // lox_array_h
