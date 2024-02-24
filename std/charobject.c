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

BuiltinResult
_char_is_punctuation(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(new_bool(ispunct(AS_CHAR(caller)->value)));
}

BuiltinResult _char_is_alnum(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(new_bool(isalnum(AS_CHAR(caller)->value)));
}

BuiltinResult _char_to_lower(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(new_char((uint8_t)tolower(AS_CHAR(caller)->value)));
}

BuiltinResult _char_to_upper(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(new_char((uint8_t)toupper(AS_CHAR(caller)->value)));
}

BuiltinMethodTable *char_methods() {
    BuiltinMethodTable *table = malloc(sizeof(BuiltinMethodTable));
    init_method_table(table, 16);

    SET_BLTIN_METHOD(char, is_upper);
    SET_BLTIN_METHOD(char, is_lower);
    SET_BLTIN_METHOD(char, is_alpha);
    SET_BLTIN_METHOD(char, is_digit);
    SET_BLTIN_METHOD(char, is_hex_digit);
    SET_BLTIN_METHOD(char, is_space);
    SET_BLTIN_METHOD(char, is_printable);
    SET_BLTIN_METHOD(char, is_graphical);
    SET_BLTIN_METHOD(char, is_blank);
    SET_BLTIN_METHOD(char, is_control);
    SET_BLTIN_METHOD(char, is_punctuation);
    SET_BLTIN_METHOD(char, is_alnum);
    SET_BLTIN_METHOD(char, to_lower);
    SET_BLTIN_METHOD(char, to_upper);

    return table;
}
