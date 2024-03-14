#include <brotli/decode.h>
#include <brotli/encode.h>

#include "builtins.h"

static ObjModule *_compress_brotli_module = NULL;

static BuiltinResult _compress_brotli_compress(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return OK(argv_0);
}

ObjModule *get_compress_brotli_module() {
    if (_compress_brotli_module == NULL) {
        ObjModule *module = new_module(new_string("brotli", 6));

        SET_BUILTIN_FN_MEMBER("compress", _compress_brotli_compress);

        _compress_brotli_module = module;
    }

    return _compress_brotli_module;
}
