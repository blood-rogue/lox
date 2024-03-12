#include <liblzf/lzf.h>

#include "builtins.h"

static ObjModule *_compress_lzf_module = NULL;

static BuiltinResult _compress_lzf_compress(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(BYTES, 0)

    ObjBytes *bytes = AS_BYTES(argv[0]);

    size_t dest_len = bytes->length * 2;
    char *dest = malloc(dest_len);

    if ((dest_len = lzf_compress(bytes->bytes, bytes->length, dest, dest_len)) == 0)
        return ERR("Could not compress data.");

    dest = realloc(dest, dest_len);

    return OK(take_bytes((uint8_t *)dest, dest_len));
}

static BuiltinResult _compress_lzf_decompress(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(BYTES, 0)

    ObjBytes *bytes = AS_BYTES(argv[0]);

    size_t dest_len = bytes->length * 2;
    char *dest = malloc(dest_len);

    if ((dest_len = lzf_decompress(bytes->bytes, bytes->length, dest, dest_len)) == 0)
        return ERR("Could not decompress data.");

    dest = realloc(dest, dest_len);

    return OK(take_bytes((uint8_t *)dest, dest_len));
}

ObjModule *get_compress_lzf_module() {
    if (_compress_lzf_module == NULL) {
        ObjModule *module = new_module(new_string("lzf", 3));

        SET_BUILTIN_FN_MEMBER("compress", _compress_lzf_compress);
        SET_BUILTIN_FN_MEMBER("decompress", _compress_lzf_decompress);

        _compress_lzf_module = module;
    }

    return _compress_lzf_module;
}
