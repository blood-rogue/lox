#ifndef lox_builtin_h
#define lox_builtin_h

#include <math.h>
#include <time.h>

#include "common.h"
#include "memory.h"
#include "object.h"

#define ERR(err)                                                               \
    (BuiltinResult) { .error = err, .value = AS_OBJ(new_nil()) }
#define OK(ok)                                                                 \
    (BuiltinResult) { .value = AS_OBJ(ok), .error = NULL }

#define CHECK_ARG_COUNT(expected)                                              \
    if (argc != expected) {                                                    \
        char buf[100];                                                         \
        sprintf(buf, "Expected %d arguments but got %d", expected, argc);      \
        return ERR(buf);                                                       \
    }

#define UNUSED(typ, name) typ name##_UNUSED __attribute__((unused))
#define BLTIN_FN(name)    BuiltinResult name##_builtin_function(int, Obj **, Obj *)
#define SET_BLTIN_METHOD(obj, name)                                            \
    method_table_set(                                                          \
        table, #name, hash_string(#name, (int)strlen(#name)), _##obj##_##name)

BLTIN_FN(clock);
BLTIN_FN(exit);
BLTIN_FN(print);
BLTIN_FN(input);
BLTIN_FN(argv);
BLTIN_FN(run_gc);
BLTIN_FN(parse_int);
BLTIN_FN(parse_float);
BLTIN_FN(sleep);
BLTIN_FN(type);

BuiltinTable **get_builtin_methods();

BuiltinTable *float_methods();
BuiltinTable *int_methods();
BuiltinTable *bool_methods();
BuiltinTable *char_methods();
BuiltinTable *string_methods();
BuiltinTable *list_methods();
BuiltinTable *map_methods();

ObjModule *get_module(char *);

ObjModule *get_math_module();
ObjModule *get_fs_module();

#endif // lox_builtin_h
