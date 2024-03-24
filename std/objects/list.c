#include "native.h"

static NativeResult _list_append(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    ObjList *list = AS_LIST(caller);

    write_array(&list->elems, argv[0]);

    OK(new_nil());
}

static NativeResult _list_len(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    OK(new_int_i(AS_LIST(caller)->elems.count));
}

static NativeResult _list_remove(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjInt, INT, 0)

    ObjList *list = AS_LIST(caller);
    int64_t index = mpz_get_si(argv_0->value);

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
    int64_t index = mpz_get_si(argv_0->value);

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

static NativeResult _list_new(int argc, UNUSED(Obj **argv), UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(0)
    OK(new_list(NULL, 0));
}

static ObjClass *_list_class = NULL;

ObjClass *get_list_class() {
    if (_list_class == NULL) {
        ObjClass *klass = new_native_class("List");

        SET_NATIVE_FN_STATIC("__new", _list_new);

        SET_NATIVE_FN_METHOD("len", _list_len);
        SET_NATIVE_FN_METHOD("append", _list_append);
        SET_NATIVE_FN_METHOD("remove", _list_remove);
        SET_NATIVE_FN_METHOD("insert", _list_insert);
        SET_NATIVE_FN_METHOD("map", _list_map);

        _list_class = klass;
    }

    return _list_class;
}
