#include "native.h"

static ObjClass *_instance_class = NULL;

ObjClass *get_instance_class() {
    if (_instance_class == NULL) {
        ObjClass *klass = new_native_class("Instance");

        _instance_class = klass;
    }

    return _instance_class;
}
