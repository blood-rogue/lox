#include <openssl/evp.h>

#include "builtins.h"

static ObjClass *_hash_sha3_class = NULL;

static BuiltinResult _hash_sha3_digest224(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("SHA3-224", argv_0->bytes, argv_0->length);
}

static BuiltinResult _hash_sha3_digest256(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("SHA3-256", argv_0->bytes, argv_0->length);
}

static BuiltinResult _hash_sha3_digest384(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("SHA3-384", argv_0->bytes, argv_0->length);
}

static BuiltinResult _hash_sha3_digest512(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("SHA3-512", argv_0->bytes, argv_0->length);
}

static BuiltinResult _hash_sha3_init(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjInt, INT, 0)

    switch (argv_0->value) {
        case 224:
            return md_init("SHA3-224", AS_INSTANCE(caller));
        case 256:
            return md_init("SHA3-256", AS_INSTANCE(caller));
        case 384:
            return md_init("SHA3-384", AS_INSTANCE(caller));
        case 512:
            return md_init("SHA3-512", AS_INSTANCE(caller));
        default:
            ERR("Invalid bit count %ld. Can only be of size 224, 256, 384 or 512.", argv_0->value)
    }
}

ObjClass *get_hash_sha3_class() {
    if (_hash_sha3_class == NULL) {
        ObjClass *klass = new_builtin_class("SHA3");

        SET_BUILTIN_FN_STATIC("digest224", _hash_sha3_digest224);
        SET_BUILTIN_FN_STATIC("digest256", _hash_sha3_digest256);
        SET_BUILTIN_FN_STATIC("digest384", _hash_sha3_digest384);
        SET_BUILTIN_FN_STATIC("digest512", _hash_sha3_digest512);

        SET_BUILTIN_FN_METHOD("init", _hash_sha3_init);
        SET_BUILTIN_FN_METHOD("update", md_update);
        SET_BUILTIN_FN_METHOD("finish", md_finish);

        _hash_sha3_class = klass;
    }

    return _hash_sha3_class;
}
