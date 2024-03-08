#include <unicase.h>
#include <unictype.h>

#include "builtins.h"

#define IS_PROPERTY_FN(name)                                                                       \
    BuiltinResult _char_is_##name(int argc, UNUSED(Obj **, argv), Obj *caller) {                   \
        CHECK_ARG_COUNT(0)                                                                         \
        return OK(new_bool(uc_is_property_##name(AS_CHAR(caller)->value)));                        \
    }

IS_PROPERTY_FN(alphabetic)

BuiltinResult _char_to_upper(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(take_char(uc_toupper(AS_CHAR(caller)->value)));
}

BuiltinResult _char_to_lower(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(take_char(uc_tolower(AS_CHAR(caller)->value)));
}

BuiltinResult _char_to_title(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(take_char(uc_totitle(AS_CHAR(caller)->value)));
}

BuiltinTable *char_methods() {
    BuiltinTable *table = malloc(sizeof(BuiltinTable));
    init_method_table(table, 16);

    SET_BLTIN_METHOD(char, is_alphabetic);
    SET_BLTIN_METHOD(char, to_upper);
    SET_BLTIN_METHOD(char, to_lower);
    SET_BLTIN_METHOD(char, to_title);

    return table;
}
