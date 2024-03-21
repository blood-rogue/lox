#include <snappy-c.h>

#include "builtins.h"

static ObjModule *_compress_snappy_module = NULL;

static NativeResult _compress_snappy_compress(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    size_t dest_len = snappy_max_compressed_length(argv_0->length);
    char *dest = malloc(dest_len);

    if (snappy_compress((char *)argv_0->bytes, argv_0->length, dest, &dest_len) != SNAPPY_OK)
        ERR("Could not compress data.")

    dest = realloc(dest, dest_len);
    OK(take_bytes((uint8_t *)dest, dest_len));
}

static NativeResult _compress_snappy_decompress(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    if (snappy_validate_compressed_buffer((char *)argv_0->bytes, argv_0->length) != SNAPPY_OK)
        ERR("Invalid buffer")

    size_t dest_len;
    if (snappy_uncompressed_length((char *)argv_0->bytes, argv_0->length, &dest_len) != SNAPPY_OK)
        ERR("Invalid buffer")

    char *dest = malloc(dest_len);
    if (snappy_uncompress((char *)argv_0->bytes, argv_0->length, dest, &dest_len) != SNAPPY_OK)
        ERR("Could not decompress data.")

    OK(take_bytes((uint8_t *)dest, dest_len));
}

ObjModule *get_compress_snappy_module() {
    if (_compress_snappy_module == NULL) {
        ObjModule *module = new_module("snappy");

        SET_BUILTIN_FN_MEMBER("compress", _compress_snappy_compress);
        SET_BUILTIN_FN_MEMBER("decompress", _compress_snappy_decompress);

        _compress_snappy_module = module;
    }

    return _compress_snappy_module;
}
