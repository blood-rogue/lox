#include <zlib.h>

#include "builtins.h"

static ObjModule *_compress_module = NULL;
static ObjModule *_compress_zlib_module = NULL;

static BuiltinResult _compress_zlib_compress(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(BYTES, 0)
    CHECK_ARG_TYPE(INT, 1)

    z_stream stream;

    memset(&stream, 0, sizeof(stream));

    if (deflateInit(&stream, AS_INT(argv[1])->value) != Z_OK) {
        return ERR("Failed to initialize compression.");
    }

    uint8_t *input = AS_BYTES(argv[0])->bytes;

    stream.avail_in = AS_BYTES(argv[0])->length;
    stream.next_in = (Bytef *)input;

    uint32_t output_size = 2 * (stream.avail_in);
    uint8_t *output = malloc(output_size);

    stream.avail_out = output_size;
    stream.next_out = (Bytef *)(output);

    if (deflate(&stream, Z_FINISH) != Z_STREAM_END) {
        return ERR("Compression failed.");
    }

    if (deflateEnd(&stream) != Z_OK) {
        return ERR("Failed to finalize compression.");
    }

    return OK(take_bytes(output, stream.total_out));
}

static BuiltinResult _compress_zlib_decompress(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(BYTES, 0)

    z_stream stream;
    memset(&stream, 0, sizeof(stream));

    if (inflateInit(&stream) != Z_OK) {
        return ERR("Failed to initialize decompression.");
    }

    ObjBytes *bytes = AS_BYTES(argv[0]);

    stream.avail_in = bytes->length;
    stream.next_in = (Bytef *)bytes->bytes;

    uint32_t output_size = 2 * (stream.avail_in);
    uint8_t *output = malloc(output_size);

    stream.avail_out = output_size;
    stream.next_out = (Bytef *)(output);

    if (inflate(&stream, Z_FINISH) != Z_STREAM_END) {
        return ERR("Decompression failed.");
    }

    if (inflateEnd(&stream) != Z_OK) {
        return ERR("Failed to finalize decompression.");
    }

    return OK(take_bytes(output, stream.total_out));
}

ObjModule *get_compress_module(int count, char **parts) {
    if (_compress_module == NULL) {
        ObjModule *module = new_module(new_string("compress", 8));

        SET_INT_MEMBER("NO_COMPRESSION", Z_NO_COMPRESSION);
        SET_INT_MEMBER("BEST_SPEED", Z_BEST_SPEED);
        SET_INT_MEMBER("BEST_COMPRESSION", Z_BEST_COMPRESSION);
        SET_INT_MEMBER("DEFAULT_COMPRESSION", Z_DEFAULT_COMPRESSION);
        SET_INT_MEMBER("HUFFMAN_ONLY", Z_HUFFMAN_ONLY);

        if (_compress_zlib_module == NULL) {
            ObjModule *module = new_module(new_string("zlib", 4));

            SET_BUILTIN_FN_MEMBER("compress", _compress_zlib_compress);
            SET_BUILTIN_FN_MEMBER("decompress", _compress_zlib_decompress);

            _compress_zlib_module = module;
        }

        SET_MEMBER("zlib", _compress_zlib_module);

        _compress_module = module;
    }

    if (count == 0)
        return _compress_module;
    else if (count == 1) {
        if (strcmp(parts[0], "zlib") == 0)
            return _compress_zlib_module;

        return NULL;
    }

    return NULL;
}
