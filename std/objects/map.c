#include "builtins.h"

static NativeResult _map_len(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    OK(new_int_i(AS_MAP(caller)->table.count));
}

static NativeResult _map_keys(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjMap *map = AS_MAP(caller);
    ObjList *keys = new_list(NULL, 0);

    for (int i = 0; i < map->table.capacity; i++) {
        Entry *entry = &map->table.entries[i];
        if (entry->key != NULL)
            write_array(&keys->elems, entry->key);
    }

    OK(keys);
}

static NativeResult _map_values(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjMap *map = AS_MAP(caller);
    ObjList *values = new_list(NULL, 0);

    for (int i = 0; i < map->table.capacity; i++) {
        Entry *entry = &map->table.entries[i];
        if (entry->key != NULL)
            write_array(&values->elems, entry->value);
    }

    OK(values);
}

static NativeResult _map_new(int argc, UNUSED(Obj **argv), UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(0)
    OK(new_map(NULL, 0));
}

static ObjClass *_map_class = NULL;

ObjClass *get_map_class() {
    if (_map_class == NULL) {
        ObjClass *klass = new_builtin_class("Map");

        SET_BUILTIN_FN_STATIC("__new", _map_new);

        SET_BUILTIN_FN_METHOD("len", _map_len);
        SET_BUILTIN_FN_METHOD("keys", _map_keys);
        SET_BUILTIN_FN_METHOD("values", _map_values);

        _map_class = klass;
    }

    return _map_class;
}
