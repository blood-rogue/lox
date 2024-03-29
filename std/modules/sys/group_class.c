#include <grp.h>
#include <stdlib.h>
#include <unistd.h>

#include "native.h"

static ObjClass *_sys_group_class = NULL;

static NativeResult _sys_group_by_name(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    struct group *grp = getgrnam(argv_0->chars);

    ObjInstance *instance = new_instance(_sys_group_class);

    SET_STRING_FIELD("name", grp->gr_name);
    SET_INT_FIELD("group_id", grp->gr_gid);

    ObjList *members = new_list(NULL, 0);

    for (int i = 0; grp->gr_mem[i] != NULL; i++)
        write_array(&members->elems, AS_OBJ(new_string(grp->gr_mem[i], strlen(grp->gr_mem[i]))));

    SET_FIELD("members", members);

    OK(instance);
}

static NativeResult _sys_group_by_id(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjInt, INT, 0)

    struct group *grp = getgrgid(mpz_get_ui(argv_0->value));

    ObjInstance *instance = new_instance(_sys_group_class);

    SET_STRING_FIELD("name", grp->gr_name);
    SET_INT_FIELD("group_id", grp->gr_gid);

    ObjList *members = new_list(NULL, 0);

    for (int i = 0; grp->gr_mem[i] != NULL; i++)
        write_array(&members->elems, AS_OBJ(new_string(grp->gr_mem[i], strlen(grp->gr_mem[i]))));

    SET_FIELD("members", members);

    OK(instance);
}

ObjClass *get_sys_group_class() {
    if (_sys_group_class == NULL) {
        ObjClass *klass = new_native_class("Group");

        SET_NATIVE_FN_STATIC("by_name", _sys_group_by_name);
        SET_NATIVE_FN_STATIC("by_id", _sys_group_by_id);

        _sys_group_class = klass;
    }

    return _sys_group_class;
}
