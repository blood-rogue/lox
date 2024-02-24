#include "builtins.h"

BuiltinResult _float_abs(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    ObjFloat *_float = AS_FLOAT(caller);
    return OK(AS_OBJ(new_float(fabs(_float->value))));
}

BuiltinResult _float_to_str(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjFloat *_float = AS_FLOAT(caller);

    int size = snprintf(NULL, 0, "%f", _float->value) + 1;
    char *buf = malloc(size);
    snprintf(buf, size, "%f", _float->value);

    return OK(AS_OBJ(take_string(buf, (int)strlen(buf))));
}

BuiltinResult _float_ceil(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    ObjFloat *_float = AS_FLOAT(caller);
    return OK(AS_OBJ(new_float(ceil(_float->value))));
}

BuiltinResult _float_floor(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    ObjFloat *_float = AS_FLOAT(caller);
    return OK(AS_OBJ(new_float(floor(_float->value))));
}

BuiltinMethodTable *float_methods() {
    BuiltinMethodTable *table = malloc(sizeof(BuiltinMethodTable));
    init_method_table(table, 8);

    method_table_set(table, "abs", hash_string("abs", 3), _float_abs);
    method_table_set(table, "to_str", hash_string("to_str", 6), _float_to_str);
    method_table_set(table, "ceil", hash_string("ceil", 4), _float_ceil);
    method_table_set(table, "floor", hash_string("floor", 5), _float_floor);

    return table;
}