#include "builtins.h"

BuiltinResult _int_abs(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInt *_int = AS_INT(caller);

    return OK(AS_OBJ(new_int(labs(_int->value))));
}

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

BuiltinMethodTable *int_methods() {
    BuiltinMethodTable *table = malloc(sizeof(BuiltinMethodTable));
    init_method_table(table, 8);

    method_table_set(table, "abs", hash_string("abs", 3), _int_abs);
    method_table_set(table, "to_str", hash_string("to_str", 6), _int_to_str);
    method_table_set(table, "to_hex", hash_string("to_hex", 6), _int_to_hex);
    method_table_set(table, "to_oct", hash_string("to_oct", 6), _int_to_oct);

    return table;
}