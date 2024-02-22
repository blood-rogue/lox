#include "builtins.h"

STATIC(float, parse);

ObjBuiltinClass *float_builtin_class() {
    ObjBuiltinClass *klass = new_builtin_class();

    SET_STATIC(float, parse, 5);

    return klass;
}

STATIC(float, parse) {
    CHECK_ARG_COUNT(1)
    Obj *arg = args[0];

    switch (arg->type) {
        case OBJ_INT:
            return OK(AS_OBJ(new_float(AS_INT(arg)->value * 1.0)));
        case OBJ_STRING:
            return OK(AS_OBJ(new_float(strtod(AS_STRING(arg)->chars, NULL))));
        default:
            return ERR("Cannot parse to int");
    }
    return OK(AS_OBJ(AS_INT(arg)));
}