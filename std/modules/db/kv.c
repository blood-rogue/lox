#include <leveldb/c.h>

#include "builtins.h"

static ObjModule *_db_kv_module = NULL;

static BuiltinResult _db_kv_open(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    leveldb_options_t *options = leveldb_options_create();
    leveldb_options_set_create_if_missing(options, true);

    char *err = NULL;
    leveldb_t *db = leveldb_open(options, argv_0->chars, &err);

    ObjInstance *instance = new_instance(get_db_kv_kv_class());

    SET_FIELD("$$internal", new_native_struct(db, (FreeFn)leveldb_close));

    OK(instance);
}

ObjModule *get_db_kv_module() {
    if (_db_kv_module == NULL) {
        ObjModule *module = new_module("kv");

        SET_BUILTIN_FN_MEMBER("open", _db_kv_open);

        _db_kv_module = module;
    }

    return _db_kv_module;
}
