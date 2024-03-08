#include "builtins.h"

BuiltinResult _map_len(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(new_int(AS_MAP(caller)->table.count));
}

BuiltinResult _map_keys(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjMap *map = AS_MAP(caller);
    ObjList *keys = new_list(NULL, 0);

    for (int i = 0; i < map->table.capacity; i++) {
        Entry *entry = &map->table.entries[i];
        if (entry->key != NULL)
            write_array(&keys->elems, entry->key);
    }

    return OK(keys);
}

BuiltinResult _map_values(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjMap *map = AS_MAP(caller);
    ObjList *values = new_list(NULL, 0);

    for (int i = 0; i < map->table.capacity; i++) {
        Entry *entry = &map->table.entries[i];
        if (entry->key != NULL)
            write_array(&values->elems, entry->value);
    }

    return OK(values);
}

BuiltinTable *map_methods() {
    BuiltinTable *table = malloc(sizeof(BuiltinTable));
    init_method_table(table, 8);

    SET_BLTIN_METHOD("len", _map_len);
    SET_BLTIN_METHOD("keys", _map_keys);
    SET_BLTIN_METHOD("values", _map_values);

    return table;
}
