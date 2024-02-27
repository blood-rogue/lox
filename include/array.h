#ifndef lox_array_h
#define lox_array_h

#include "common.h"

typedef struct {
    int capacity;
    int count;
    Obj **values;
} Array;

void init_array(Array *);
void write_array(Array *, Obj *);
void free_array(Array *);

#endif // lox_array_h
