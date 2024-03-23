#include "builtins.h"

static NativeResult _int_to_str(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInt *_int = AS_INT(caller);

    int size = snprintf(NULL, 0, "%ld", _int->value) + 1;
    char *buf = malloc(size);
    snprintf(buf, size, "%ld", _int->value);

    OK(AS_OBJ(take_string(buf, (int)strlen(buf))));
}

static NativeResult _int_to_hex(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInt *_int = AS_INT(caller);

    int size = snprintf(NULL, 0, "0x%lx", _int->value) + 1;
    char *buf = malloc(size);
    snprintf(buf, size, "0x%lx", _int->value);

    OK(AS_OBJ(take_string(buf, (int)strlen(buf))));
}

static NativeResult _int_to_oct(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInt *_int = AS_INT(caller);

    int size = snprintf(NULL, 0, "0o%lo", _int->value) + 1;
    char *buf = malloc(size);
    snprintf(buf, size, "0o%lo", _int->value);

    OK(AS_OBJ(take_string(buf, (int)strlen(buf))));
}

static NativeResult _int_abs(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    OK(new_int(labs(AS_INT(caller)->value)));
}

static NativeResult _int_num_zeroes(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    OK(new_int(__builtin_popcountl(~AS_INT(caller)->value)));
}

static NativeResult _int_num_ones(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    OK(new_int(__builtin_popcountl(AS_INT(caller)->value)));
}

static NativeResult _int_parity(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    OK(new_int(__builtin_parityl(AS_INT(caller)->value)));
}

static NativeResult _int_leading_zeroes(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    OK(new_int(__builtin_clzl(AS_INT(caller)->value)));
}

static NativeResult _int_trailing_zeroes(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    OK(new_int(__builtin_ctzl(AS_INT(caller)->value)));
}

static ObjClass *_int_class = NULL;

ObjClass *get_int_class() {
    if (_int_class == NULL) {
        ObjClass *klass = new_builtin_class("Int");

        SET_BUILTIN_FN_METHOD("to_str", _int_to_str);
        SET_BUILTIN_FN_METHOD("to_hex", _int_to_hex);
        SET_BUILTIN_FN_METHOD("to_oct", _int_to_oct);
        SET_BUILTIN_FN_METHOD("abs", _int_abs);
        SET_BUILTIN_FN_METHOD("num_ones", _int_num_ones);
        SET_BUILTIN_FN_METHOD("num_zeroes", _int_num_zeroes);
        SET_BUILTIN_FN_METHOD("parity", _int_parity);
        SET_BUILTIN_FN_METHOD("leading_zeroes", _int_leading_zeroes);
        SET_BUILTIN_FN_METHOD("trailing_zeroes", _int_trailing_zeroes);

        printf("%p", (void *)klass->methods.entries);

        _int_class = klass;
    }

    return _int_class;
}
