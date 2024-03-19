#ifndef lox_builtin_h
#define lox_builtin_h

#include <math.h>

#include "common.h"
#include "memory.h"
#include "object.h"
#include "std/methods.h"
#include "std/modules.h"

#define ERR(...)                                                                                   \
    {                                                                                              \
        int size = snprintf(NULL, 0, __VA_ARGS__);                                                 \
        char *buf = malloc(size + 1);                                                              \
        snprintf(buf, size + 1, __VA_ARGS__);                                                      \
        return (BuiltinResult){.value = AS_OBJ(new_nil()), .error = buf};                          \
    }
#define OK(ok)                                                                                     \
    return (BuiltinResult) { .value = AS_OBJ(ok), .error = NULL }

#define CHECK_ARG_COUNT(expected)                                                                  \
    if (argc != expected)                                                                          \
    ERR("Expected %d arguments but got %d", expected, argc)

#define CHECK_ARG_TYPE(typ, kind, pos)                                                             \
    if (!IS_##kind(argv[pos]))                                                                     \
        ERR("Expected %s at pos %d but got %s", #kind, pos, get_obj_kind(argv[pos]))               \
    typ *argv_##pos = AS_##kind(argv[pos]);

#define BLTIN_FN(name) BuiltinResult name##_builtin_function(int, Obj **, Obj *)

#define GET_INTERNAL(typ, name)                                                                    \
    Obj *name##_obj;                                                                               \
    table_get(&name##_instance->fields, AS_OBJ(new_string("$$internal", 10)), &name##_obj);        \
    typ name = AS_NATIVE_STRUCT(name##_obj)->ptr;

BLTIN_FN(exit);
BLTIN_FN(print);
BLTIN_FN(input);
BLTIN_FN(argv);
BLTIN_FN(run_gc);
BLTIN_FN(parse_int);
BLTIN_FN(parse_float);
BLTIN_FN(sleep);
BLTIN_FN(type);
BLTIN_FN(repr);
BLTIN_FN(assert);

#endif // lox_builtin_h
