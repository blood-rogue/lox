#include "native.h"

static ObjClass *_native_function_class = NULL;

ObjClass *get_native_function_class() {
    if (_native_function_class == NULL) {
        ObjClass *klass = new_native_class("NativeStruct");

        _native_function_class = klass;
    }

    return _native_function_class;
}
