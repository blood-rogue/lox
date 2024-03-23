#include "builtins.h"

static NativeResult _list_append(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    ObjList *list = AS_LIST(caller);

    write_array(&list->elems, argv[0]);

    OK(new_nil());
}

static NativeResult _list_len(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    OK(new_int(AS_LIST(caller)->elems.count));
}

static NativeResult _list_remove(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjInt, INT, 0)

    ObjList *list = AS_LIST(caller);
    int64_t index = argv_0->value;

    if (index < 0)
        index = list->elems.count + index;

    if (index >= list->elems.count)
        ERR("Index out of bounds.")

    OK(remove_at_array(&list->elems, index));
}

static NativeResult _list_insert(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjInt, INT, 0)

    ObjList *list = AS_LIST(caller);
    int64_t index = argv_0->value;

    if (index < 0)
        index = list->elems.count + index;

    if (index >= list->elems.count)
        ERR("Index out of bounds.")

    insert_at_array(&list->elems, index, argv[1]);

    OK(new_nil());
}

static NativeResult _list_map(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(1)

    ObjList *mapped = new_list(NULL, 0);
    ObjList *list = AS_LIST(caller);

    for (int i = 0; i < list->elems.count; i++)
        write_array(&mapped->elems, list->elems.values[i]);

    OK(mapped);
}

static ObjClass *_list_class = NULL;

ObjClass *get_list_class() {
    if (_list_class == NULL) {
        printf("here why?\n");
        ObjClass *klass = new_builtin_class("List");

        SET_BUILTIN_FN_METHOD("len", _list_len);
        SET_BUILTIN_FN_METHOD("append", _list_append);
        SET_BUILTIN_FN_METHOD("remove", _list_remove);
        SET_BUILTIN_FN_METHOD("insert", _list_insert);
        SET_BUILTIN_FN_METHOD("map", _list_map);

        _list_class = klass;
    }

    printf("name of list object class = %s\n", _list_class->name->chars);

    return _list_class;
}
