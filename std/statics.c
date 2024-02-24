#include "builtins.h"

BuiltinMethodTable **get_builtin_methods() {
    BuiltinMethodTable **statics = calloc(16, sizeof(BuiltinMethodTable *));

    statics[OBJ_FLOAT] = float_methods();
    statics[OBJ_INT] = int_methods();
    statics[OBJ_CHAR] = char_methods();
    statics[OBJ_BOOL] = bool_methods();
    statics[OBJ_STRING] = string_methods();
    statics[OBJ_LIST] = list_methods();
    statics[OBJ_MAP] = map_methods();

    return statics;
}