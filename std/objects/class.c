#include "native.h"

static ObjClass *_class_class = NULL;

ObjClass *get_class_class() {
    if (_class_class == NULL) {
        ObjClass *klass = new_native_class("Class");

        _class_class = klass;
    }

    return _class_class;
}
