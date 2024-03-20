#include <unicase.h>
#include <unistr.h>

#include "builtins.h"

static BuiltinResult _string_to_upper(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    ObjString *_caller = AS_STRING(caller);

    size_t length;
    uint8_t *chars =
        u8_toupper((uint8_t *)_caller->chars, _caller->raw_length, NULL, NULL, NULL, &length);

    ObjString *str = take_string((char *)chars, length);

    OK(str);
}

static BuiltinResult _string_to_lower(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    ObjString *_caller = AS_STRING(caller);

    size_t length;
    uint8_t *chars =
        u8_tolower((uint8_t *)_caller->chars, _caller->raw_length, NULL, NULL, NULL, &length);

    ObjString *str = take_string((char *)chars, length);

    OK(str);
}

static BuiltinResult _string_to_title(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    ObjString *_caller = AS_STRING(caller);

    size_t length;
    uint8_t *chars =
        u8_totitle((uint8_t *)_caller->chars, _caller->raw_length, NULL, NULL, NULL, &length);

    ObjString *str = take_string((char *)chars, length);

    OK(str);
}

static BuiltinResult _string_len(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    OK(new_int(AS_STRING(caller)->length));
}

static BuiltinResult _string_find(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjChar, CHAR, 0)

    ucs4_t puc = 0;
    int idx = -1;
    for (uint8_t *tmp = (uint8_t *)AS_STRING(caller)->chars; tmp != NULL && puc != argv_0->value;
         tmp = (uint8_t *)u8_next(&puc, tmp))
        idx++;

    OK(puc == argv_0->value ? AS_OBJ(new_int(idx)) : AS_OBJ(new_nil()));
}

BuiltinTable *string_methods() {
    BuiltinTable *table = malloc(sizeof(BuiltinTable));
    init_method_table(table, 8);

    SET_BLTIN_METHOD("to_upper", _string_to_upper);
    SET_BLTIN_METHOD("to_lower", _string_to_lower);
    SET_BLTIN_METHOD("to_title", _string_to_title);
    SET_BLTIN_METHOD("len", _string_len);
    SET_BLTIN_METHOD("find", _string_find);

    return table;
}
