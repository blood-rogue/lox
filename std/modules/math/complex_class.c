#include <complex.h>

#include "builtins.h"

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

static ObjClass *_math_complex_class = NULL;

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
        snprintf(buf, 99, "Expected INT or FLOAT at pos 0 but got %s", get_obj_kind(argv[0]));
        return ERR(buf);
    }

    if (!IS_INT(argv[1]) && !IS_FLOAT(argv[1])) {
        char buf[100];
        snprintf(buf, 99, "Expected INT or FLOAT at pos 1 but got %s", get_obj_kind(argv[1]));
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
    CHECK_ARG_TYPE(ObjInstance, INSTANCE, 0)

    if (!obj_equal(AS_OBJ(new_string("Complex", 7)), AS_OBJ(argv_0->klass->name))) {
        return ERR("Instance of 'Complex' required.");
    }

    ObjInstance *_instance = AS_INSTANCE(caller);

    double xr, xi, yr, yi;
    Obj *field;

    table_get(&_instance->fields, AS_OBJ(new_string("real", 4)), &field);
    xr = IS_INT(field) ? (double)(AS_INT(field)->value) : AS_FLOAT(field)->value;

    table_get(&_instance->fields, AS_OBJ(new_string("imag", 4)), &field);
    xi = IS_INT(field) ? (double)(AS_INT(field)->value) : AS_FLOAT(field)->value;

    table_get(&argv_0->fields, AS_OBJ(new_string("real", 4)), &field);
    yr = IS_INT(field) ? (double)(AS_INT(field)->value) : AS_FLOAT(field)->value;

    table_get(&argv_0->fields, AS_OBJ(new_string("imag", 4)), &field);
    yi = IS_INT(field) ? (double)(AS_INT(field)->value) : AS_FLOAT(field)->value;

    double complex c = cpow(CMPLX(xr, xi), CMPLX(yr, yi));

    ObjInstance *instance = new_instance(_math_complex_class);

    SET_FLOAT_FIELD("real", creal(c));
    SET_FLOAT_FIELD("imag", cimag(c));

    return OK(instance);
}

ObjClass *get_math_complex_class() {
    if (_math_complex_class == NULL) {
        ObjClass *klass = new_class(new_string("Complex", 7));
        klass->is_builtin = true;

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

    return _math_complex_class;
}
