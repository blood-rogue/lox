#include "native.h"

static NativeResult _float_is_nan(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    OK(AS_OBJ(new_bool(mpfr_nan_p(AS_FLOAT(caller)->value))));
}

static NativeResult _float_is_finite(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    OK(AS_OBJ(new_bool(mpfr_number_p(AS_FLOAT(caller)->value))));
}

static NativeResult _float_is_infinite(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    OK(AS_OBJ(new_bool(mpfr_inf_p(AS_FLOAT(caller)->value))));
}

static NativeResult _float_new(int argc, UNUSED(Obj **argv), UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(0)
    OK(new_float_d(0.0));
}

static ObjClass *_float_class = NULL;

ObjClass *get_float_class() {
    if (_float_class == NULL) {
        ObjClass *klass = new_native_class("Float");

        SET_NATIVE_FN_STATIC("__new", _float_new);

        SET_NATIVE_FN_METHOD("is_nan", _float_is_nan);
        SET_NATIVE_FN_METHOD("is_finite", _float_is_finite);
        SET_NATIVE_FN_METHOD("is_infinite", _float_is_infinite);

        _float_class = klass;
    }

    return _float_class;
}
