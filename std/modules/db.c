#include "builtins.h"

static ObjModule *_db_module = NULL;

ObjModule *get_db_module(int count, UNUSED(char **parts)) {
    CHECK_PART_COUNT

    if (_db_module == NULL) {
        ObjModule *module = new_module("db");

        SET_MEMBER("KV", get_db_kv_class());

        _db_module = module;
    }

    return _db_module;
}
