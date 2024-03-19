#include <openssl/evp.h>

#include "builtins.h"

static ObjClass *_hash_shake256_class = NULL;

static BuiltinResult _hash_shake256_digest(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBytes, BYTES, 0)

    return md_digest("SHAKE-256", argv_0->bytes, argv_0->length);
}

static BuiltinResult _hash_shake256_init(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    return md_init("SHAKE-256", AS_INSTANCE(caller));
}

ObjClass *get_hash_shake256_class() {
    if (_hash_shake256_class == NULL) {
        ObjClass *klass = new_builtin_class("Shake256");

        SET_BUILTIN_FN_STATIC("digest", _hash_shake256_digest);

        SET_BUILTIN_FN_METHOD("init", _hash_shake256_init);
        SET_BUILTIN_FN_METHOD("update", md_update);
        SET_BUILTIN_FN_METHOD("finish", md_finish);

        _hash_shake256_class = klass;
    }

    return _hash_shake256_class;
}
