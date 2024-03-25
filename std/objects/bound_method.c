#include "native.h"

static ObjClass *_bound_method_class = NULL;

ObjClass *get_bound_method_class() {
    if (_bound_method_class == NULL) {
        ObjClass *klass = new_native_class("BoundMethod");

        _bound_method_class = klass;
    }

    return _bound_method_class;
}
