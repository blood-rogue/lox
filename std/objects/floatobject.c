#include "builtins.h"

BuiltinResult _float_to_str(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjFloat *_float = AS_FLOAT(caller);

    int size = snprintf(NULL, 0, "%f", _float->value) + 1;
    char *buf = malloc(size);
    snprintf(buf, size, "%f", _float->value);

    return OK(AS_OBJ(take_string(buf, (int)strlen(buf))));
}

BuiltinResult _float_is_nan(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(AS_OBJ(new_bool(isnan(AS_FLOAT(caller)->value))));
}

BuiltinResult _float_is_finite(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(AS_OBJ(new_bool(isfinite(AS_FLOAT(caller)->value))));
}

BuiltinResult _float_is_infinite(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(AS_OBJ(new_bool(isinf(AS_FLOAT(caller)->value))));
}

BuiltinResult _float_is_normal(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(AS_OBJ(new_bool(isnormal(AS_FLOAT(caller)->value))));
}

BuiltinTable *float_methods() {
    BuiltinTable *table = malloc(sizeof(BuiltinTable));
    init_method_table(table, 8);

    SET_BLTIN_METHOD(float, to_str);
    SET_BLTIN_METHOD(float, is_nan);
    SET_BLTIN_METHOD(float, is_finite);
    SET_BLTIN_METHOD(float, is_infinite);
    SET_BLTIN_METHOD(float, is_normal);

    INFINITY;

    return table;
}