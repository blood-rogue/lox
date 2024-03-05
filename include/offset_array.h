#ifndef lox_offset_array_h
#define lox_offset_array_h

#include "common.h"

typedef struct {
    int capacity;
    int count;
    int *values;
} OffsetArray;

void init_offset_array(OffsetArray *);
void write_offset_array(OffsetArray *, int);
void free_offset_array(OffsetArray *);

#endif // lox_offset_array_h
