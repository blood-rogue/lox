#include <ctype.h>

#include "builtins.h"

BuiltinResult _char_is_upper(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(new_bool(isupper(AS_CHAR(caller)->value)));
}

BuiltinResult _char_is_lower(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(new_bool(islower(AS_CHAR(caller)->value)));
}

BuiltinResult _char_is_alpha(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(new_bool(isalpha(AS_CHAR(caller)->value)));
}

BuiltinResult _char_is_digit(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(new_bool(isdigit(AS_CHAR(caller)->value)));
}

BuiltinResult _char_is_hex_digit(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(new_bool(isxdigit(AS_CHAR(caller)->value)));
}

BuiltinResult _char_is_space(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(new_bool(isspace(AS_CHAR(caller)->value)));
}

BuiltinResult _char_is_printable(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(new_bool(isprint(AS_CHAR(caller)->value)));
}

BuiltinResult _char_is_graphical(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(new_bool(isgraph(AS_CHAR(caller)->value)));
}

BuiltinResult _char_is_blank(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(new_bool(isblank(AS_CHAR(caller)->value)));
}

BuiltinResult _char_is_control(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(new_bool(iscntrl(AS_CHAR(caller)->value)));
}

BuiltinResult _char_is_punctuation(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(new_bool(ispunct(AS_CHAR(caller)->value)));
}

BuiltinResult _char_is_alnum(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(new_bool(isalnum(AS_CHAR(caller)->value)));
}

BuiltinMethodTable *char_methods() {
    BuiltinMethodTable *table = malloc(sizeof(BuiltinMethodTable));
    init_method_table(table, 16);

    SET_BLTIN_METHOD(char, is_upper, 8);
    SET_BLTIN_METHOD(char, is_lower, 8);
    SET_BLTIN_METHOD(char, is_alpha, 8);
    SET_BLTIN_METHOD(char, is_digit, 8);
    SET_BLTIN_METHOD(char, is_hex_digit, 12);
    SET_BLTIN_METHOD(char, is_space, 8);
    SET_BLTIN_METHOD(char, is_printable, 12);
    SET_BLTIN_METHOD(char, is_graphical, 12);
    SET_BLTIN_METHOD(char, is_blank, 8);
    SET_BLTIN_METHOD(char, is_control, 10);
    SET_BLTIN_METHOD(char, is_punctuation, 14);
    SET_BLTIN_METHOD(char, is_alnum, 8);

    return table;
}