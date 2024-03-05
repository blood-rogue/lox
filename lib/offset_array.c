#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "offset_array.h"

void init_offset_array(OffsetArray *array) {
    array->values = NULL;
    array->capacity = 0;
    array->count = 0;
}

void write_offset_array(OffsetArray *array, int offset) {
    if (array->capacity < array->count + 1) {
        int old_capacity = array->capacity;
        array->capacity = GROW_CAPACITY(old_capacity);
        array->values = GROW_ARRAY(int, array->values, old_capacity, array->capacity);
    }

    array->values[array->count] = offset;
    array->count++;
}

void free_offset_array(OffsetArray *array) {
    FREE_ARRAY(int, array->values, array->capacity);
    init_offset_array(array);
}
