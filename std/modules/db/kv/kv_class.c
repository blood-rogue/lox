#include <leveldb/c.h>

#include "builtins.h"

static ObjClass *_db_kv_kv_class = NULL;

ObjClass *get_db_kv_kv_class() {
    if (_db_kv_kv_class == NULL) {
        ObjClass *klass = new_builtin_class("KV");

        _db_kv_kv_class = klass;
    }

    return _db_kv_kv_class;
}
