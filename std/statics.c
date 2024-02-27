#include "builtins.h"

BuiltinTable **get_builtin_methods() {
    BuiltinTable **statics = calloc(NUM_OBJS, sizeof(BuiltinTable *));

    statics[OBJ_FLOAT] = float_methods();
    statics[OBJ_INT] = int_methods();
    statics[OBJ_CHAR] = char_methods();
    statics[OBJ_BOOL] = bool_methods();
    statics[OBJ_STRING] = string_methods();
    statics[OBJ_LIST] = list_methods();
    statics[OBJ_MAP] = map_methods();

    return statics;
}
