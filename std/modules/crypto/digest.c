#include <openssl/evp.h>

#include "native.h"

static ObjModule *_digest_module = NULL;

NativeResult md_digest(const char *alg, uint8_t *bytes, int bytes_len) {
    EVP_MD_CTX *ctx = NULL;
    EVP_MD *md = NULL;

    ctx = EVP_MD_CTX_new();
    if (ctx == NULL)
        ERR("Could not initialize context.")

    md = EVP_MD_fetch(NULL, alg, NULL);
    if (md == NULL) {
        EVP_MD_CTX_free(ctx);
        ERR("Could not get %s algorithm.", alg)
    }

    if (!EVP_DigestInit_ex(ctx, md, NULL)) {
        EVP_MD_CTX_free(ctx);
        EVP_MD_free(md);
        ERR("Could not init digest.")
    }

    if (!EVP_DigestUpdate(ctx, bytes, bytes_len)) {
        EVP_MD_CTX_free(ctx);
        EVP_MD_free(md);
        ERR("Could not update digest.")
    }

    uint8_t *outdigest = OPENSSL_malloc(EVP_MD_get_size(md));
    if (outdigest == NULL) {
        EVP_MD_CTX_free(ctx);
        EVP_MD_free(md);
        ERR("Could not allocate ")
    }

    uint32_t len;
    if (!EVP_DigestFinal_ex(ctx, outdigest, &len)) {
        EVP_MD_CTX_free(ctx);
        EVP_MD_free(md);
        OPENSSL_free(outdigest);
        ERR("Could not finalize digest.")
    }

    OK(new_bytes(outdigest, len));
}

static void free_mdctx(MdCtx *md_ctx) {
    EVP_MD_CTX_free(md_ctx->ctx);
    EVP_MD_free(md_ctx->md);

    free(md_ctx);
}

NativeResult md_init(const char *alg, ObjInstance *instance) {
    EVP_MD_CTX *ctx = NULL;
    EVP_MD *md = NULL;

    ctx = EVP_MD_CTX_new();
    if (ctx == NULL)
        ERR("Could not initialize context.")

    md = EVP_MD_fetch(NULL, alg, NULL);
    if (md == NULL) {
        EVP_MD_CTX_free(ctx);
        ERR("Could not get %s algorithm.", alg)
    }

    if (!EVP_DigestInit_ex(ctx, md, NULL)) {
        EVP_MD_CTX_free(ctx);
        EVP_MD_free(md);
        ERR("Could not init digest.")
    }

    MdCtx *md_ctx = malloc(sizeof(MdCtx));
    md_ctx->ctx = ctx;
    md_ctx->md = md;

    SET_FIELD("$$internal", new_native_struct(md_ctx, (FreeFn)free_mdctx));

    OK(new_nil());
}

NativeResult md_update(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    ObjInstance *md_ctx_instance = AS_INSTANCE(caller);

    GET_INTERNAL(MdCtx *, md_ctx)

    if (!EVP_DigestUpdate(md_ctx->ctx, argv_0->bytes, argv_0->length)) {
        EVP_MD_CTX_free(md_ctx->ctx);
        EVP_MD_free(md_ctx->md);
        ERR("Could not update digest.")
    }

    OK(new_nil());
}

NativeResult md_finish(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInstance *md_ctx_instance = AS_INSTANCE(caller);
    GET_INTERNAL(MdCtx *, md_ctx)

    uint8_t *outdigest = OPENSSL_malloc(EVP_MD_get_size(md_ctx->md));
    if (outdigest == NULL) {
        EVP_MD_CTX_free(md_ctx->ctx);
        EVP_MD_free(md_ctx->md);
        ERR("Could not allocate ")
    }

    uint32_t len;
    if (!EVP_DigestFinal_ex(md_ctx->ctx, outdigest, &len)) {
        EVP_MD_CTX_free(md_ctx->ctx);
        EVP_MD_free(md_ctx->md);
        OPENSSL_free(outdigest);
        ERR("Could not finalize digest.")
    }

    OK(new_bytes(outdigest, len));
}

ObjModule *get_crypto_digest_module() {
    if (_digest_module == NULL) {
        ObjModule *module = new_module("digest");

        SET_MEMBER("SHA1", get_crypto_digest_sha1_class());
        SET_MEMBER("SHA2", get_crypto_digest_sha2_class());
        SET_MEMBER("SHA3", get_crypto_digest_sha3_class());
        SET_MEMBER("MD4", get_crypto_digest_md4_class());
        SET_MEMBER("MD5", get_crypto_digest_md5_class());
        SET_MEMBER("Shake128", get_crypto_digest_shake128_class());
        SET_MEMBER("Shake256", get_crypto_digest_shake256_class());
        SET_MEMBER("SM3", get_crypto_digest_sm3_class());
        SET_MEMBER("RIPEMD160", get_crypto_digest_ripemd160_class());
        SET_MEMBER("Whirlpool", get_crypto_digest_whirlpool_class());
        SET_MEMBER("BLAKE2b512", get_crypto_digest_blake2b512_class());
        SET_MEMBER("BLAKE2s256", get_crypto_digest_blake2s256_class());

        SET_INT_MEMBER("SHA_224", 0);
        SET_INT_MEMBER("SHA_256", 1);
        SET_INT_MEMBER("SHA_384", 2);
        SET_INT_MEMBER("SHA_512", 3);
        SET_INT_MEMBER("SHA_512_224", 4);
        SET_INT_MEMBER("SHA_512_256", 5);

        _digest_module = module;
    }

    return _digest_module;
}
