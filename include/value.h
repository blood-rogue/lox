#ifndef clox_value_h
#define clox_value_h

#include "common.h"

typedef struct
{
    int capacity;
    int count;
    Obj **values;
} ValueArray;

void initValueArray(ValueArray *);
void writeValueArray(ValueArray *, Obj *);
void freeValueArray(ValueArray *);

#endif // clox_value_h
