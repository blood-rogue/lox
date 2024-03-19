#include <openssl/evp.h>

#include "builtins.h"

static ObjClass *_hash_sha2_class = NULL;

static BuiltinResult _hash_sha2_digest224(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("SHA2-224", argv_0->bytes, argv_0->length);
}

static BuiltinResult _hash_sha2_digest256(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("SHA2-256", argv_0->bytes, argv_0->length);
}

static BuiltinResult _hash_sha2_digest384(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("SHA2-384", argv_0->bytes, argv_0->length);
}

static BuiltinResult _hash_sha2_digest512(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("SHA2-512", argv_0->bytes, argv_0->length);
}

static BuiltinResult _hash_sha2_init(int argc, Obj **argv, Obj *caller) {
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

ObjClass *get_hash_sha2_class() {
    if (_hash_sha2_class == NULL) {
        ObjClass *klass = new_builtin_class("SHA2");

        SET_BUILTIN_FN_STATIC("digest224", _hash_sha2_digest224);
        SET_BUILTIN_FN_STATIC("digest256", _hash_sha2_digest256);
        SET_BUILTIN_FN_STATIC("digest384", _hash_sha2_digest384);
        SET_BUILTIN_FN_STATIC("digest512", _hash_sha2_digest512);

        SET_BUILTIN_FN_METHOD("init", _hash_sha2_init);
        SET_BUILTIN_FN_METHOD("update", md_update);
        SET_BUILTIN_FN_METHOD("finish", md_finish);

        _hash_sha2_class = klass;
    }

    return _hash_sha2_class;
}
