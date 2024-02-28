#include <time.h>

#include "builtins.h"

#define DEFINE_MEMBER(func)                                                                        \
    static BuiltinResult __##func(int argc, UNUSED(Obj **, argv), UNUSED(Obj *, caller)) {         \
        CHECK_ARG_COUNT(0)                                                                         \
        return OK(new_int(func(NULL)));                                                            \
    }

#define SET_TABLE(name) SET_MODULE_TABLE(time, name)

static BuiltinResult _time_time(int argc, UNUSED(Obj **, argv), UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(0)
    return OK(new_int(time(NULL)));
}

static BuiltinResult _time_clock(int argc, UNUSED(Obj **, argv), UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(0)
    return OK(new_int(clock()));
}

static BuiltinResult _time_difftime(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)

    if (!IS_INT(argv[0]) && !IS_INT(argv[1]))
        return ERR("Only INT's can be diffed as time.");

    return OK(new_int(difftime(AS_INT(argv[0])->value, AS_INT(argv[1])->value)));
}

static BuiltinResult _time_to_utc_str(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)

    if (!IS_INT(argv[0]))
        return ERR("Only INT can be converted to UTC time.");

    if (!IS_STRING(argv[1]))
        return ERR("Time format can only be a string.");

    char *strf = malloc(70);
    struct tm *t = gmtime(&AS_INT(argv[0])->value);
    strftime(strf, 70, AS_STRING(argv[1])->chars, t);

    return OK(take_string(strf, strlen(strf)));
}

static BuiltinResult _time_to_local_str(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)

    if (!IS_INT(argv[0]))
        return ERR("Only INT can be converted to Local time.");

    if (!IS_STRING(argv[1]))
        return ERR("Time format can only be a string.");

    char *strf = malloc(70);
    struct tm *t = localtime(&AS_INT(argv[0])->value);
    strftime(strf, 70, AS_STRING(argv[1])->chars, t);

    return OK(take_string(strf, strlen(strf)));
}

ObjModule *get_time_module() {
    ObjModule *module = new_module(new_string("time", 4));

    SET_TABLE(time);
    SET_TABLE(clock);
    SET_TABLE(difftime);
    SET_TABLE(to_utc_str);
    SET_TABLE(to_local_str);

    table_set(
        &module->globals,
        AS_OBJ(new_string("CLOCKS_PER_SEC", 14)),
        AS_OBJ(new_int(CLOCKS_PER_SEC)));

    return module;
}
