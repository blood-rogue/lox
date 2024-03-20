#include <openssl/evp.h>

#include "builtins.h"

static ObjClass *_digest_sha1_class = NULL;

static BuiltinResult _digest_sha1_hash(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("SHA1", argv_0->bytes, argv_0->length);
}

static BuiltinResult _digest_sha1_init(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    return md_init("SHA1", AS_INSTANCE(caller));
}

ObjClass *get_crypto_digest_sha1_class() {
    if (_digest_sha1_class == NULL) {
        ObjClass *klass = new_builtin_class("SHA1");

        SET_BUILTIN_FN_STATIC("hash", _digest_sha1_hash);

        SET_BUILTIN_FN_METHOD("init", _digest_sha1_init);
        SET_BUILTIN_FN_METHOD("update", md_update);
        SET_BUILTIN_FN_METHOD("finish", md_finish);

        _digest_sha1_class = klass;
    }

    return _digest_sha1_class;
}