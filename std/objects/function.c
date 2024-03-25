#include "native.h"

static ObjClass *_function_class = NULL;

ObjClass *get_function_class() {
    if (_function_class == NULL) {
        ObjClass *klass = new_native_class("Function");

        _function_class = klass;
    }

    return _function_class;
}
