#include "builtins.h"

BuiltinResult _int_to_str(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInt *_int = AS_INT(caller);

    int size = snprintf(NULL, 0, "%ld", _int->value) + 1;
    char *buf = malloc(size);
    snprintf(buf, size, "%ld", _int->value);

    return OK(AS_OBJ(take_string(buf, (int)strlen(buf))));
}

BuiltinResult _int_to_hex(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInt *_int = AS_INT(caller);

    int size = snprintf(NULL, 0, "0x%lx", _int->value) + 1;
    char *buf = malloc(size);
    snprintf(buf, size, "0x%lx", _int->value);

    return OK(AS_OBJ(take_string(buf, (int)strlen(buf))));
}

BuiltinResult _int_to_oct(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInt *_int = AS_INT(caller);

    int size = snprintf(NULL, 0, "0o%lo", _int->value) + 1;
    char *buf = malloc(size);
    snprintf(buf, size, "0o%lo", _int->value);

    return OK(AS_OBJ(take_string(buf, (int)strlen(buf))));
}

BuiltinTable *int_methods() {
    BuiltinTable *table = malloc(sizeof(BuiltinTable));
    init_method_table(table, 4);

    SET_BLTIN_METHOD("to_str", _int_to_str);
    SET_BLTIN_METHOD("to_hex", _int_to_hex);
    SET_BLTIN_METHOD("to_oct", _int_to_oct);

    return table;
}
