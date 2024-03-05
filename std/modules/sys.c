#include <grp.h>
#include <linux/limits.h>
#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>

#include "builtins.h"

static ObjModule *_sys_module = NULL;
static ObjClass *_sys_user_class = NULL;
static ObjClass *_sys_group_class = NULL;

static BuiltinResult _sys_getenv(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(STRING, 0)

    char *env = getenv(AS_STRING(argv[0])->chars);

    if (env != NULL)
        return OK(new_string(env, strlen(env)));

    return ERR("No environment variable found with the specified name.");
}

static BuiltinResult _sys_getcwd(int argc, UNUSED(Obj **, argv), UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(0)

    char *cwd = getcwd(NULL, 0);
    if (cwd != NULL)
        return OK(take_string(cwd, strlen(cwd)));

    return ERR("Could not read current working dir.");
}

static BuiltinResult _sys_getuid(int argc, UNUSED(Obj **, argv), UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(0)

    return OK(new_int(getuid()));
}

static BuiltinResult _sys_getgid(int argc, UNUSED(Obj **, argv), UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(0)

    return OK(new_int(getgid()));
}

static BuiltinResult _sys_setuid(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(INT, 0)

    if (setuid(AS_INT(argv[0])->value) == 0)
        return OK(new_nil());

    return ERR("Could not set uid.");
}

static BuiltinResult _sys_setgid(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(INT, 0)

    if (setgid(AS_INT(argv[0])->value) == 0)
        return OK(new_nil());

    return ERR("Could not set gid.");
}

static BuiltinResult _sys_chdir(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(STRING, 0)

    if (chdir(AS_STRING(argv[0])->chars) == 0)
        return OK(new_nil());

    return ERR("Could not change working directory of current process.");
}

static BuiltinResult _sys_getpid(int argc, UNUSED(Obj **, argv), UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(0)

    return OK(new_int(getpid()));
}

static BuiltinResult _sys_user_by_name(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(STRING, 0)

    struct passwd *user = getpwnam(AS_STRING(argv[0])->chars);

    ObjInstance *instance = new_instance(_sys_user_class);

    SET_STRING_FIELD("name", user->pw_name);
    SET_INT_FIELD("user_id", user->pw_uid);
    SET_INT_FIELD("group_id", user->pw_gid);
    SET_STRING_FIELD("real_name", user->pw_gecos);
    SET_STRING_FIELD("home_dir", user->pw_dir);
    SET_STRING_FIELD("shell", user->pw_shell);

    return OK(instance);
}

static BuiltinResult _sys_user_by_id(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(INT, 0)

    struct passwd *user = getpwuid(AS_INT(argv[0])->value);

    ObjInstance *instance = new_instance(_sys_user_class);

    SET_STRING_FIELD("name", user->pw_name);
    SET_INT_FIELD("user_id", user->pw_uid);
    SET_INT_FIELD("group_id", user->pw_gid);
    SET_STRING_FIELD("real_name", user->pw_gecos);
    SET_STRING_FIELD("home_dir", user->pw_dir);
    SET_STRING_FIELD("shell", user->pw_shell);

    return OK(instance);
}

static BuiltinResult _sys_group_by_name(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(STRING, 0)

    struct group *grp = getgrnam(AS_STRING(argv[0])->chars);

    ObjInstance *instance = new_instance(_sys_user_class);

    SET_STRING_FIELD("name", grp->gr_name);
    SET_INT_FIELD("group_id", grp->gr_gid);

    ObjList *members = new_list(NULL, 0);

    for (int i = 0; grp->gr_mem[i] != NULL; i++)
        write_array(&members->elems, AS_OBJ(new_string(grp->gr_mem[i], strlen(grp->gr_mem[i]))));

    SET_FIELD("members", members);

    return OK(instance);
}

static BuiltinResult _sys_group_by_id(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(INT, 0)

    struct group *grp = getgrgid(AS_INT(argv[0])->value);

    ObjInstance *instance = new_instance(_sys_user_class);

    SET_STRING_FIELD("name", grp->gr_name);
    SET_INT_FIELD("group_id", grp->gr_gid);

    ObjList *members = new_list(NULL, 0);

    for (int i = 0; grp->gr_mem[i] != NULL; i++)
        write_array(&members->elems, AS_OBJ(new_string(grp->gr_mem[i], strlen(grp->gr_mem[i]))));

    SET_FIELD("members", members);

    return OK(instance);
}

ObjModule *get_sys_module() {
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

        if (_sys_user_class == NULL) {
            ObjClass *klass = new_class(new_string("User", 4));

            SET_BUILTIN_FN_STATIC("by_name", _sys_user_by_name);
            SET_BUILTIN_FN_STATIC("by_id", _sys_user_by_id);

            _sys_user_class = klass;
        }

        if (_sys_group_class == NULL) {
            ObjClass *klass = new_class(new_string("Group", 5));

            SET_BUILTIN_FN_STATIC("by_name", _sys_group_by_name);
            SET_BUILTIN_FN_STATIC("by_id", _sys_group_by_id);

            _sys_group_class = klass;
        }

        SET_MEMBER("User", _sys_user_class);
        SET_MEMBER("Group", _sys_group_class);

        _sys_module = module;
    }

    return _sys_module;
}
