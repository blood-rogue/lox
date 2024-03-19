#include <openssl/evp.h>

#include "builtins.h"

static ObjClass *_digest_sha2_class = NULL;

static BuiltinResult _digest_sha2_hash224(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("SHA2-224", argv_0->bytes, argv_0->length);
}

static BuiltinResult _digest_sha2_hash256(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("SHA2-256", argv_0->bytes, argv_0->length);
}

static BuiltinResult _digest_sha2_hash384(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("SHA2-384", argv_0->bytes, argv_0->length);
}

static BuiltinResult _digest_sha2_hash512(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("SHA2-512", argv_0->bytes, argv_0->length);
}

static BuiltinResult _digest_sha2_init(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjInt, INT, 0)

    switch (argv_0->value) {
        case 224:
            return md_init("SHA2-224", AS_INSTANCE(caller));
        case 256:
            return md_init("SHA2-256", AS_INSTANCE(caller));
        case 384:
            return md_init("SHA2-384", AS_INSTANCE(caller));
        case 512:
            return md_init("SHA2-512", AS_INSTANCE(caller));
        default:
            ERR("Invalid bit count %ld. Can only be of size 224, 256, 384 or 512.", argv_0->value)
    }
}

ObjClass *get_digest_sha2_class() {
    if (_digest_sha2_class == NULL) {
        ObjClass *klass = new_builtin_class("SHA2");

        SET_BUILTIN_FN_STATIC("hash224", _digest_sha2_hash224);
        SET_BUILTIN_FN_STATIC("hash256", _digest_sha2_hash256);
        SET_BUILTIN_FN_STATIC("hash384", _digest_sha2_hash384);
        SET_BUILTIN_FN_STATIC("hash512", _digest_sha2_hash512);

        SET_BUILTIN_FN_METHOD("init", _digest_sha2_init);
        SET_BUILTIN_FN_METHOD("update", md_update);
        SET_BUILTIN_FN_METHOD("finish", md_finish);

        _digest_sha2_class = klass;
    }

    return _digest_sha2_class;
}
