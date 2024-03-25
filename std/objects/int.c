#include "native.h"

static NativeResult _int_new(int argc, UNUSED(Obj **argv), UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(0)
    OK(new_int_i(0));
}

static NativeResult _int_eq(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjInt, INT, 0)
    CHECK_ARG_TYPE(ObjInt, INT, 1)

    OK(new_bool(mpz_cmp(argv_0->value, argv_1->value) == 0));
}

static NativeResult _int_init(int argc, Obj **argv, UNUSED(Obj *caller)) {
    if (argc == 0) {
    } else if (argc == 1) {
        switch (argv[0]->type) {
            case OBJ_INT:
                break;
            default:
                ERR("Cannot parse '%s' into Int.", get_obj_kind(argv[0]));
        }
    } else
        ERR("Expected 0 or 1 argument(s) but got %d", argc)
    OK(new_nil());
}

static NativeResult _int_add(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjInt, INT, 0)
    CHECK_ARG_TYPE(ObjInt, INT, 1)

    mpz_t r;
    mpz_init_set_si(r, 0);
    mpz_add(r, argv_0->value, argv_1->value);

    OK(new_int(r));
}

static NativeResult _int_sub(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjInt, INT, 0)
    CHECK_ARG_TYPE(ObjInt, INT, 1)

    mpz_t r;
    mpz_init_set_si(r, 0);
    mpz_sub(r, argv_0->value, argv_1->value);

    OK(new_int(r));
}

static NativeResult _int_mul(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjInt, INT, 0)
    CHECK_ARG_TYPE(ObjInt, INT, 1)

    mpz_t r;
    mpz_init_set_si(r, 0);
    mpz_mul(r, argv_0->value, argv_1->value);

    OK(new_int(r));
}

static NativeResult _int_div(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjInt, INT, 0)
    CHECK_ARG_TYPE(ObjInt, INT, 1)

    mpz_t r;
    mpz_init_set_si(r, 0);
    mpz_div(r, argv_0->value, argv_1->value);

    OK(new_int(r));
}

static NativeResult _int_neg(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjInt, INT, 0)

    mpz_t r;
    mpz_init_set_si(r, 0);
    mpz_neg(r, argv_0->value);

    OK(new_int(r));
}

static NativeResult _int_lt(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjInt, INT, 0)
    CHECK_ARG_TYPE(ObjInt, INT, 1)

    OK(new_bool(mpz_cmp(argv_0->value, argv_1->value) < 0));
}

static NativeResult _int_gt(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjInt, INT, 0)
    CHECK_ARG_TYPE(ObjInt, INT, 1)

    OK(new_bool(mpz_cmp(argv_0->value, argv_1->value) > 0));
}

static NativeResult _int_bitshr(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjInt, INT, 0)
    CHECK_ARG_TYPE(ObjInt, INT, 1)

    mpz_t r;
    mpz_init_set_si(r, 0);
    mpz_fdiv_q_2exp(r, argv_0->value, mpz_get_ui(argv_1->value));

    OK(new_int(r));
}

static NativeResult _int_bitshl(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjInt, INT, 0)
    CHECK_ARG_TYPE(ObjInt, INT, 1)

    mpz_t r;
    mpz_init_set_si(r, 0);
    mpz_mul_2exp(r, argv_0->value, mpz_get_ui(argv_1->value));

    OK(new_int(r));
}

static NativeResult _int_bitand(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjInt, INT, 0)
    CHECK_ARG_TYPE(ObjInt, INT, 1)

    mpz_t r;
    mpz_init_set_si(r, 0);
    mpz_and(r, argv_0->value, argv_1->value);

    OK(new_int(r));
}

static NativeResult _int_bitor(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjInt, INT, 0)
    CHECK_ARG_TYPE(ObjInt, INT, 1)

    mpz_t r;
    mpz_init_set_si(r, 0);
    mpz_ior(r, argv_0->value, argv_1->value);

    OK(new_int(r));
}

static NativeResult _int_bitxor(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjInt, INT, 0)
    CHECK_ARG_TYPE(ObjInt, INT, 1)

    mpz_t r;
    mpz_init_set_si(r, 0);
    mpz_xor(r, argv_0->value, argv_1->value);

    OK(new_int(r));
}

static NativeResult _int_bitcomp(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjInt, INT, 0)

    mpz_t r;
    mpz_init_set_si(r, 0);
    mpz_com(r, argv_0->value);

    OK(new_int(r));
}

static ObjClass *_int_class = NULL;

ObjClass *get_int_class() {
    if (_int_class == NULL) {
        ObjClass *klass = new_native_class("Int");

        SET_NATIVE_FN_STATIC("__init", _int_init);
        SET_NATIVE_FN_STATIC("__new", _int_new);
        SET_NATIVE_FN_STATIC("__add", _int_add);
        SET_NATIVE_FN_STATIC("__sub", _int_sub);
        SET_NATIVE_FN_STATIC("__mul", _int_mul);
        SET_NATIVE_FN_STATIC("__div", _int_div);
        SET_NATIVE_FN_STATIC("__eq", _int_eq);
        SET_NATIVE_FN_STATIC("__neg", _int_neg);
        SET_NATIVE_FN_STATIC("__lt", _int_lt);
        SET_NATIVE_FN_STATIC("__gt", _int_gt);
        SET_NATIVE_FN_STATIC("__bitshr", _int_bitshr);
        SET_NATIVE_FN_STATIC("__bitshl", _int_bitshl);
        SET_NATIVE_FN_STATIC("__bitand", _int_bitand);
        SET_NATIVE_FN_STATIC("__bitor", _int_bitor);
        SET_NATIVE_FN_STATIC("__bitxor", _int_bitxor);
        SET_NATIVE_FN_STATIC("__bitcomp", _int_bitcomp);

        _int_class = klass;
    }

    return _int_class;
}
