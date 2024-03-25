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

static NativeResult _float_eq(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjFloat, FLOAT, 0)
    CHECK_ARG_TYPE(ObjFloat, FLOAT, 1)

    OK(new_bool(mpfr_cmp(argv_0->value, argv_1->value) == 0));
}

static NativeResult _float_init(int argc, Obj **argv, Obj *caller) {
    if (argc == 0) {
    } else if (argc == 1) {
        switch (argv[0]->type) {
            case OBJ_FLOAT:
                break;
            case OBJ_INT:
                mpfr_set_z(AS_FLOAT(caller)->value, AS_INT(argv[0])->value, MPFR_RNDN);
                break;
            default:
                ERR("Cannot parse '%s' into Int.", get_obj_kind(argv[0]));
        }
    } else
        ERR("Expected 0 or 1 argument(s) but got %d", argc)

    OK(new_nil());
}

static NativeResult _float_add(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjFloat, FLOAT, 0)
    CHECK_ARG_TYPE(ObjFloat, FLOAT, 1)

    mpfr_t r;
    mpfr_init_set_d(r, 0.0, MPFR_RNDN);
    mpfr_add(r, argv_0->value, argv_1->value, MPFR_RNDN);

    OK(new_float(r));
}

static NativeResult _float_sub(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjFloat, FLOAT, 0)
    CHECK_ARG_TYPE(ObjFloat, FLOAT, 1)

    mpfr_t r;
    mpfr_init_set_d(r, 0.0, MPFR_RNDN);
    mpfr_sub(r, argv_0->value, argv_1->value, MPFR_RNDN);

    OK(new_float(r));
}

static NativeResult _float_mul(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjFloat, FLOAT, 0)
    CHECK_ARG_TYPE(ObjFloat, FLOAT, 1)

    mpfr_t r;
    mpfr_init_set_d(r, 0.0, MPFR_RNDN);
    mpfr_mul(r, argv_0->value, argv_1->value, MPFR_RNDN);

    OK(new_float(r));
}

static NativeResult _float_div(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjFloat, FLOAT, 0)
    CHECK_ARG_TYPE(ObjFloat, FLOAT, 1)

    mpfr_t r;
    mpfr_init_set_d(r, 0.0, MPFR_RNDN);
    mpfr_div(r, argv_0->value, argv_1->value, MPFR_RNDN);

    OK(new_float(r));
}

static NativeResult _float_neg(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjFloat, FLOAT, 0)

    mpfr_t r;
    mpfr_init_set_d(r, 0.0, MPFR_RNDN);
    mpfr_neg(r, argv_0->value, MPFR_RNDN);

    OK(new_float(r));
}

static NativeResult _float_lt(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjFloat, FLOAT, 0)
    CHECK_ARG_TYPE(ObjFloat, FLOAT, 1)

    OK(new_bool(mpfr_cmp(argv_0->value, argv_1->value) < 0));
}

static NativeResult _float_gt(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjFloat, FLOAT, 0)
    CHECK_ARG_TYPE(ObjFloat, FLOAT, 1)

    OK(new_bool(mpfr_cmp(argv_0->value, argv_1->value) > 0));
}

static ObjClass *_float_class = NULL;

ObjClass *get_float_class() {
    if (_float_class == NULL) {
        ObjClass *klass = new_native_class("Float");

        SET_NATIVE_FN_STATIC("__new", _float_new);
        SET_NATIVE_FN_STATIC("__add", _float_add);
        SET_NATIVE_FN_STATIC("__sub", _float_sub);
        SET_NATIVE_FN_STATIC("__mul", _float_mul);
        SET_NATIVE_FN_STATIC("__div", _float_div);
        SET_NATIVE_FN_STATIC("__eq", _float_eq);
        SET_NATIVE_FN_STATIC("__neg", _float_neg);
        SET_NATIVE_FN_STATIC("__lt", _float_lt);
        SET_NATIVE_FN_STATIC("__gt", _float_gt);

        SET_NATIVE_FN_METHOD("__init", _float_init);
        SET_NATIVE_FN_METHOD("is_nan", _float_is_nan);
        SET_NATIVE_FN_METHOD("is_finite", _float_is_finite);
        SET_NATIVE_FN_METHOD("is_infinite", _float_is_infinite);

        _float_class = klass;
    }

    return _float_class;
}
