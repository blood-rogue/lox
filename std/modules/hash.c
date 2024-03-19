#include "builtins.h"

static ObjModule *_hash_module = NULL;

ObjModule *get_hash_module(int count, UNUSED(char **, parts)) {
    CHECK_PART_COUNT

    if (_hash_module == NULL) {
        ObjModule *module = new_module(new_string("hash", 4));

        SET_MEMBER("Sha1", get_hash_sha1_class());
        SET_MEMBER("Sha2", get_hash_sha2_class());
        SET_MEMBER("Sha3", get_hash_sha3_class());

        _hash_module = module;
    }

    return _hash_module;
}
