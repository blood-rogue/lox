#include <openssl/evp.h>

#include "builtins.h"

static ObjClass *_digest_md5_class = NULL;

static BuiltinResult _digest_md5_hash(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("MD5", argv_0->bytes, argv_0->length);
}

static BuiltinResult _digest_md5_init(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    return md_init("MD5", AS_INSTANCE(caller));
}

ObjClass *get_crypto_digest_md5_class() {
    if (_digest_md5_class == NULL) {
        ObjClass *klass = new_builtin_class("MD5");

        SET_BUILTIN_FN_STATIC("hash", _digest_md5_hash);

        SET_BUILTIN_FN_METHOD("init", _digest_md5_init);
        SET_BUILTIN_FN_METHOD("update", md_update);
        SET_BUILTIN_FN_METHOD("finish", md_finish);

        _digest_md5_class = klass;
    }

    return _digest_md5_class;
}
