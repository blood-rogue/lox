#include <zlib.h>

#include "builtins.h"

static ObjModule *_compress_zlib_module = NULL;

static BuiltinResult _compress_zlib_compress(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(BYTES, 0)
    CHECK_ARG_TYPE(INT, 1)

    ObjBytes *bytes = AS_BYTES(argv[0]);

    uLong dest_len = compressBound(bytes->length);
    Bytef *dest = malloc(dest_len);

    if (compress2(dest, &dest_len, bytes->bytes, bytes->length, AS_INT(argv[1])->value) != Z_OK)
        return ERR("Could not compress data.");

    dest = realloc(dest, dest_len);

    return OK(take_bytes(dest, dest_len));
}

static BuiltinResult _compress_zlib_decompress(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(BYTES, 0)

    ObjBytes *bytes = AS_BYTES(argv[0]);
    uLong dest_len = 2 * bytes->length;
    Bytef *dest = malloc(2 * bytes->length);

    if (uncompress(dest, &dest_len, bytes->bytes, bytes->length) != Z_OK)
        return ERR("Could not decompress data.");

    dest = realloc(dest, dest_len);

    return OK(take_bytes(dest, dest_len));
}

ObjModule *get_compress_zlib_module() {
    if (_compress_zlib_module == NULL) {
        ObjModule *module = new_module(new_string("zlib", 4));

        SET_BUILTIN_FN_MEMBER("compress", _compress_zlib_compress);
        SET_BUILTIN_FN_MEMBER("decompress", _compress_zlib_decompress);

        _compress_zlib_module = module;
    }

    return _compress_zlib_module;
}
