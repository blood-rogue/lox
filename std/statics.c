#include "builtins.h"

BuiltinMethodTable **get_builtin_methods() {
    BuiltinMethodTable **statics = malloc(sizeof(BuiltinMethodTable *) * 15);

    statics[OBJ_NIL] = nil_methods();
    statics[OBJ_FLOAT] = float_methods();
    statics[OBJ_INT] = int_methods();
    statics[OBJ_BOOL] = bool_methods();
    statics[OBJ_BOUND_METHOD] = NULL;
    statics[OBJ_CLASS] = NULL;
    statics[OBJ_CLOSURE] = NULL;
    statics[OBJ_FUNCTION] = NULL;
    statics[OBJ_INSTANCE] = NULL;
    statics[OBJ_BUILTIN_FUNCTION] = NULL;
    statics[OBJ_STRING] = string_methods();
    statics[OBJ_UPVALUE] = NULL;
    statics[OBJ_LIST] = list_methods();
    statics[OBJ_MAP] = map_methods();
    statics[OBJ_BUILTIN_BOUND_METHOD] = NULL;

    return statics;
}