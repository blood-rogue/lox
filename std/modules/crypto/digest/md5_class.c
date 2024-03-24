#include <openssl/evp.h>

#include "native.h"

static ObjClass *_digest_md5_class = NULL;

static NativeResult _digest_md5_hash(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("MD5", argv_0->bytes, argv_0->length);
}

static NativeResult _digest_md5_init(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    return md_init("MD5", AS_INSTANCE(caller));
}

ObjClass *get_crypto_digest_md5_class() {
    if (_digest_md5_class == NULL) {
        ObjClass *klass = new_native_class("MD5");

        SET_NATIVE_FN_STATIC("hash", _digest_md5_hash);

        SET_NATIVE_FN_METHOD("__init", _digest_md5_init);
        SET_NATIVE_FN_METHOD("update", md_update);
        SET_NATIVE_FN_METHOD("finish", md_finish);

        _digest_md5_class = klass;
    }

    return _digest_md5_class;
}
