#ifndef clox_array_h
#define clox_array_h

#include "common.h"

typedef struct
{
    int capacity;
    int count;
    Obj **values;
} Array;

void init_array(Array *);
void write_array(Array *, Obj *);
void free_array(Array *);

#endif // clox_array_h
