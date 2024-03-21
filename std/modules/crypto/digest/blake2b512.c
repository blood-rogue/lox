#include <openssl/evp.h>

#include "builtins.h"

static ObjClass *_digest_blake2b512_class = NULL;

static NativeResult _digest_blake2b512_hash(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("BLAKE2b512", argv_0->bytes, argv_0->length);
}

static NativeResult _digest_blake2b512_init(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    return md_init("BLAKE2b512", AS_INSTANCE(caller));
}

ObjClass *get_crypto_digest_blake2b512_class() {
    if (_digest_blake2b512_class == NULL) {
        ObjClass *klass = new_builtin_class("BLAKE2b512");

        SET_BUILTIN_FN_STATIC("hash", _digest_blake2b512_hash);

        SET_BUILTIN_FN_METHOD("init", _digest_blake2b512_init);
        SET_BUILTIN_FN_METHOD("update", md_update);
        SET_BUILTIN_FN_METHOD("finish", md_finish);

        _digest_blake2b512_class = klass;
    }

    return _digest_blake2b512_class;
}
