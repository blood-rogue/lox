#ifndef lox_native_h
#define lox_native_h

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
        return (NativeResult){.value = AS_OBJ(new_nil()), .error = buf};                           \
    }
#define OK(ok)                                                                                     \
    return (NativeResult) { .value = AS_OBJ(ok), .error = NULL }

#define CHECK_ARG_COUNT(expected)                                                                  \
    if (argc != expected)                                                                          \
    ERR("Expected %d arguments but got %d", expected, argc)

#define CHECK_ARG_TYPE(typ, kind, pos)                                                             \
    if (!IS_##kind(argv[pos]))                                                                     \
        ERR("Expected '%s' at pos %d but got '%s'", #kind, pos, get_obj_kind(argv[pos]))           \
    typ *argv_##pos = AS_##kind(argv[pos]);

#define NATIVE_FN(name) NativeResult name##_native_function(int, Obj **, Obj *)

#define GET_INTERNAL(typ, name)                                                                    \
    Obj *name##_obj;                                                                               \
    table_get(&name##_instance->fields, AS_OBJ(new_string("$$internal", 10)), &name##_obj);        \
    ObjNativeStruct *native = AS_NATIVE_STRUCT(name##_obj);                                        \
    typ name = native->ptr;

NATIVE_FN(exit);
NATIVE_FN(print);
NATIVE_FN(input);
NATIVE_FN(argv);
NATIVE_FN(run_gc);
NATIVE_FN(sleep);
NATIVE_FN(type);
NATIVE_FN(repr);
NATIVE_FN(assert);

#endif // lox_native_h
