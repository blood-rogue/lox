#include <json-c/json.h>
#include <unistr.h>

#include "builtins.h"

static ObjModule *_serde_json_module = NULL;

static json_object *obj_to_json(Obj *obj, char **err) {
    switch (obj->type) {
        case OBJ_MAP:
            {
                ObjMap *map = AS_MAP(obj);
                json_object *json_map = json_object_new_object();
                int count = map->table.count;
                Obj *value;

                for (int i = 0; i < map->table.capacity && count > 0; i++) {
                    Entry *entry = &map->table.entries[i];

                    if (entry->key == NULL)
                        continue;

                    if (!IS_STRING(entry->key)) {
                        *err = "Map keys must be STRING.";
                        return NULL;
                    }

                    table_get(&map->table, entry->key, &value);
                    json_object *json_value = obj_to_json(value, err);

                    if (json_value == NULL) {
                        return NULL;
                    }

                    json_object_object_add(json_map, AS_STRING(entry->key)->chars, json_value);

                    count--;
                }

                return json_map;
            }
        case OBJ_LIST:
            {
                ObjList *list = AS_LIST(obj);
                json_object *json_list = json_object_new_array();

                for (int i = 0; i < list->elems.count; i++) {
                    json_object *json_elem = obj_to_json(list->elems.values[i], err);

                    if (json_elem == NULL) {
                        return NULL;
                    }

                    json_object_array_add(json_list, json_elem);
                }

                return json_list;
            }
        case OBJ_BOOL:
            return json_object_new_boolean(AS_BOOL(obj)->value);
        case OBJ_INT:
            return json_object_new_int64(AS_INT(obj)->value);
        case OBJ_FLOAT:
            return json_object_new_double(AS_FLOAT(obj)->value);
        case OBJ_NIL:
            return json_object_new_null();
        case OBJ_STRING:
            return json_object_new_string_len(AS_STRING(obj)->chars, AS_STRING(obj)->raw_length);
        case OBJ_CHAR:
            {
                uint8_t s[4] = {0};
                int len = u8_uctomb(s, AS_CHAR(obj)->value, 4);
                return json_object_new_string_len((char *)s, len);
            }
        default:
            *err = "Invalid JSON value";
            return NULL;
    }
}

static NativeResult _serde_json_serialize(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjInt, INT, 1)

    char *err = NULL;
    json_object *json_obj = obj_to_json(argv[0], &err);

    if (json_obj == NULL)
        ERR("JSON error: %s", err)

    const char *json = json_object_to_json_string_ext(json_obj, argv_1->value);
    ObjString *str = new_string(json, strlen(json));

    json_object_put(json_obj);

    OK(str);
}

static Obj *json_to_obj(json_object *json) {
    json_type type = json_object_get_type(json);
    switch (type) {
        case json_type_boolean:
            return AS_OBJ(new_bool(json_object_get_boolean(json)));
        case json_type_null:
            return AS_OBJ(new_nil());
        case json_type_double:
            return AS_OBJ(new_float(json_object_get_double(json)));
        case json_type_int:
            return AS_OBJ(new_int(json_object_get_int64(json)));
        case json_type_string:
            return AS_OBJ(
                new_string(json_object_get_string(json), json_object_get_string_len(json)));
        case json_type_array:
            {
                ObjList *list = new_list(NULL, 0);
                for (size_t i = 0; i < json_object_array_length(json); i++) {
                    json_object *el = json_object_array_get_idx(json, i);
                    write_array(&list->elems, json_to_obj(el));
                }

                return AS_OBJ(list);
            }
        case json_type_object:
            {
                ObjMap *map = new_map(NULL, 0);

                json_object_object_foreach(json, key, val) {
                    table_set(
                        &map->table,
                        AS_OBJ(new_string(key, strlen(key))),
                        AS_OBJ(json_to_obj(val)));
                }

                return AS_OBJ(map);
            }
    }

    return NULL;
}

static NativeResult _serde_json_deserialize(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    json_object *json = json_tokener_parse(argv_0->chars);

    Obj *obj = json_to_obj(json);
    json_object_put(json);

    OK(obj);
}

ObjModule *get_serde_json_module() {
    if (_serde_json_module == NULL) {
        ObjModule *module = new_module("json");

        SET_BUILTIN_FN_MEMBER("serialize", _serde_json_serialize);
        SET_BUILTIN_FN_MEMBER("deserialize", _serde_json_deserialize);

        SET_INT_MEMBER("TO_STRING_PLAIN", JSON_C_TO_STRING_PLAIN);
        SET_INT_MEMBER("TO_STRING_SPACED", JSON_C_TO_STRING_SPACED);
        SET_INT_MEMBER("TO_STRING_PRETTY", JSON_C_TO_STRING_PRETTY);
        SET_INT_MEMBER("TO_STRING_PRETTY_TAB", JSON_C_TO_STRING_PRETTY_TAB);
        SET_INT_MEMBER("TO_STRING_NOZERO", JSON_C_TO_STRING_NOZERO);
        SET_INT_MEMBER("TO_STRING_NOSLASHESCAPE", JSON_C_TO_STRING_NOSLASHESCAPE);

        _serde_json_module = module;
    }

    return _serde_json_module;
}
