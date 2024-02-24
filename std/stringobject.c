#include <ctype.h>

#include "builtins.h"

BuiltinResult _string_to_lower(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    ObjString *_caller = AS_STRING(caller);
    ObjString *str = new_string(_caller->chars, _caller->length);

    for (int i = 0; i < _caller->length; i++) {
        str->chars[i] = (char)tolower(str->chars[i]);
    }

    return OK(str);
}

BuiltinResult _string_to_upper(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    ObjString *_caller = AS_STRING(caller);
    ObjString *str = new_string(_caller->chars, _caller->length);

    for (int i = 0; i < _caller->length; i++) {
        str->chars[i] = (char)toupper(str->chars[i]);
    }

    return OK(str);
}

BuiltinResult _string_len(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(new_int(AS_STRING(caller)->length));
}

BuiltinMethodTable *string_methods() {
    BuiltinMethodTable *table = malloc(sizeof(BuiltinMethodTable));
    init_method_table(table, 8);

    SET_BLTIN_METHOD(string, to_upper);
    SET_BLTIN_METHOD(string, to_lower);
    SET_BLTIN_METHOD(string, len);

    return table;
}
