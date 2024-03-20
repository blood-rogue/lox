#include <math.h>

#include "builtins.h"

static ObjModule *_math_module = NULL;

#define MATH_BLTIN_FN(func)                                                                        \
    static BuiltinResult _math_##func(int argc, Obj **argv, UNUSED(Obj *caller)) {                 \
        CHECK_ARG_COUNT(1)                                                                         \
        CHECK_ARG_TYPE(ObjFloat, FLOAT, 0)                                                         \
        OK(new_float(func(argv_0->value)));                                                        \
    }

MATH_BLTIN_FN(fabs)
MATH_BLTIN_FN(exp)
MATH_BLTIN_FN(exp2)
MATH_BLTIN_FN(log)
MATH_BLTIN_FN(log10)
MATH_BLTIN_FN(log2)
MATH_BLTIN_FN(sqrt)
MATH_BLTIN_FN(cbrt)
MATH_BLTIN_FN(sin)
MATH_BLTIN_FN(cos)
MATH_BLTIN_FN(tan)
MATH_BLTIN_FN(asin)
MATH_BLTIN_FN(acos)
MATH_BLTIN_FN(atan)
MATH_BLTIN_FN(sinh)
MATH_BLTIN_FN(cosh)
MATH_BLTIN_FN(tanh)
MATH_BLTIN_FN(asinh)
MATH_BLTIN_FN(acosh)
MATH_BLTIN_FN(atanh)
MATH_BLTIN_FN(ceil)
MATH_BLTIN_FN(floor)
MATH_BLTIN_FN(trunc)
MATH_BLTIN_FN(round)
MATH_BLTIN_FN(nearbyint)
MATH_BLTIN_FN(rint)

static BuiltinResult _math_pow(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjFloat, FLOAT, 0)
    CHECK_ARG_TYPE(ObjFloat, FLOAT, 1)

    OK(new_float(pow(argv_0->value, argv_1->value)));
}

static BuiltinResult _math_hypot(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjFloat, FLOAT, 0)
    CHECK_ARG_TYPE(ObjFloat, FLOAT, 1)

    OK(new_float(hypot(argv_0->value, argv_1->value)));
}

ObjModule *get_math_module(int count, UNUSED(char **parts)) {
    CHECK_PART_COUNT

    if (_math_module == NULL) {
        ObjModule *module = new_module("math");

        SET_BUILTIN_FN_MEMBER("abs", _math_fabs);
        SET_BUILTIN_FN_MEMBER("exp", _math_exp);
        SET_BUILTIN_FN_MEMBER("exp2", _math_exp2);
        SET_BUILTIN_FN_MEMBER("log", _math_log);
        SET_BUILTIN_FN_MEMBER("log10", _math_log10);
        SET_BUILTIN_FN_MEMBER("log2", _math_log2);
        SET_BUILTIN_FN_MEMBER("pow", _math_pow);
        SET_BUILTIN_FN_MEMBER("sqrt", _math_sqrt);
        SET_BUILTIN_FN_MEMBER("cbrt", _math_cbrt);
        SET_BUILTIN_FN_MEMBER("hypot", _math_hypot);
        SET_BUILTIN_FN_MEMBER("sin", _math_sin);
        SET_BUILTIN_FN_MEMBER("cos", _math_cos);
        SET_BUILTIN_FN_MEMBER("tan", _math_tan);
        SET_BUILTIN_FN_MEMBER("asin", _math_asin);
        SET_BUILTIN_FN_MEMBER("acos", _math_acos);
        SET_BUILTIN_FN_MEMBER("atan", _math_atan);
        SET_BUILTIN_FN_MEMBER("sinh", _math_sinh);
        SET_BUILTIN_FN_MEMBER("cosh", _math_cosh);
        SET_BUILTIN_FN_MEMBER("tanh", _math_tanh);
        SET_BUILTIN_FN_MEMBER("asinh", _math_asinh);
        SET_BUILTIN_FN_MEMBER("acosh", _math_acosh);
        SET_BUILTIN_FN_MEMBER("atanh", _math_atanh);
        SET_BUILTIN_FN_MEMBER("ceil", _math_ceil);
        SET_BUILTIN_FN_MEMBER("floor", _math_floor);
        SET_BUILTIN_FN_MEMBER("trunc", _math_trunc);
        SET_BUILTIN_FN_MEMBER("round", _math_round);
        SET_BUILTIN_FN_MEMBER("nearbyint", _math_nearbyint);
        SET_BUILTIN_FN_MEMBER("rint", _math_rint);

        SET_FLOAT_MEMBER("NAN", __builtin_nan(""));
        SET_FLOAT_MEMBER("INFINITY", __builtin_inf());

        SET_MEMBER("Complex", get_math_complex_class());

        _math_module = module;
    }

    return _math_module;
}
