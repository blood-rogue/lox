#include "builtins.h"

STATIC(int, parse) {
    CHECK_ARG_COUNT(1)
    Obj *arg = argv[0];

    switch (arg->type) {
        case OBJ_INT:
            return OK(arg);
        case OBJ_STRING:
            return OK(AS_OBJ(
                new_int((int64_t)strtol(AS_STRING(arg)->chars, NULL, 10))));
        case OBJ_FLOAT:
            return OK(AS_OBJ(new_int((int64_t)floor(AS_FLOAT(arg)->value))));
        default:
            return ERR("Cannot parse to int");
    }
    return OK(AS_OBJ(AS_INT(arg)));
}

STATIC(float, parse) {
    CHECK_ARG_COUNT(1)
    Obj *arg = argv[0];

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

ObjBuiltinClass *int_builtin_class() {
    ObjBuiltinClass *klass = new_builtin_class(8);

    SET_STATIC(int, parse, 5);

    return klass;
}

ObjBuiltinClass *float_builtin_class() {
    ObjBuiltinClass *klass = new_builtin_class(8);

    SET_STATIC(float, parse, 5);

    return klass;
}
