#include "builtins.h"

BuiltinResult _list_append(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    ObjList *list = AS_LIST(caller);

    write_array(&list->elems, argv[0]);

    return OK(new_nil());
}

BuiltinResult _list_len(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(new_int(AS_LIST(caller)->elems.count));
}

BuiltinTable *list_methods() {
    BuiltinTable *table = malloc(sizeof(BuiltinTable));
    init_method_table(table, 8);

    SET_BLTIN_METHOD(list, append);
    SET_BLTIN_METHOD(list, len);

    return table;
}
