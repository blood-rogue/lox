#include <lz4.h>

#include "native.h"

static ObjModule *_compress_lz4_module = NULL;

static NativeResult _compress_lz4_compress(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    ObjBytes *bytes = argv_0;

    int dest_len = LZ4_compressBound(bytes->length);
    char *dest = malloc(dest_len);

    if ((dest_len = LZ4_compress_default((char *)bytes->bytes, dest, bytes->length, dest_len)) == 0)
        ERR("Could not compress data.")

    dest = realloc(dest, dest_len);

    OK(take_bytes((uint8_t *)dest, dest_len));
}

static NativeResult _compress_lz4_decompress(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    ObjBytes *bytes = argv_0;

    int dest_len = bytes->length * 2;
    char *dest = malloc(dest_len);

    if ((dest_len = LZ4_decompress_safe((char *)bytes->bytes, dest, bytes->length, dest_len)) < 0)
        ERR("Could not compress data.")

    dest = realloc(dest, dest_len);

    OK(take_bytes((uint8_t *)dest, dest_len));
}

ObjModule *get_compress_lz4_module() {
    if (_compress_lz4_module == NULL) {
        ObjModule *module = new_module("lz4");

        SET_NATIVE_FN_MEMBER("compress", _compress_lz4_compress);
        SET_NATIVE_FN_MEMBER("decompress", _compress_lz4_decompress);

        _compress_lz4_module = module;
    }

    return _compress_lz4_module;
}
