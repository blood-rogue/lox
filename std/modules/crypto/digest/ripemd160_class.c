#include <openssl/evp.h>

#include "builtins.h"

static ObjClass *_digest_ripemd160_class = NULL;

static NativeResult _digest_ripemd160_hash(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("RIPEMD160", argv_0->bytes, argv_0->length);
}

static NativeResult _digest_ripemd160_init(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    return md_init("RIPEMD160", AS_INSTANCE(caller));
}

ObjClass *get_crypto_digest_ripemd160_class() {
    if (_digest_ripemd160_class == NULL) {
        ObjClass *klass = new_builtin_class("RIPEMD160");

        SET_BUILTIN_FN_STATIC("hash", _digest_ripemd160_hash);

        SET_BUILTIN_FN_METHOD("__init", _digest_ripemd160_init);
        SET_BUILTIN_FN_METHOD("update", md_update);
        SET_BUILTIN_FN_METHOD("finish", md_finish);

        _digest_ripemd160_class = klass;
    }

    return _digest_ripemd160_class;
}
