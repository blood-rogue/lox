#include <time.h>

#include "builtins.h"

static ObjClass *_time_time_class = NULL;

static BuiltinResult _time_time_local_now(int argc, UNUSED(Obj **, argv), UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(0)

    int64_t now = time(NULL);
    struct tm *_tm = localtime(&now);

    ObjInstance *instance = new_instance(_time_time_class);

    SET_INT_FIELD("year", _tm->tm_year);
    SET_INT_FIELD("month", _tm->tm_mon);
    SET_INT_FIELD("day", _tm->tm_mday);
    SET_INT_FIELD("hour", _tm->tm_hour);
    SET_INT_FIELD("minute", _tm->tm_min);
    SET_INT_FIELD("second", _tm->tm_sec);

    OK(instance);
}

static BuiltinResult _time_time_utc_now(int argc, UNUSED(Obj **, argv), UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(0)

    int64_t now = time(NULL);
    struct tm *_tm = gmtime(&now);

    ObjInstance *instance = new_instance(_time_time_class);

    SET_INT_FIELD("year", _tm->tm_year);
    SET_INT_FIELD("month", _tm->tm_mon);
    SET_INT_FIELD("day", _tm->tm_mday);
    SET_INT_FIELD("hour", _tm->tm_hour);
    SET_INT_FIELD("minute", _tm->tm_min);
    SET_INT_FIELD("second", _tm->tm_sec);

    OK(instance);
}

static BuiltinResult _time_time_to_str(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    ObjInstance *_instance = AS_INSTANCE(caller);

    char *strf = malloc(70);
    struct tm t;

    Obj *field;

    table_get(&_instance->fields, AS_OBJ(new_string("year", 4)), &field);
    t.tm_year = AS_INT(field)->value;

    table_get(&_instance->fields, AS_OBJ(new_string("month", 5)), &field);
    t.tm_mon = AS_INT(field)->value;

    table_get(&_instance->fields, AS_OBJ(new_string("day", 3)), &field);
    t.tm_mday = AS_INT(field)->value;

    table_get(&_instance->fields, AS_OBJ(new_string("hour", 4)), &field);
    t.tm_hour = AS_INT(field)->value;

    table_get(&_instance->fields, AS_OBJ(new_string("minute", 6)), &field);
    t.tm_min = AS_INT(field)->value;

    table_get(&_instance->fields, AS_OBJ(new_string("second", 6)), &field);
    t.tm_sec = AS_INT(field)->value;

    strftime(strf, 70, argv_0->chars, &t);

    OK(take_string(strf, (int)strlen(strf)));
}

ObjClass *get_time_time_class() {
    if (_time_time_class == NULL) {
        ObjClass *klass = new_class(new_string("Time", 4));
        klass->is_builtin = true;

        SET_BUILTIN_FN_STATIC("local_now", _time_time_local_now);
        SET_BUILTIN_FN_STATIC("utc_now", _time_time_utc_now);
        SET_BUILTIN_FN_METHOD("to_str", _time_time_to_str);

        _time_time_class = klass;
    }

    return _time_time_class;
}
