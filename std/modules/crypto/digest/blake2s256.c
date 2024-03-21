#include <openssl/evp.h>

#include "builtins.h"

static ObjClass *_digest_blake2s256_class = NULL;

static BuiltinResult _digest_blake2s256_hash(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("BLAKE2s256", argv_0->bytes, argv_0->length);
}

static BuiltinResult _digest_blake2s256_init(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    return md_init("BLAKE2s256", AS_INSTANCE(caller));
}

ObjClass *get_crypto_digest_blake2s256_class() {
    if (_digest_blake2s256_class == NULL) {
        ObjClass *klass = new_builtin_class("BLAKE2s256");

        SET_BUILTIN_FN_STATIC("hash", _digest_blake2s256_hash);

        SET_BUILTIN_FN_METHOD("init", _digest_blake2s256_init);
        SET_BUILTIN_FN_METHOD("update", md_update);
        SET_BUILTIN_FN_METHOD("finish", md_finish);

        _digest_blake2s256_class = klass;
    }

    return _digest_blake2s256_class;
}
