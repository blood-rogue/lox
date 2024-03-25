#include "native.h"

static ObjClass *_nil_class = NULL;

ObjClass *get_nil_class() {
    if (_nil_class == NULL) {
        ObjClass *klass = new_native_class("Nil");

        _nil_class = klass;
    }

    return _nil_class;
}
