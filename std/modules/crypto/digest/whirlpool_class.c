#include <openssl/evp.h>

#include "builtins.h"

static ObjClass *_digest_whirlpool_class = NULL;

static NativeResult _digest_whirlpool_hash(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("whirlpool", argv_0->bytes, argv_0->length);
}

static NativeResult _digest_whirlpool_init(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    return md_init("whirlpool", AS_INSTANCE(caller));
}

ObjClass *get_crypto_digest_whirlpool_class() {
    if (_digest_whirlpool_class == NULL) {
        ObjClass *klass = new_builtin_class("Whirlpool");

        SET_BUILTIN_FN_STATIC("hash", _digest_whirlpool_hash);

        SET_BUILTIN_FN_METHOD("init", _digest_whirlpool_init);
        SET_BUILTIN_FN_METHOD("update", md_update);
        SET_BUILTIN_FN_METHOD("finish", md_finish);

        _digest_whirlpool_class = klass;
    }

    return _digest_whirlpool_class;
}
