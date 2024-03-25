#include "native.h"

static ObjClass *_module_class = NULL;

ObjClass *get_module_class() {
    if (_module_class == NULL) {
        ObjClass *klass = new_native_class("Module");

        _module_class = klass;
    }

    return _module_class;
}
