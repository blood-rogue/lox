#include "native.h"

static ObjClass *_closure_class = NULL;

ObjClass *get_closure_class() {
    if (_closure_class == NULL) {
        ObjClass *klass = new_native_class("Closure");

        _closure_class = klass;
    }

    return _closure_class;
}
