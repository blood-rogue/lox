#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>

#include "builtins.h"

static ObjClass *_sys_user_class = NULL;

static BuiltinResult _sys_user_by_name(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    struct passwd *user = getpwnam(argv_0->chars);

    ObjInstance *instance = new_instance(_sys_user_class);

    SET_STRING_FIELD("name", user->pw_name);
    SET_INT_FIELD("user_id", user->pw_uid);
    SET_INT_FIELD("group_id", user->pw_gid);
    SET_STRING_FIELD("real_name", user->pw_gecos);
    SET_STRING_FIELD("home_dir", user->pw_dir);
    SET_STRING_FIELD("shell", user->pw_shell);

    OK(instance);
}

static BuiltinResult _sys_user_by_id(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjInt, INT, 0)

    struct passwd *user = getpwuid(argv_0->value);

    ObjInstance *instance = new_instance(_sys_user_class);

    SET_STRING_FIELD("name", user->pw_name);
    SET_INT_FIELD("user_id", user->pw_uid);
    SET_INT_FIELD("group_id", user->pw_gid);
    SET_STRING_FIELD("real_name", user->pw_gecos);
    SET_STRING_FIELD("home_dir", user->pw_dir);
    SET_STRING_FIELD("shell", user->pw_shell);

    OK(instance);
}

ObjClass *get_sys_user_class() {
    if (_sys_user_class == NULL) {
        ObjClass *klass = new_builtin_class("User");

        SET_BUILTIN_FN_STATIC("by_name", _sys_user_by_name);
        SET_BUILTIN_FN_STATIC("by_id", _sys_user_by_id);

        _sys_user_class = klass;
    }

    return _sys_user_class;
}
