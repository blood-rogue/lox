#include "builtins.h"

STATIC(int, parse);

ObjBuiltinClass *int_builtin_class() {
    ObjBuiltinClass *klass = new_builtin_class();

    SET_STATIC(int, parse, 5);

    return klass;
}

STATIC(int, parse) {
    CHECK_ARG_COUNT(1)
    Obj *arg = args[0];

    switch (arg->type) {
        case OBJ_INT:
            return OK(arg);
        case OBJ_STRING:
            return OK(OBJ_VAL(
                new_int((int64_t)strtol(AS_STRING(arg)->chars, NULL, 10))));
        case OBJ_FLOAT:
            return OK(OBJ_VAL(new_int((int64_t)floor(AS_FLOAT(arg)->value))));
        default:
            return ERR("Cannot parse to int");
    }
    return OK(OBJ_VAL(AS_INT(arg)));
}