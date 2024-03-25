#include "native.h"

static ObjClass *_bool_class = NULL;

ObjClass *get_bool_class() {
    if (_bool_class == NULL) {
        ObjClass *klass = new_native_class("Bool");

        _bool_class = klass;
    }

    return _bool_class;
}
