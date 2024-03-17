#include <linux/limits.h>
#include <stdlib.h>
#include <unistd.h>

#include "builtins.h"

static ObjModule *_sys_module = NULL;

static BuiltinResult _sys_getenv(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    char *env = getenv(argv_0->chars);

    if (env != NULL)
        OK(new_string(env, strlen(env)));

    ERR("No environment variable found with the name '%s'.", argv_0->chars)
}

static BuiltinResult _sys_getcwd(int argc, UNUSED(Obj **, argv), UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(0)

    char *cwd = getcwd(NULL, 0);
    if (cwd != NULL)
        OK(take_string(cwd, strlen(cwd)));

    ERR("Could not read current working dir.")
}

static BuiltinResult _sys_getuid(int argc, UNUSED(Obj **, argv), UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(0)

    OK(new_int(getuid()));
}

static BuiltinResult _sys_getgid(int argc, UNUSED(Obj **, argv), UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(0)

    OK(new_int(getgid()));
}

static BuiltinResult _sys_setuid(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjInt, INT, 0)

    if (setuid(argv_0->value) == 0)
        OK(new_nil());

    ERR("Could not set uid.")
}

static BuiltinResult _sys_setgid(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjInt, INT, 0)

    if (setgid(argv_0->value) == 0)
        OK(new_nil());

    ERR("Could not set gid.")
}

static BuiltinResult _sys_chdir(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    if (chdir(argv_0->chars) == 0)
        OK(new_nil());

    ERR("Could not change working directory of current process to '%s'.", argv_0->chars)
}

static BuiltinResult _sys_getpid(int argc, UNUSED(Obj **, argv), UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(0)

    OK(new_int(getpid()));
}

ObjModule *get_sys_module(int count, UNUSED(char **, parts)) {
    CHECK_PART_COUNT

    if (_sys_module == NULL) {
        ObjModule *module = new_module(new_string("sys", 3));

        ObjList *_sys_environ = new_list(NULL, 0);
        for (int i = 0; __environ[i] != NULL; i++) {
            write_array(
                &_sys_environ->elems, AS_OBJ(new_string(__environ[i], strlen(__environ[i]))));
        }

        SET_MEMBER("ENVIRON", _sys_environ);
        SET_BUILTIN_FN_MEMBER("getenv", _sys_getenv);
        SET_BUILTIN_FN_MEMBER("getcwd", _sys_getcwd);
        SET_BUILTIN_FN_MEMBER("getuid", _sys_getuid);
        SET_BUILTIN_FN_MEMBER("getgid", _sys_getgid);
        SET_BUILTIN_FN_MEMBER("setuid", _sys_setuid);
        SET_BUILTIN_FN_MEMBER("setgid", _sys_setgid);
        SET_BUILTIN_FN_MEMBER("chdir", _sys_chdir);
        SET_BUILTIN_FN_MEMBER("getpid", _sys_getpid);

        SET_MEMBER("User", get_sys_user_class());
        SET_MEMBER("Group", get_sys_group_class());

        _sys_module = module;
    }

    return _sys_module;
}
