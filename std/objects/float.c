#include "builtins.h"

static NativeResult _float_to_str(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjFloat *_float = AS_FLOAT(caller);

    int size = snprintf(NULL, 0, "%f", _float->value) + 1;
    char *buf = malloc(size);
    snprintf(buf, size, "%f", _float->value);

    OK(AS_OBJ(take_string(buf, (int)strlen(buf))));
}

static NativeResult _float_is_nan(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    OK(AS_OBJ(new_bool(isnan(AS_FLOAT(caller)->value))));
}

static NativeResult _float_is_finite(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    OK(AS_OBJ(new_bool(isfinite(AS_FLOAT(caller)->value))));
}

static NativeResult _float_is_infinite(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    OK(AS_OBJ(new_bool(isinf(AS_FLOAT(caller)->value))));
}

static NativeResult _float_is_normal(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    OK(AS_OBJ(new_bool(isnormal(AS_FLOAT(caller)->value))));
}

static ObjClass *_float_class = NULL;

ObjClass *get_float_class() {
    if (_float_class == NULL) {
        ObjClass *klass = new_builtin_class("Float");

        SET_BUILTIN_FN_METHOD("to_str", _float_to_str);
        SET_BUILTIN_FN_METHOD("is_nan", _float_is_nan);
        SET_BUILTIN_FN_METHOD("is_finite", _float_is_finite);
        SET_BUILTIN_FN_METHOD("is_infinite", _float_is_infinite);
        SET_BUILTIN_FN_METHOD("is_normal", _float_is_normal);

        _float_class = klass;
    }

    return _float_class;
}
