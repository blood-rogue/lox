#include <time.h>

#include "builtins.h"

static ObjModule *_time_module = NULL;

static BuiltinResult _time_now(int argc, UNUSED(Obj **, argv), UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(0)
    OK(new_int(time(NULL)));
}

static BuiltinResult _time_clock(int argc, UNUSED(Obj **, argv), UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(0)
    OK(new_int(clock()));
}

static BuiltinResult _time_difftime(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)

    CHECK_ARG_TYPE(ObjInt, INT, 0)
    CHECK_ARG_TYPE(ObjInt, INT, 1)

    OK(new_int(difftime(argv_0->value, argv_1->value)));
}

static BuiltinResult _time_to_utc_str(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)

    CHECK_ARG_TYPE(ObjInt, INT, 0)
    CHECK_ARG_TYPE(ObjString, STRING, 1)

    char *strf = malloc(70);
    struct tm *t = gmtime(&argv_0->value);
    strftime(strf, 70, argv_1->chars, t);

    OK(take_string(strf, strlen(strf)));
}

static BuiltinResult _time_to_local_str(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)

    CHECK_ARG_TYPE(ObjInt, INT, 0)
    CHECK_ARG_TYPE(ObjString, STRING, 1)

    char *strf = malloc(70);
    struct tm *t = localtime(&argv_0->value);
    strftime(strf, 70, argv_1->chars, t);

    OK(take_string(strf, strlen(strf)));
}

ObjModule *get_time_module(int count, UNUSED(char **, parts)) {
    CHECK_PART_COUNT

    if (_time_module == NULL) {
        ObjModule *module = new_module(new_string("time", 4));

        SET_BUILTIN_FN_MEMBER("now", _time_now);
        SET_BUILTIN_FN_MEMBER("clock", _time_clock);
        SET_BUILTIN_FN_MEMBER("difftime", _time_difftime);
        SET_BUILTIN_FN_MEMBER("to_utc_str", _time_to_utc_str);
        SET_BUILTIN_FN_MEMBER("to_local_str", _time_to_local_str);
        SET_INT_MEMBER("CLOCKS_PER_SEC", CLOCKS_PER_SEC);

        SET_MEMBER("Time", get_time_time_class());

        _time_module = module;
    }

    return _time_module;
}
