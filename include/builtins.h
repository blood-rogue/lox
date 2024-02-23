#ifndef clox_builtin_h
#define clox_builtin_h

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common.h"
#include "memory.h"
#include "object.h"

#define ERR(err)                                                                                   \
    (BuiltinResult) { .error = err, .value = AS_OBJ(new_nil()) }
#define OK(ok)                                                                                     \
    (BuiltinResult) { .value = ok, .error = NULL }

#define CHECK_ARG_COUNT(expected)                                                                  \
    if (argc != expected) {                                                                        \
        char buf[100];                                                                             \
        sprintf(buf, "Expected %d arguments but got %d", expected, argc);                          \
        return ERR(buf);                                                                           \
    }

#define UNUSED(typ, name) typ name##_UNUSED __attribute__((unused))
#define BLTIN_FN(name)    BuiltinResult name##_builtin_function(int, Obj **, Obj *)

BLTIN_FN(clock);
BLTIN_FN(exit);
BLTIN_FN(print);
BLTIN_FN(input);
BLTIN_FN(len);
BLTIN_FN(argv);
BLTIN_FN(run_gc);

BuiltinMethodTable **get_builtin_methods();

BuiltinMethodTable *nil_methods();
BuiltinMethodTable *float_methods();
BuiltinMethodTable *int_methods();
BuiltinMethodTable *bool_methods();
BuiltinMethodTable *string_methods();
BuiltinMethodTable *list_methods();
BuiltinMethodTable *map_methods();

#endif // clox_builtin_h
