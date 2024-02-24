#ifndef clox_memory_h
#define clox_memory_h

#include "common.h"
#include "object.h"

#define ALLOCATE(type, n)        (type *)reallocate(NULL, 0, sizeof(type) * (n))
#define FREE(type, ptr)          reallocate(ptr, sizeof(type), 0)
#define GROW_CAPACITY(capacity)  ((capacity) < 8 ? 8 : (capacity) * 2)
#define FREE_ARRAY(type, ptr, n) reallocate(ptr, sizeof(type) * (size_t)(n), 0)

#define GROW_ARRAY(type, ptr, m, n)                                            \
    (type *)reallocate(                                                        \
        ptr, sizeof(type) * (size_t)(m), sizeof(type) * (size_t)(n))

void *reallocate(void *, size_t, size_t);
void mark_object(Obj *);
void free_objects();

void sweep();
void collect_garbage();

#endif // clox_memory_h
