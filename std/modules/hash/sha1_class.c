#include <openssl/evp.h>

#include "builtins.h"

static ObjClass *_hash_sha1_class = NULL;

static BuiltinResult _hash_sha1_digest(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    EVP_MD_CTX *ctx = NULL;
    EVP_MD *sha1 = NULL;

    unsigned int len = 0;
    uint8_t *outdigest = NULL;

    ctx = EVP_MD_CTX_new();
    if (ctx == NULL)
        ERR("Could not initialize context.")

    sha1 = EVP_MD_fetch(NULL, "SHA1", NULL);
    if (sha1 == NULL) {
        EVP_MD_CTX_free(ctx);
        ERR("Could not get SHA1 algorithm.")
    }

    if (!EVP_DigestInit_ex(ctx, sha1, NULL)) {
        EVP_MD_CTX_free(ctx);
        EVP_MD_free(sha1);
        ERR("Could not init digest.")
    }

    if (!EVP_DigestUpdate(ctx, argv_0->bytes, argv_0->length)) {
        EVP_MD_CTX_free(ctx);
        EVP_MD_free(sha1);
        ERR("Could not update digest.")
    }

    outdigest = OPENSSL_malloc(EVP_MD_get_size(sha1));
    if (outdigest == NULL) {
        EVP_MD_CTX_free(ctx);
        EVP_MD_free(sha1);
        ERR("Could not allocate ")
    }

    if (!EVP_DigestFinal_ex(ctx, outdigest, &len)) {
        EVP_MD_CTX_free(ctx);
        EVP_MD_free(sha1);
        OPENSSL_free(outdigest);
        ERR("Could not finalize digest.")
    }

    OK(new_bytes(outdigest, len));
}

ObjClass *get_hash_sha1_class() {
    if (_hash_sha1_class == NULL) {
        ObjClass *klass = new_builtin_class("Sha1");

        SET_BUILTIN_FN_STATIC("digest", _hash_sha1_digest);

        _hash_sha1_class = klass;
    }

    return _hash_sha1_class;
}
