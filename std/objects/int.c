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

NativeTable *int_methods() {
    NativeTable *table = malloc(sizeof(NativeTable));
    init_method_table(table, 16);

    SET_BLTIN_METHOD("to_str", _int_to_str);
    SET_BLTIN_METHOD("to_hex", _int_to_hex);
    SET_BLTIN_METHOD("to_oct", _int_to_oct);
    SET_BLTIN_METHOD("abs", _int_abs);
    SET_BLTIN_METHOD("num_ones", _int_num_ones);
    SET_BLTIN_METHOD("num_zeroes", _int_num_zeroes);
    SET_BLTIN_METHOD("parity", _int_parity);
    SET_BLTIN_METHOD("leading_zeroes", _int_leading_zeroes);
    SET_BLTIN_METHOD("trailing_zeroes", _int_trailing_zeroes);

    return table;
}
