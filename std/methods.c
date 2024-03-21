#include "std/methods.h"

NativeTable **get_builtin_methods() {
    NativeTable **statics = calloc(NUM_OBJS, sizeof(NativeTable *));

    statics[__builtin_ctz(OBJ_FLOAT)] = float_methods();
    statics[__builtin_ctz(OBJ_INT)] = int_methods();
    statics[__builtin_ctz(OBJ_CHAR)] = char_methods();
    statics[__builtin_ctz(OBJ_STRING)] = string_methods();
    statics[__builtin_ctz(OBJ_LIST)] = list_methods();
    statics[__builtin_ctz(OBJ_MAP)] = map_methods();
    statics[__builtin_ctz(OBJ_BYTES)] = bytes_methods();

    return statics;
}
