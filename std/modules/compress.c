#include <brotli/decode.h>
#include <brotli/encode.h>
#include <liblzf/lzf.h>
#include <lz4.h>
#include <lzma.h>
#include <zlib.h>
#include <zstd.h>

#include "builtins.h"

static ObjModule *_compress_module = NULL;
static ObjModule *_compress_zlib_module = NULL;
static ObjModule *_compress_brotli_module = NULL;
static ObjModule *_compress_lzf_module = NULL;
static ObjModule *_compress_lz4_module = NULL;
static ObjModule *_compress_lzma_module = NULL;
static ObjModule *_compress_zstd_module = NULL;

static BuiltinResult _compress_zlib_compress(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(BYTES, 0)
    CHECK_ARG_TYPE(INT, 1)

    ObjBytes *bytes = AS_BYTES(argv[0]);

    uLong dest_len = compressBound(bytes->length);
    Bytef *dest = malloc(dest_len);

    if (compress2(dest, &dest_len, bytes->bytes, bytes->length, AS_INT(argv[1])->value) != Z_OK)
        return ERR("Could not compress data.");

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

    return OK(take_bytes(dest, dest_len));
}

static BuiltinResult _compress_zstd_compress(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(BYTES, 0)
    CHECK_ARG_TYPE(INT, 1)

    ObjBytes *bytes = AS_BYTES(argv[0]);

    size_t dest_len = ZSTD_compressBound(bytes->length);
    uint8_t *dest = malloc(dest_len);

    dest_len = ZSTD_compress(dest, dest_len, bytes->bytes, bytes->length, AS_INT(argv[1])->value);
    if (ZSTD_isError(dest_len))
        return ERR("Could not compress data.");

    return OK(take_bytes(dest, dest_len));
}

typedef struct {
    uint8_t *src;
    size_t off;
    size_t length;
} reader_ctx_t;

static size_t read_bytes(reader_ctx_t *ctx, uint8_t *dest, size_t len) {
    if (ctx->length == ctx->off)
        return 0;

    size_t read = ctx->length - ctx->off;
    if (len >= read) {
        memcpy(dest, ctx->src + ctx->off, read);
        ctx->off = ctx->length;
        return read;
    }

    memcpy(dest, ctx->src + ctx->off, len);
    ctx->off += len;
    return len;
}

typedef struct {
    uint8_t *dest;
    size_t capacity;
    size_t length;
} writer_ctx_t;

static void write_bytes(writer_ctx_t *ctx, uint8_t *src, size_t count) {
    if (ctx->length + count >= ctx->capacity) {
        ctx->capacity *= 2;
        ctx->dest = realloc(ctx->dest, ctx->capacity);
    }

    memcpy(ctx->dest + ctx->length, src, count);
    ctx->length += count;
}

static BuiltinResult _compress_zstd_decompress(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(BYTES, 0)

    ObjBytes *bytes = AS_BYTES(argv[0]);

    uLong dest_len = ZSTD_getFrameContentSize(bytes->bytes, bytes->length);

    if (dest_len == ZSTD_CONTENTSIZE_ERROR)
        return ERR("Invalid compressed data");

    if (dest_len == ZSTD_CONTENTSIZE_UNKNOWN) {
        size_t buffInSize = ZSTD_DStreamInSize();
        uint8_t *buffIn = malloc(buffInSize);

        size_t buffOutSize = ZSTD_DStreamOutSize();
        void *buffOut = malloc(buffOutSize);

        ZSTD_DCtx *dctx = ZSTD_createDCtx();

        if (dctx == NULL)
            return ERR("Could not create ZSTD Context.");

        size_t const toRead = buffInSize;
        size_t read;
        size_t lastRet = 0;
        int isEmpty = 1;

        reader_ctx_t reader_ctx;
        reader_ctx.src = bytes->bytes;
        reader_ctx.length = bytes->length;
        reader_ctx.off = 0;

        writer_ctx_t write_ctx;
        write_ctx.length = 0;
        write_ctx.capacity = 8;
        write_ctx.dest = malloc(write_ctx.capacity);

        while ((read = read_bytes(&reader_ctx, buffIn, toRead))) {
            isEmpty = 0;
            ZSTD_inBuffer input = {buffIn, read, 0};
            while (input.pos < input.size) {
                ZSTD_outBuffer output = {buffOut, buffOutSize, 0};
                size_t const ret = ZSTD_decompressStream(dctx, &output, &input);

                write_bytes(&write_ctx, buffOut, output.pos);
                lastRet = ret;
            }
        }

        if (isEmpty) {
            return ERR("Empty input.");
        }

        if (lastRet != 0) {
            return ERR("EOF before end of stream.");
        }

        ZSTD_freeDCtx(dctx);
        free(buffIn);
        free(buffOut);

        return OK(take_bytes(write_ctx.dest, write_ctx.length));
    } else {
        Bytef *dest = malloc(dest_len);

        dest_len = ZSTD_decompress(dest, dest_len, bytes->bytes, bytes->length);
        if (ZSTD_isError(dest_len))
            return ERR("Could not decompress data.");

        return OK(take_bytes(dest, dest_len));
    }
}

ObjModule *get_compress_module(int count, char **parts) {
    if (_compress_zlib_module == NULL) {
        ObjModule *module = new_module(new_string("zlib", 4));

        SET_BUILTIN_FN_MEMBER("compress", _compress_zlib_compress);
        SET_BUILTIN_FN_MEMBER("decompress", _compress_zlib_decompress);

        _compress_zlib_module = module;
    }
    if (_compress_brotli_module == NULL) {
        ObjModule *module = new_module(new_string("brotli", 6));

        _compress_brotli_module = module;
    }
    if (_compress_lzf_module == NULL) {
        ObjModule *module = new_module(new_string("lzf", 3));

        _compress_lzf_module = module;
    }
    if (_compress_lz4_module == NULL) {
        ObjModule *module = new_module(new_string("lz4", 3));

        _compress_lz4_module = module;
    }
    if (_compress_lzma_module == NULL) {
        ObjModule *module = new_module(new_string("lzma", 4));

        _compress_lzma_module = module;
    }
    if (_compress_zstd_module == NULL) {
        ObjModule *module = new_module(new_string("zstd", 4));

        _compress_zstd_module = module;
    }

    if (_compress_module == NULL) {
        ObjModule *module = new_module(new_string("compress", 8));

        SET_INT_MEMBER("NO_COMPRESSION", Z_NO_COMPRESSION);
        SET_INT_MEMBER("BEST_SPEED", Z_BEST_SPEED);
        SET_INT_MEMBER("BEST_COMPRESSION", Z_BEST_COMPRESSION);
        SET_INT_MEMBER("DEFAULT_COMPRESSION", Z_DEFAULT_COMPRESSION);
        SET_INT_MEMBER("HUFFMAN_ONLY", Z_HUFFMAN_ONLY);

        SET_MEMBER("zlib", _compress_zlib_module);
        SET_MEMBER("brotli", _compress_brotli_module);
        SET_MEMBER("lzf", _compress_lzf_module);
        SET_MEMBER("lz4", _compress_lz4_module);
        SET_MEMBER("lzma", _compress_lzma_module);
        SET_MEMBER("zstd", _compress_zstd_module);

        _compress_module = module;
    }

    if (count == 0)
        return _compress_module;
    else if (count == 1) {
        if (strcmp(parts[0], "zlib") == 0)
            return _compress_zlib_module;
        else if (strcmp(parts[0], "brotli") == 0)
            return _compress_brotli_module;
        else if (strcmp(parts[0], "zlib") == 0)
            return _compress_lzf_module;
        else if (strcmp(parts[0], "lzf") == 0)
            return _compress_lz4_module;
        else if (strcmp(parts[0], "lzma") == 0)
            return _compress_lzma_module;
        else if (strcmp(parts[0], "zstd") == 0)
            return _compress_zstd_module;

        return NULL;
    }

    return NULL;
}
