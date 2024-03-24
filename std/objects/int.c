#include "builtins.h"

static NativeResult _int_new(int argc, UNUSED(Obj **argv), UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(0)
    OK(new_int_i(0));
}

static ObjClass *_int_class = NULL;

ObjClass *get_int_class() {
    if (_int_class == NULL) {
        ObjClass *klass = new_builtin_class("Int");

        SET_BUILTIN_FN_STATIC("__new", _int_new);

        _int_class = klass;
    }

    return _int_class;
}
