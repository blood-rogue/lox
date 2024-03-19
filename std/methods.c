#include "std/methods.h"

BuiltinTable **get_builtin_methods() {
    BuiltinTable **statics = calloc(NUM_OBJS, sizeof(BuiltinTable *));

    statics[__builtin_ctz(OBJ_FLOAT)] = float_methods();
    statics[__builtin_ctz(OBJ_INT)] = int_methods();
    statics[__builtin_ctz(OBJ_CHAR)] = char_methods();
    statics[__builtin_ctz(OBJ_STRING)] = string_methods();
    statics[__builtin_ctz(OBJ_LIST)] = list_methods();
    statics[__builtin_ctz(OBJ_MAP)] = map_methods();
    statics[__builtin_ctz(OBJ_BYTES)] = bytes_methods();

    return statics;
}
