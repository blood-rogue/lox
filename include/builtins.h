#ifndef lox_builtin_h
#define lox_builtin_h

#include <math.h>

#include "common.h"
#include "memory.h"
#include "methods.h"
#include "modules.h"
#include "object.h"

#define ERR(err)                                                                                   \
    (BuiltinResult) { .value = AS_OBJ(new_nil()), .error = err }
#define OK(ok)                                                                                     \
    (BuiltinResult) { .value = AS_OBJ(ok), .error = NULL }

#define CHECK_ARG_COUNT(expected)                                                                  \
    if (argc != expected) {                                                                        \
        char buf[100];                                                                             \
        snprintf(buf, 99, "Expected %d arguments but got %d", expected, argc);                     \
        return ERR(buf);                                                                           \
    }

#define CHECK_ARG_TYPE(typ, kind, pos)                                                             \
    if (!IS_##kind(argv[pos])) {                                                                   \
        char buf[100];                                                                             \
        snprintf(                                                                                  \
            buf, 99, "Expected %s at pos %d but got %s", #kind, pos, get_obj_kind(argv[pos]));     \
        return ERR(buf);                                                                           \
    }                                                                                              \
    typ *argv_##pos = AS_##kind(argv[pos]);

#define BLTIN_FN(name) BuiltinResult name##_builtin_function(int, Obj **, Obj *)

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
