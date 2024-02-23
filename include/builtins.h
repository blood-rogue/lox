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
    if (argc != expected) {                                                    \
        char buf[100];                                                         \
        sprintf(buf, "Expected %d arguments but got %d", expected, argc);      \
        return ERR(buf);                                                       \
    }

#define UNUSED(type, name) type name##_UNUSED __attribute__((unused))

#define STATIC(klass, name)                                                    \
    BuiltinResult __##klass##_##name##_builtin_static(int argc, Obj **argv,    \
                                                      UNUSED(Obj *, callee))
#define SET_STATIC(_klass, name, len)                                          \
    method_table_set(&klass->methods, #name, hash_string(#name, len),          \
                     __##_klass##_##name##_builtin_static)

#define BLTIN_FN(name)      BuiltinResult name##_builtin_function(int, Obj **, Obj *)
#define BUILTIN_CLASS(name) ObjBuiltinClass *name##_builtin_class()

BLTIN_FN(clock);
BLTIN_FN(exit);
BLTIN_FN(print);
BLTIN_FN(input);
BLTIN_FN(len);
BLTIN_FN(argv);
BLTIN_FN(run_gc);

BUILTIN_CLASS(int);
BUILTIN_CLASS(float);

BuiltinMethodTable **get_builtin_methods();

BuiltinMethodTable *nil_methods();
BuiltinMethodTable *float_methods();
BuiltinMethodTable *int_methods();
BuiltinMethodTable *bool_methods();
BuiltinMethodTable *string_methods();
BuiltinMethodTable *list_methods();
BuiltinMethodTable *map_methods();

#endif // clox_builtin_h
