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

#define SET_TABLE(name) SET_MODULE_TABLE(math, name)

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

static ObjModule *__math_module = NULL;

ObjModule *get_math_module() {
    if (__math_module == NULL) {
        ObjModule *module = new_module(new_string("math", 4));

        SET_TABLE(fabs);
        SET_TABLE(exp);
        SET_TABLE(exp2);
        SET_TABLE(log);
        SET_TABLE(log10);
        SET_TABLE(log2);
        SET_TABLE(pow);
        SET_TABLE(sqrt);
        SET_TABLE(cbrt);
        SET_TABLE(hypot);
        SET_TABLE(sin);
        SET_TABLE(cos);
        SET_TABLE(tan);
        SET_TABLE(asin);
        SET_TABLE(acos);
        SET_TABLE(atan);
        SET_TABLE(sinh);
        SET_TABLE(cosh);
        SET_TABLE(tanh);
        SET_TABLE(asinh);
        SET_TABLE(acosh);
        SET_TABLE(atanh);
        SET_TABLE(ceil);
        SET_TABLE(floor);
        SET_TABLE(trunc);
        SET_TABLE(round);
        SET_TABLE(rint);

        __math_module = module;
    }

    return __math_module;
}
