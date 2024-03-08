#include <unicase.h>

#include "builtins.h"

BuiltinResult _string_to_upper(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    ObjString *_caller = AS_STRING(caller);

    size_t length;
    uint8_t *chars =
        u8_toupper((uint8_t *)_caller->chars, _caller->length, NULL, NULL, NULL, &length);

    ObjString *str = take_string((char *)chars, length);

    return OK(str);
}

BuiltinResult _string_to_lower(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    ObjString *_caller = AS_STRING(caller);

    size_t length;
    uint8_t *chars =
        u8_tolower((uint8_t *)_caller->chars, _caller->length, NULL, NULL, NULL, &length);

    ObjString *str = take_string((char *)chars, length);

    return OK(str);
}

BuiltinResult _string_to_title(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    ObjString *_caller = AS_STRING(caller);

    size_t length;
    uint8_t *chars =
        u8_totitle((uint8_t *)_caller->chars, _caller->length, NULL, NULL, NULL, &length);

    ObjString *str = take_string((char *)chars, length);

    return OK(str);
}

BuiltinResult _string_len(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(new_int(AS_STRING(caller)->length));
}

BuiltinTable *string_methods() {
    BuiltinTable *table = malloc(sizeof(BuiltinTable));
    init_method_table(table, 8);

    SET_BLTIN_METHOD(string, to_upper);
    SET_BLTIN_METHOD(string, to_lower);
    SET_BLTIN_METHOD(string, to_title);
    SET_BLTIN_METHOD(string, len);

    return table;
}
