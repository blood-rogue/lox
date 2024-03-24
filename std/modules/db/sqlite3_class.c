#include <sqlite3.h>

#include "native.h"

ObjClass *_db_sqlite3_class = NULL;

static void close_db(void *db) { sqlite3_close(db); }

static NativeResult _db_sqlite3_class_init(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    sqlite3 *db;
    if (sqlite3_open(argv_0->chars, &db) != SQLITE_OK)
        ERR("Can't open database: %s", sqlite3_errmsg(db))

    ObjInstance *instance = AS_INSTANCE(caller);
    SET_FIELD("$$internal", new_native_struct(db, close_db));

    OK(new_nil());
}

ObjClass *get_db_sqlite3_class() {
    if (_db_sqlite3_class == NULL) {
        ObjClass *klass = new_native_class("SQLite3");

        SET_NATIVE_FN_METHOD("__init", _db_sqlite3_class_init);

        _db_sqlite3_class = klass;
    }

    return _db_sqlite3_class;
}
