#include "native.h"

static ObjClass *_native_struct_class = NULL;

ObjClass *get_native_struct_class() {
    if (_native_struct_class == NULL) {
        ObjClass *klass = new_native_class("NativeStruct");

        _native_struct_class = klass;
    }

    return _native_struct_class;
}
