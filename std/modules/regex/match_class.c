#include "native.h"

static ObjClass *_regex_match_class = NULL;

static NativeResult _regex_match_group(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjInt, INT, 0)

    ObjInstance *instance = AS_INSTANCE(caller);

    Obj *field;
    table_get(&instance->fields, AS_OBJ(new_string("groups", 6)), &field);

    ObjList *groups = AS_LIST(field);

    int64_t index = mpz_get_si(argv_0->value);
    if (index < 0)
        index = groups->elems.count + index;

    if (index >= groups->elems.count)
        ERR("Index out of bounds.")

    Obj *indexed = groups->elems.values[index];

    OK(indexed);
}

ObjClass *get_regex_match_class() {
    if (_regex_match_class == NULL) {
        ObjClass *klass = new_native_class("Match");

        SET_NATIVE_FN_METHOD("group", _regex_match_group);

        _regex_match_class = klass;
    }

    return _regex_match_class;
}
