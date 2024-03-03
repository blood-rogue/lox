#include <complex.h>
#include <math.h>

#include "builtins.h"
#include "object.h"

static ObjModule *_math_module = NULL;
static ObjClass *_math_complex_class = NULL;

#define MATH_BLTIN_FN(func)                                                                        \
    static BuiltinResult _math_##func(int argc, Obj **argv, UNUSED(Obj *, caller)) {               \
        CHECK_ARG_COUNT(1)                                                                         \
        if (IS_FLOAT(argv[0]))                                                                     \
            return OK(new_float(func(AS_FLOAT(argv[0])->value)));                                  \
        return ERR("Expected float argument.");                                                    \
    }

#define COMPLEX_METHOD(name, func)                                                                 \
    static BuiltinResult name(int argc, UNUSED(Obj **, argv), Obj *caller) {                       \
        CHECK_ARG_COUNT(0)                                                                         \
        ObjInstance *_instance = AS_INSTANCE(caller);                                              \
        double r, i;                                                                               \
        Obj *field;                                                                                \
        table_get(&_instance->fields, AS_OBJ(new_string("real", 4)), &field);                      \
        r = IS_INT(field) ? (double)(AS_INT(field)->value) : AS_FLOAT(field)->value;               \
        table_get(&_instance->fields, AS_OBJ(new_string("imag", 4)), &field);                      \
        i = IS_INT(field) ? (double)(AS_INT(field)->value) : AS_FLOAT(field)->value;               \
        double complex c = func(CMPLX(r, i));                                                      \
        ObjInstance *instance = new_instance(_math_complex_class);                                 \
        SET_FLOAT_FIELD("real", creal(c));                                                         \
        SET_FLOAT_FIELD("imag", cimag(c));                                                         \
        return OK(instance);                                                                       \
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

static BuiltinResult _math_pow(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)
    if (IS_FLOAT(argv[0]) && IS_FLOAT(argv[1]))
        return OK(new_float(pow(AS_FLOAT(argv[0])->value, AS_FLOAT(argv[1])->value)));

    return ERR("Expected float argument.");
}

static BuiltinResult _math_hypot(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)
    if (IS_FLOAT(argv[0]) && IS_FLOAT(argv[1]))
        return OK(new_float(hypot(AS_FLOAT(argv[0])->value, AS_FLOAT(argv[1])->value)));

    return ERR("Expected float argument.");
}

COMPLEX_METHOD(_math_complex_conjugate, conj)
COMPLEX_METHOD(_math_complex_projection, cproj)
COMPLEX_METHOD(_math_complex_exp, cexp)
COMPLEX_METHOD(_math_complex_log, clog)
COMPLEX_METHOD(_math_complex_sqrt, csqrt)
COMPLEX_METHOD(_math_complex_sin, csin)
COMPLEX_METHOD(_math_complex_cos, ccos)
COMPLEX_METHOD(_math_complex_tan, ctan)
COMPLEX_METHOD(_math_complex_asin, casin)
COMPLEX_METHOD(_math_complex_acos, cacos)
COMPLEX_METHOD(_math_complex_atan, catan)
COMPLEX_METHOD(_math_complex_sinh, csinh)
COMPLEX_METHOD(_math_complex_cosh, ccosh)
COMPLEX_METHOD(_math_complex_tanh, ctanh)
COMPLEX_METHOD(_math_complex_asinh, casinh)
COMPLEX_METHOD(_math_complex_acosh, cacosh)
COMPLEX_METHOD(_math_complex_atanh, catanh)

static BuiltinResult _math_complex_init(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(2)

    if (!IS_INT(argv[0]) && !IS_FLOAT(argv[0])) {
        char buf[100];
        snprintf(buf, 99, "Expected INT or FLOAT at pos 0 but got %s", OBJ_NAMES[argv[0]->type]);
        return ERR(buf);
    }

    if (!IS_INT(argv[1]) && !IS_FLOAT(argv[1])) {
        char buf[100];
        snprintf(buf, 99, "Expected INT or FLOAT at pos 1 but got %s", OBJ_NAMES[argv[1]->type]);
        return ERR(buf);
    }

    ObjInstance *instance = AS_INSTANCE(caller);

    SET_FIELD("real", argv[0]);
    SET_FIELD("imag", argv[1]);

    return OK(new_nil());
}

static BuiltinResult _math_complex_abs(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInstance *instance = AS_INSTANCE(caller);

    double r, i;
    Obj *field;

    table_get(&instance->fields, AS_OBJ(new_string("real", 4)), &field);
    r = IS_INT(field) ? (double)(AS_INT(field)->value) : AS_FLOAT(field)->value;

    table_get(&instance->fields, AS_OBJ(new_string("imag", 4)), &field);
    i = IS_INT(field) ? (double)(AS_INT(field)->value) : AS_FLOAT(field)->value;

    return OK(new_float(cabs(CMPLX(r, i))));
}

static BuiltinResult _math_complex_arg(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInstance *instance = AS_INSTANCE(caller);

    double r, i;
    Obj *field;

    table_get(&instance->fields, AS_OBJ(new_string("real", 4)), &field);
    r = IS_INT(field) ? (double)(AS_INT(field)->value) : AS_FLOAT(field)->value;

    table_get(&instance->fields, AS_OBJ(new_string("imag", 4)), &field);
    i = IS_INT(field) ? (double)(AS_INT(field)->value) : AS_FLOAT(field)->value;

    return OK(new_float(carg(CMPLX(r, i))));
}

static BuiltinResult _math_complex_pow(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(INSTANCE, 0)

    ObjInstance *y = AS_INSTANCE(argv[0]);

    if (!obj_equal(AS_OBJ(new_string("Complex", 7)), AS_OBJ(y->klass->name))) {
        return ERR("Instance of 'Complex' required.");
    }

    ObjInstance *_instance = AS_INSTANCE(caller);

    double xr, xi, yr, yi;
    Obj *field;

    table_get(&_instance->fields, AS_OBJ(new_string("real", 4)), &field);
    xr = IS_INT(field) ? (double)(AS_INT(field)->value) : AS_FLOAT(field)->value;

    table_get(&_instance->fields, AS_OBJ(new_string("imag", 4)), &field);
    xi = IS_INT(field) ? (double)(AS_INT(field)->value) : AS_FLOAT(field)->value;

    table_get(&y->fields, AS_OBJ(new_string("real", 4)), &field);
    yr = IS_INT(field) ? (double)(AS_INT(field)->value) : AS_FLOAT(field)->value;

    table_get(&y->fields, AS_OBJ(new_string("imag", 4)), &field);
    yi = IS_INT(field) ? (double)(AS_INT(field)->value) : AS_FLOAT(field)->value;

    double complex c = cpow(CMPLX(xr, xi), CMPLX(yr, yi));

    ObjInstance *instance = new_instance(_math_complex_class);

    SET_FLOAT_FIELD("real", creal(c));
    SET_FLOAT_FIELD("imag", cimag(c));

    return OK(instance);
}

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
        SET_BUILTIN_FN_MEMBER("nearbyint", _math_nearbyint);
        SET_BUILTIN_FN_MEMBER("rint", _math_rint);

        SET_FLOAT_MEMBER("NAN", __builtin_nan(""));
        SET_FLOAT_MEMBER("INFINITY", __builtin_inf());

        if (_math_complex_class == NULL) {
            ObjClass *klass = new_class(new_string("Complex", 7));

            SET_BUILTIN_FN_METHOD("init", _math_complex_init);
            SET_BUILTIN_FN_METHOD("abs", _math_complex_abs);
            SET_BUILTIN_FN_METHOD("arg", _math_complex_arg);
            SET_BUILTIN_FN_METHOD("conjugate", _math_complex_conjugate);
            SET_BUILTIN_FN_METHOD("projection", _math_complex_projection);
            SET_BUILTIN_FN_METHOD("exp", _math_complex_exp);
            SET_BUILTIN_FN_METHOD("log", _math_complex_log);
            SET_BUILTIN_FN_METHOD("pow", _math_complex_pow);
            SET_BUILTIN_FN_METHOD("sqrt", _math_complex_sqrt);
            SET_BUILTIN_FN_METHOD("sin", _math_complex_sin);
            SET_BUILTIN_FN_METHOD("cos", _math_complex_cos);
            SET_BUILTIN_FN_METHOD("tan", _math_complex_tan);
            SET_BUILTIN_FN_METHOD("asin", _math_complex_asin);
            SET_BUILTIN_FN_METHOD("acos", _math_complex_acos);
            SET_BUILTIN_FN_METHOD("atan", _math_complex_atan);
            SET_BUILTIN_FN_METHOD("sinh", _math_complex_sinh);
            SET_BUILTIN_FN_METHOD("cosh", _math_complex_cosh);
            SET_BUILTIN_FN_METHOD("tanh", _math_complex_tanh);
            SET_BUILTIN_FN_METHOD("asinh", _math_complex_asinh);
            SET_BUILTIN_FN_METHOD("acosh", _math_complex_acosh);
            SET_BUILTIN_FN_METHOD("atanh", _math_complex_atanh);

            _math_complex_class = klass;
        }

        SET_MEMBER("Complex", _math_complex_class);

        _math_module = module;
    }

    return _math_module;
}
