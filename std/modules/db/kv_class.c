#include <leveldb/c.h>

#include "builtins.h"

static ObjClass *_db_kv_class = NULL;

static BuiltinResult _db_kv_init(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    leveldb_options_t *options = leveldb_options_create();
    leveldb_options_set_create_if_missing(options, true);

    char *err = NULL;
    leveldb_t *db = leveldb_open(options, argv_0->chars, &err);

    ObjInstance *instance = AS_INSTANCE(caller);

    SET_FIELD("$$internal", new_native_struct(db, (FreeFn)leveldb_close));

    OK(instance);
}

static BuiltinResult _db_kv_set(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjString, STRING, 0)
    CHECK_ARG_TYPE(ObjString, STRING, 1)

    ObjInstance *db_instance = AS_INSTANCE(caller);
    GET_INTERNAL(leveldb_t *, db);

    leveldb_writeoptions_t *options = leveldb_writeoptions_create();
    char *err = NULL;
    leveldb_put(
        db, options, argv_0->chars, argv_0->raw_length, argv_1->chars, argv_1->raw_length, &err);

    if (err != NULL)
        ERR("DB write error: %s", err)

    OK(new_nil());
}

static BuiltinResult _db_kv_get(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    ObjInstance *db_instance = AS_INSTANCE(caller);
    GET_INTERNAL(leveldb_t *, db);

    leveldb_readoptions_t *options = leveldb_readoptions_create();
    char *err = NULL;
    size_t val_len;
    char *val = leveldb_get(db, options, argv_0->chars, argv_0->raw_length, &val_len, &err);

    if (err != NULL)
        ERR("DB read error: %s", err)

    if (val == NULL)
        ERR("No value found for key '%.*s'.", argv_0->raw_length, argv_0->chars)

    OK(take_string(val, val_len));
}

static BuiltinResult _db_kv_delete(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    ObjInstance *db_instance = AS_INSTANCE(caller);
    GET_INTERNAL(leveldb_t *, db);

    leveldb_writeoptions_t *options = leveldb_writeoptions_create();
    char *err = NULL;

    leveldb_delete(db, options, argv_0->chars, argv_0->raw_length, &err);

    if (err != NULL)
        ERR("DB delete error: %s", err)

    OK(new_nil());
}

static BuiltinResult _db_kv_close(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInstance *db_instance = AS_INSTANCE(caller);
    GET_INTERNAL(leveldb_t *, db);

    leveldb_close(db);

    native->ptr = NULL;

    OK(new_nil());
}

ObjClass *get_db_kv_class() {
    if (_db_kv_class == NULL) {
        ObjClass *klass = new_builtin_class("KV");

        SET_BUILTIN_FN_METHOD("init", _db_kv_init);
        SET_BUILTIN_FN_METHOD("set", _db_kv_set);
        SET_BUILTIN_FN_METHOD("get", _db_kv_get);
        SET_BUILTIN_FN_METHOD("delete", _db_kv_delete);
        SET_BUILTIN_FN_METHOD("close", _db_kv_close);

        _db_kv_class = klass;
    }

    return _db_kv_class;
}
