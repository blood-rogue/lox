#include "native.h"

static ObjClass *_upvalue_class = NULL;

ObjClass *get_upvalue_class() {
    if (_upvalue_class == NULL) {
        ObjClass *klass = new_native_class("Upvalue");

        _upvalue_class = klass;
    }

    return _upvalue_class;
}
