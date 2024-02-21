#ifndef clox_builtin_h
#define clox_builtin_h

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "common.h"
#include "object.h"
#include "memory.h"

#define ERR(err) \
    (BuiltinResult) { .error = err, .value = OBJ_VAL(new_nil()) }
#define OK(ok) \
    (BuiltinResult) { .value = ok, .error = NULL }

#define CHECK_ARG_COUNT(expected)                                              \
    if (arg_count != expected)                                                 \
    {                                                                          \
        char buf[100];                                                         \
        sprintf(buf, "Expected %d arguments but got %d", expected, arg_count); \
        return ERR(buf);                                                       \
    }

#define BUILTIN_FUNCTION(name) BuiltinResult name##_builtin_function(int, Obj **)
#define BUILTIN_CLASS(name) ObjBuiltinClass *name##_builtin_class()

BUILTIN_FUNCTION(clock);
BUILTIN_FUNCTION(exit);
BUILTIN_FUNCTION(print);
BUILTIN_FUNCTION(input);
BUILTIN_FUNCTION(len);
BUILTIN_FUNCTION(argv);

BUILTIN_CLASS(int);

#endif // clox_builtin_h
