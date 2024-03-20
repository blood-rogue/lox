#include "builtins.h"
#include "toml.h"

static ObjModule *_serde_toml_module = NULL;

static Obj *toml_array_to_obj(toml_array_t *toml_array);

static Obj *toml_table_to_obj(toml_table_t *toml_table) {
    int len = toml_table_len(toml_table);
    ObjMap *map = new_map(NULL, 0);

    for (int idx = 0; idx < len; idx++) {
        int key_len;
        const char *key = toml_table_key(toml_table, idx, &key_len);

        Obj *key_obj = AS_OBJ(new_string(key, key_len));
        Obj *value_obj;

        toml_value_t value;
        toml_table_t *table;
        toml_array_t *array;

        if ((table = toml_table_table(toml_table, key)) != NULL) {
            value_obj = toml_table_to_obj(table);
        } else if ((array = toml_table_array(toml_table, key)) != NULL) {
            value_obj = toml_array_to_obj(array);
        } else if ((value = toml_table_timestamp(toml_table, key)).ok) {
            toml_timestamp_t *ts = value.u.ts;

            ObjClass *time_class = get_time_time_class();
            ObjInstance *instance = new_instance(time_class);

            SET_INT_FIELD("year", ts->year);
            SET_INT_FIELD("month", ts->month);
            SET_INT_FIELD("day", ts->day);
            SET_INT_FIELD("hour", ts->hour);
            SET_INT_FIELD("minute", ts->minute);
            SET_INT_FIELD("second", ts->second);

            value_obj = AS_OBJ(instance);
        } else if ((value = toml_table_bool(toml_table, key)).ok) {
            value_obj = AS_OBJ(new_bool(value.u.b));
        } else if ((value = toml_table_int(toml_table, key)).ok) {
            value_obj = AS_OBJ(new_int(value.u.i));
        } else if ((value = toml_table_double(toml_table, key)).ok) {
            value_obj = AS_OBJ(new_float(value.u.d));
        } else if ((value = toml_table_string(toml_table, key)).ok) {
            value_obj = AS_OBJ(new_string(value.u.s, value.u.sl));
        }

        table_set(&map->table, key_obj, value_obj);
    }

    return AS_OBJ(map);
}

static Obj *toml_array_to_obj(toml_array_t *toml_array) {
    ObjList *list = new_list(NULL, 0);

    for (int idx = 0; idx < toml_array_len(toml_array); idx++) {
        Obj *elem_obj;

        toml_value_t value;
        toml_table_t *table;
        toml_array_t *array;

        if ((table = toml_array_table(toml_array, idx)) != NULL) {
            elem_obj = toml_table_to_obj(table);
        } else if ((array = toml_array_array(toml_array, idx)) != NULL) {
            elem_obj = toml_array_to_obj(array);
        } else if ((value = toml_array_timestamp(toml_array, idx)).ok) {
            toml_timestamp_t *ts = value.u.ts;

            ObjClass *time_class = get_time_time_class();
            ObjInstance *instance = new_instance(time_class);

            SET_INT_FIELD("year", ts->year);
            SET_INT_FIELD("month", ts->month);
            SET_INT_FIELD("day", ts->day);
            SET_INT_FIELD("hour", ts->hour);
            SET_INT_FIELD("minute", ts->minute);
            SET_INT_FIELD("second", ts->second);

            elem_obj = AS_OBJ(instance);
        } else if ((value = toml_array_bool(toml_array, idx)).ok) {
            elem_obj = AS_OBJ(new_bool(value.u.b));
        } else if ((value = toml_array_int(toml_array, idx)).ok) {
            elem_obj = AS_OBJ(new_int(value.u.i));
        } else if ((value = toml_array_double(toml_array, idx)).ok) {
            elem_obj = AS_OBJ(new_float(value.u.d));
        } else if ((value = toml_array_string(toml_array, idx)).ok) {
            elem_obj = AS_OBJ(new_string(value.u.s, value.u.sl));
        }

        write_array(&list->elems, elem_obj);
    }

    return AS_OBJ(list);
}

static BuiltinResult _serde_toml_deserialize(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    char errbuf[200];
    toml_table_t *toml = toml_parse(argv_0->chars, errbuf, sizeof(errbuf));

    Obj *obj = toml_table_to_obj(toml);

    toml_free(toml);
    OK(obj);
}

ObjModule *get_serde_toml_module() {
    if (_serde_toml_module == NULL) {
        ObjModule *module = new_module("toml");

        SET_BUILTIN_FN_MEMBER("deserialize", _serde_toml_deserialize);

        _serde_toml_module = module;
    }

    return _serde_toml_module;
}
