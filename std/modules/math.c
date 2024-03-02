#include <math.h>

#include "builtins.h"
#include "object.h"

#define DEFINE_MEMBER(func)                                                                        \
    BuiltinResult _math_##func(int argc, Obj **argv, UNUSED(Obj *, caller)) {                      \
        CHECK_ARG_COUNT(1)                                                                         \
        if (IS_FLOAT(argv[0]))                                                                     \
            return OK(new_float(func(AS_FLOAT(argv[0])->value)));                                  \
        return ERR("Expected float argument.");                                                    \
    }

DEFINE_MEMBER(fabs)
DEFINE_MEMBER(exp)
DEFINE_MEMBER(exp2)
DEFINE_MEMBER(log)
DEFINE_MEMBER(log10)
DEFINE_MEMBER(log2)
DEFINE_MEMBER(sqrt)
DEFINE_MEMBER(cbrt)
DEFINE_MEMBER(sin)
DEFINE_MEMBER(cos)
DEFINE_MEMBER(tan)
DEFINE_MEMBER(asin)
DEFINE_MEMBER(acos)
DEFINE_MEMBER(atan)
DEFINE_MEMBER(sinh)
DEFINE_MEMBER(cosh)
DEFINE_MEMBER(tanh)
DEFINE_MEMBER(asinh)
DEFINE_MEMBER(acosh)
DEFINE_MEMBER(atanh)
DEFINE_MEMBER(ceil)
DEFINE_MEMBER(floor)
DEFINE_MEMBER(trunc)
DEFINE_MEMBER(round)
DEFINE_MEMBER(nearbyint)
DEFINE_MEMBER(rint)

BuiltinResult _math_pow(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)
    if (IS_FLOAT(argv[0]) && IS_FLOAT(argv[1]))
        return OK(new_float(pow(AS_FLOAT(argv[0])->value, AS_FLOAT(argv[1])->value)));

    return ERR("Expected float argument.");
}

BuiltinResult _math_hypot(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)
    if (IS_FLOAT(argv[0]) && IS_FLOAT(argv[1]))
        return OK(new_float(hypot(AS_FLOAT(argv[0])->value, AS_FLOAT(argv[1])->value)));

    return ERR("Expected float argument.");
}

static ObjClass *_math_complex_class = NULL;

static ObjModule *_math_module = NULL;

ObjModule *get_math_module() {
    if (_math_module == NULL) {
        ObjModule *module = new_module(new_string("math", 4));

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
        SET_BUILTIN_FN_MEMBER("rint", _math_rint);

        SET_FLOAT_MEMBER("NAN", __builtin_nan(""));
        SET_FLOAT_MEMBER("INFINITY", __builtin_inf());

        if (_math_complex_class == NULL) {
            ObjClass *klass = new_class(new_string("Complex", 7));

            _math_complex_class = klass;
        }

        SET_MEMBER("Complex", _math_complex_class);

        _math_module = module;
    }

    return _math_module;
}
