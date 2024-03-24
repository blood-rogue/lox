#include <openssl/evp.h>

#include "native.h"

static ObjClass *_digest_sha3_class = NULL;

static NativeResult _digest_sha3_hash224(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("SHA3-224", argv_0->bytes, argv_0->length);
}

static NativeResult _digest_sha3_hash256(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("SHA3-256", argv_0->bytes, argv_0->length);
}

static NativeResult _digest_sha3_hash384(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("SHA3-384", argv_0->bytes, argv_0->length);
}

static NativeResult _digest_sha3_hash512(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("SHA3-512", argv_0->bytes, argv_0->length);
}

static NativeResult _digest_sha3_init(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjInt, INT, 0)

    if (mpz_cmp_si(argv_0->value, 0) == 0)
        return md_init("SHA3-224", AS_INSTANCE(caller));
    else if (mpz_cmp_si(argv_0->value, 1) == 0)
        return md_init("SHA3-256", AS_INSTANCE(caller));
    else if (mpz_cmp_si(argv_0->value, 2) == 0)
        return md_init("SHA3-384", AS_INSTANCE(caller));
    else if (mpz_cmp_si(argv_0->value, 3) == 0)
        return md_init("SHA3-512", AS_INSTANCE(caller));
    else
        ERR("Invalid bit count %s. Can only be of size 224, 256, 384 or 512.",
            mpz_get_str(NULL, 10, argv_0->value))
}

ObjClass *get_crypto_digest_sha3_class() {
    if (_digest_sha3_class == NULL) {
        ObjClass *klass = new_native_class("SHA3");

        SET_NATIVE_FN_STATIC("hash224", _digest_sha3_hash224);
        SET_NATIVE_FN_STATIC("hash256", _digest_sha3_hash256);
        SET_NATIVE_FN_STATIC("hash384", _digest_sha3_hash384);
        SET_NATIVE_FN_STATIC("hash512", _digest_sha3_hash512);

        SET_NATIVE_FN_METHOD("__init", _digest_sha3_init);
        SET_NATIVE_FN_METHOD("update", md_update);
        SET_NATIVE_FN_METHOD("finish", md_finish);

        _digest_sha3_class = klass;
    }

    return _digest_sha3_class;
}
