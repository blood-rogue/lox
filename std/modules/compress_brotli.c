#include <brotli/decode.h>
#include <brotli/encode.h>

#include "builtins.h"

static ObjModule *_compress_brotli_module = NULL;

ObjModule *get_compress_brotli_module() {
    if (_compress_brotli_module == NULL) {
        ObjModule *module = new_module(new_string("brotli", 6));

        _compress_brotli_module = module;
    }

    return _compress_brotli_module;
}
