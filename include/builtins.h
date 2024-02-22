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

#define ERR(err)                                                               \
    (BuiltinResult) { .error = err, .value = AS_OBJ(new_nil()) }
#define OK(ok)                                                                 \
    (BuiltinResult) { .value = ok, .error = NULL }

#define CHECK_ARG_COUNT(expected)                                              \
    if (arg_count != expected) {                                               \
        char buf[100];                                                         \
        sprintf(buf, "Expected %d arguments but got %d", expected, arg_count); \
        return ERR(buf);                                                       \
    }

#define STATIC(klass, name)                                                    \
    BuiltinResult __##klass##_##name##_builtin_static(int arg_count, Obj **args)
#define SET_STATIC(_klass, name, len)                                          \
    builtin_table_set(&klass->statics, #name, hash_string(#name, len),         \
                      __##_klass##_##name##_builtin_static)

#define BUILTIN_FUNCTION(name)                                                 \
    BuiltinResult name##_builtin_function(int, Obj **)
#define BUILTIN_CLASS(name) ObjBuiltinClass *name##_builtin_class()

BUILTIN_FUNCTION(clock);
BUILTIN_FUNCTION(exit);
BUILTIN_FUNCTION(print);
BUILTIN_FUNCTION(input);
BUILTIN_FUNCTION(len);
BUILTIN_FUNCTION(argv);
BUILTIN_FUNCTION(run_gc);

BUILTIN_CLASS(int);
BUILTIN_CLASS(float);

#endif // clox_builtin_h
