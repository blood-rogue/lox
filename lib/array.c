#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"

void init_array(Array *array) {
    array->values = NULL;
    array->capacity = 0;
    array->count = 0;
}

void write_array(Array *array, Obj *value) {
    if (array->capacity < array->count + 1) {
        int old_capacity = array->capacity;
        array->capacity = GROW_CAPACITY(old_capacity);
        array->values =
            GROW_ARRAY(Obj *, array->values, old_capacity, array->capacity);
    }

    array->values[array->count] = value;
    array->count++;
}

void free_array(Array *array) {
    FREE_ARRAY(Obj *, array->values, array->capacity);
    init_array(array);
}
