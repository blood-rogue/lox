#include "builtins.h"

static ObjModule *_crypto_module = NULL;

ObjModule *get_crypto_module(int count, char **parts) {
    if (_crypto_module == NULL) {
        ObjModule *module = new_module("crypto");

        SET_MEMBER("digest", get_crypto_digest_module());

        _crypto_module = module;
    }

    if (count == 0)
        return _crypto_module;
    else if (count == 1) {
        if (strcmp(parts[0], "digest") == 0)
            return get_crypto_digest_module();

        return NULL;
    }

    return NULL;
}
