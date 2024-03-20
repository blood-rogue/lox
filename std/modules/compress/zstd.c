#include <zstd.h>

#include "builtins.h"

static ObjModule *_compress_zstd_module = NULL;

static BuiltinResult _compress_zstd_compress(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)
    CHECK_ARG_TYPE(ObjInt, INT, 1)

    size_t dest_len = ZSTD_compressBound(argv_0->length);
    uint8_t *dest = malloc(dest_len);

    dest_len = ZSTD_compress(dest, dest_len, argv_0->bytes, argv_0->length, argv_1->value);
    if (ZSTD_isError(dest_len))
        ERR("Could not compress data.")

    dest = realloc(dest, dest_len);

    OK(take_bytes(dest, dest_len));
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

static BuiltinResult _compress_zstd_decompress(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    ObjBytes *bytes = argv_0;

    uint64_t dest_len = ZSTD_getFrameContentSize(bytes->bytes, bytes->length);

    if (dest_len == ZSTD_CONTENTSIZE_ERROR)
        ERR("Invalid compressed data")

    if (dest_len == ZSTD_CONTENTSIZE_UNKNOWN) {
        size_t buffInSize = ZSTD_DStreamInSize();
        uint8_t *buffIn = malloc(buffInSize);

        size_t buffOutSize = ZSTD_DStreamOutSize();
        void *buffOut = malloc(buffOutSize);

        ZSTD_DCtx *dctx = ZSTD_createDCtx();

        if (dctx == NULL)
            ERR("Could not create ZSTD Context.")

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
            ERR("Empty input.")
        }

        if (lastRet != 0) {
            ERR("EOF before end of stream.")
        }

        ZSTD_freeDCtx(dctx);
        free(buffIn);
        free(buffOut);

        OK(take_bytes(write_ctx.dest, write_ctx.length));
    } else {
        uint8_t *dest = malloc(dest_len);

        dest_len = ZSTD_decompress(dest, dest_len, bytes->bytes, bytes->length);
        if (ZSTD_isError(dest_len))
            ERR("Could not decompress data.")

        dest = realloc(dest, dest_len);

        OK(take_bytes(dest, dest_len));
    }
}

ObjModule *get_compress_zstd_module() {
    if (_compress_zstd_module == NULL) {
        ObjModule *module = new_module("zstd");

        SET_BUILTIN_FN_MEMBER("compress", _compress_zstd_compress);
        SET_BUILTIN_FN_MEMBER("decompress", _compress_zstd_decompress);

        _compress_zstd_module = module;
    }

    return _compress_zstd_module;
}
