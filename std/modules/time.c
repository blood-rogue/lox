#include <time.h>

#include "builtins.h"

#define SET_TABLE(name) SET_MODULE_TABLE(time, name)

static ObjClass *__time_time_class = NULL;

static BuiltinResult __time_time_local_now(int argc, UNUSED(Obj **, argv), UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(0)

    int64_t now = time(NULL);
    struct tm *_tm = localtime(&now);

    ObjInstance *_instance = new_instance(__time_time_class);

    SET_FIELD(raw, new_int(now));
    SET_FIELD(year, new_int(_tm->tm_year));
    SET_FIELD(month, new_int(_tm->tm_mon));
    SET_FIELD(day, new_int(_tm->tm_mday));
    SET_FIELD(hour, new_int(_tm->tm_hour));
    SET_FIELD(minute, new_int(_tm->tm_min));
    SET_FIELD(second, new_int(_tm->tm_sec));

    return OK(_instance);
}

static BuiltinResult __time_time_utc_now(int argc, UNUSED(Obj **, argv), UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(0)

    int64_t now = time(NULL);
    struct tm *_tm = gmtime(&now);

    ObjInstance *_instance = new_instance(__time_time_class);

    SET_FIELD(raw, new_int(now));
    SET_FIELD(year, new_int(_tm->tm_year));
    SET_FIELD(month, new_int(_tm->tm_mon));
    SET_FIELD(day, new_int(_tm->tm_mday));
    SET_FIELD(hour, new_int(_tm->tm_hour));
    SET_FIELD(minute, new_int(_tm->tm_min));
    SET_FIELD(second, new_int(_tm->tm_sec));

    return OK(_instance);
}

static BuiltinResult __time_time_to_str(int argc, Obj **argv, Obj *caller) {
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

static ObjModule *__time_module = NULL;

ObjModule *get_time_module() {
    if (__time_module == NULL) {
        ObjModule *module = new_module(new_string("time", 4));

        SET_TABLE(now);
        SET_TABLE(clock);
        SET_TABLE(difftime);
        SET_TABLE(to_utc_str);
        SET_TABLE(to_local_str);

        table_set(
            &module->globals,
            AS_OBJ(new_string("CLOCKS_PER_SEC", 14)),
            AS_OBJ(new_int(CLOCKS_PER_SEC)));

        if (__time_time_class == NULL) {
            ObjClass *klass = new_class(new_string("Time", 4));
            klass->is_builtin = true;

            table_set(
                &klass->statics,
                AS_OBJ(new_string("local_now", 9)),
                AS_OBJ(new_builtin_function(__time_time_local_now, "local_now")));
            table_set(
                &klass->statics,
                AS_OBJ(new_string("utc_now", 7)),
                AS_OBJ(new_builtin_function(__time_time_utc_now, "utc_now")));

            table_set(
                &klass->methods,
                AS_OBJ(new_string("to_str", 6)),
                AS_OBJ(new_builtin_function(__time_time_to_str, "to_str")));

            __time_time_class = klass;
        }

        table_set(&module->globals, AS_OBJ(new_string("Time", 4)), AS_OBJ(__time_time_class));

        __time_module = module;
    }

    return __time_module;
}
