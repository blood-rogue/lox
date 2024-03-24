#include <openssl/evp.h>

#include "builtins.h"

static ObjClass *_digest_sha2_class = NULL;

static NativeResult _digest_sha2_hash224(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("SHA2-224", argv_0->bytes, argv_0->length);
}

static NativeResult _digest_sha2_hash256(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("SHA2-256", argv_0->bytes, argv_0->length);
}

static NativeResult _digest_sha2_hash384(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("SHA2-384", argv_0->bytes, argv_0->length);
}

static NativeResult _digest_sha2_hash512(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("SHA2-512", argv_0->bytes, argv_0->length);
}

static NativeResult _digest_sha2_hash512_224(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("SHA2-512", argv_0->bytes, argv_0->length);
}

static NativeResult _digest_sha2_hash512_256(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("SHA2-512", argv_0->bytes, argv_0->length);
}

static NativeResult _digest_sha2_init(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjInt, INT, 0)

    if (mpz_cmp_si(argv_0->value, 0) == 0)
        return md_init("SHA2-224", AS_INSTANCE(caller));
    else if (mpz_cmp_si(argv_0->value, 1) == 0)
        return md_init("SHA2-256", AS_INSTANCE(caller));
    else if (mpz_cmp_si(argv_0->value, 2) == 0)
        return md_init("SHA2-384", AS_INSTANCE(caller));
    else if (mpz_cmp_si(argv_0->value, 3) == 0)
        return md_init("SHA2-512", AS_INSTANCE(caller));
    else if (mpz_cmp_si(argv_0->value, 4) == 0)
        return md_init("SHA2-512/224", AS_INSTANCE(caller));
    else if (mpz_cmp_si(argv_0->value, 5) == 0)
        return md_init("SHA2-512/256", AS_INSTANCE(caller));
    else
        ERR("Invalid bit count %s. Can only be of size 224, 256, 384, 512, 512/224 or "
            "512/256.",
            mpz_get_str(NULL, 10, argv_0->value))
}

ObjClass *get_crypto_digest_sha2_class() {
    if (_digest_sha2_class == NULL) {
        ObjClass *klass = new_builtin_class("SHA2");

        SET_BUILTIN_FN_STATIC("hash224", _digest_sha2_hash224);
        SET_BUILTIN_FN_STATIC("hash256", _digest_sha2_hash256);
        SET_BUILTIN_FN_STATIC("hash384", _digest_sha2_hash384);
        SET_BUILTIN_FN_STATIC("hash512", _digest_sha2_hash512);
        SET_BUILTIN_FN_STATIC("hash512_224", _digest_sha2_hash512_224);
        SET_BUILTIN_FN_STATIC("hash512_256", _digest_sha2_hash512_256);

        SET_BUILTIN_FN_METHOD("__init", _digest_sha2_init);
        SET_BUILTIN_FN_METHOD("update", md_update);
        SET_BUILTIN_FN_METHOD("finish", md_finish);

        _digest_sha2_class = klass;
    }

    return _digest_sha2_class;
}
