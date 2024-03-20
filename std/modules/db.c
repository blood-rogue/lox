#include "builtins.h"

static ObjModule *_db_module = NULL;

ObjModule *get_db_module(int count, char **parts) {
    if (_db_module == NULL) {
        ObjModule *module = new_module("db");

        SET_MEMBER("kv", get_db_kv_module());

        _db_module = module;
    }

    if (count == 0)
        return _db_module;
    else if (count == 1) {
        if (strcmp(parts[0], "kv") == 0)
            return get_db_kv_module();

        return NULL;
    }

    return NULL;
}
