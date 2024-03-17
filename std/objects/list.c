#include "builtins.h"

static BuiltinResult _list_append(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    ObjList *list = AS_LIST(caller);

    write_array(&list->elems, argv[0]);

    return OK(new_nil());
}

static BuiltinResult _list_len(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(new_int(AS_LIST(caller)->elems.count));
}

static BuiltinResult _list_remove(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjInt, INT, 0)

    ObjList *list = AS_LIST(caller);
    int64_t index = argv_0->value;

    if (index < 0)
        index = list->elems.count + index;

    if (index >= list->elems.count) {
        return ERR("Index out of bounds.");
    }

    return OK(remove_at_array(&list->elems, index));
}

static BuiltinResult _list_insert(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjInt, INT, 0)

    ObjList *list = AS_LIST(caller);
    int64_t index = argv_0->value;

    if (index < 0)
        index = list->elems.count + index;

    if (index >= list->elems.count) {
        return ERR("Index out of bounds.");
    }

    insert_at_array(&list->elems, index, argv[1]);

    return OK(new_nil());
}

static BuiltinResult _list_map(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(1)

    ObjList *mapped = new_list(NULL, 0);
    ObjList *list = AS_LIST(caller);

    for (int i = 0; i < list->elems.count; i++)
        write_array(&mapped->elems, list->elems.values[i]);

    return OK(mapped);
}

BuiltinTable *list_methods() {
    BuiltinTable *table = malloc(sizeof(BuiltinTable));
    init_method_table(table, 8);

    SET_BLTIN_METHOD("append", _list_append);
    SET_BLTIN_METHOD("len", _list_len);
    SET_BLTIN_METHOD("remove", _list_remove);
    SET_BLTIN_METHOD("insert", _list_insert);
    SET_BLTIN_METHOD("map", _list_map);

    return table;
}
