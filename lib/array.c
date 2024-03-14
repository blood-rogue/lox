#include <stdio.h>
#include <string.h>

#include "array.h"
#include "memory.h"
#include "object.h"

void init_array(Array *array) {
    array->values = NULL;
    array->capacity = 0;
    array->count = 0;
}

void free_array(Array *array) {
    FREE_ARRAY(Obj *, array->values, array->capacity);
    init_array(array);
}

void write_array(Array *array, Obj *value) {
    if (array->capacity < array->count + 1) {
        int old_capacity = array->capacity;
        array->capacity = GROW_CAPACITY(old_capacity);
        array->values = GROW_ARRAY(Obj *, array->values, old_capacity, array->capacity);
    }

    array->values[array->count] = value;
    array->count++;
}

Obj *remove_at_array(Array *array, size_t pos) {
    Obj *obj = array->values[pos];
    memmove(array->values + pos, array->values + pos + 1, (array->count - 1 - pos) * sizeof(Obj *));
    array->count--;

    return obj;
}

void insert_at_array(Array *array, size_t pos, Obj *obj) {
    write_array(array, AS_OBJ(new_nil()));
    memmove(array->values + pos + 1, array->values + pos, (array->count - 1 - pos) * sizeof(Obj *));

    array->values[pos] = obj;
}
