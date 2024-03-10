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

    return OK(instance);
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

    return OK(instance);
}

static BuiltinResult _time_time_to_str(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(STRING, 0)

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

    strftime(strf, 70, AS_STRING(argv[1])->chars, &t);

    return OK(take_string(strf, (int)strlen(strf)));
}

static BuiltinResult _time_now(int argc, UNUSED(Obj **, argv), UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(0)
    return OK(new_int(time(NULL)));
}

static BuiltinResult _time_clock(int argc, UNUSED(Obj **, argv), UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(0)
    return OK(new_int(clock()));
}

static BuiltinResult _time_difftime(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(INT, 0)
    CHECK_ARG_TYPE(INT, 1)

    return OK(new_int(difftime(AS_INT(argv[0])->value, AS_INT(argv[1])->value)));
}

static BuiltinResult _time_to_utc_str(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)

    CHECK_ARG_TYPE(INT, 0)
    CHECK_ARG_TYPE(STRING, 1)

    char *strf = malloc(70);
    struct tm *t = gmtime(&AS_INT(argv[0])->value);
    strftime(strf, 70, AS_STRING(argv[1])->chars, t);

    return OK(take_string(strf, strlen(strf)));
}

static BuiltinResult _time_to_local_str(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)

    CHECK_ARG_TYPE(INT, 0)
    CHECK_ARG_TYPE(STRING, 1)

    char *strf = malloc(70);
    struct tm *t = localtime(&AS_INT(argv[0])->value);
    strftime(strf, 70, AS_STRING(argv[1])->chars, t);

    return OK(take_string(strf, strlen(strf)));
}

static ObjModule *_time_module = NULL;

ObjModule *get_time_module(int count, UNUSED(char **, parts)) {
    CHECK_PART_COUNT(0)

    if (_time_module == NULL) {
        ObjModule *module = new_module(new_string("time", 4));

        SET_BUILTIN_FN_MEMBER("now", _time_now);
        SET_BUILTIN_FN_MEMBER("clock", _time_clock);
        SET_BUILTIN_FN_MEMBER("difftime", _time_difftime);
        SET_BUILTIN_FN_MEMBER("to_utc_str", _time_to_utc_str);
        SET_BUILTIN_FN_MEMBER("to_local_str", _time_to_local_str);
        SET_INT_MEMBER("CLOCKS_PER_SEC", CLOCKS_PER_SEC);

        if (_time_time_class == NULL) {
            ObjClass *klass = new_class(new_string("Time", 4));
            klass->is_builtin = true;

            SET_BUILTIN_FN_STATIC("local_now", _time_time_local_now);
            SET_BUILTIN_FN_STATIC("utc_now", _time_time_utc_now);

            SET_BUILTIN_FN_METHOD("to_str", _time_time_to_str);

            _time_time_class = klass;
        }

        SET_MEMBER("Time", _time_time_class);

        _time_module = module;
    }

    return _time_module;
}
