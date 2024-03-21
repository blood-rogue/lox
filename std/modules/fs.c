#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "builtins.h"

static ObjModule *_fs_module = NULL;

static NativeResult _fs_dup2(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjInt, INT, 0)
    CHECK_ARG_TYPE(ObjInt, INT, 1)

    OK(new_int(dup2(argv_0->value, argv_1->value)));
}

static NativeResult _fs_chown(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(3)
    CHECK_ARG_TYPE(ObjString, STRING, 0)
    CHECK_ARG_TYPE(ObjInt, INT, 1)
    CHECK_ARG_TYPE(ObjInt, INT, 2)

    if (chown(argv_0->chars, argv_1->value, argv_2->value) == 0)
        OK(new_nil());

    ERR("Could not change ownership of file '%.*s'", argv_0->raw_length, argv_0->chars)
}

static NativeResult _fs_link(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjString, STRING, 0)
    CHECK_ARG_TYPE(ObjString, STRING, 1)

    if (link(argv_0->chars, argv_1->chars) == 0)
        OK(new_nil());

    ERR("Could not create link from '%.*s' to '%.*s'.",
        argv_0->raw_length,
        argv_0->chars,
        argv_1->raw_length,
        argv_1->chars)
}

static NativeResult _fs_unlink(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    if (unlink(argv_0->chars) == 0)
        OK(new_nil());

    ERR("Could not delete link '%.*s'.", argv_0->raw_length, argv_0->chars)
}

static NativeResult _fs_symlink(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjString, STRING, 0)
    CHECK_ARG_TYPE(ObjString, STRING, 1)

    if (symlink(argv_0->chars, argv_1->chars) == 0)
        OK(new_nil());

    ERR("Could not create symlink from '%.*s' to '%.*s'.",
        argv_0->raw_length,
        argv_0->chars,
        argv_1->raw_length,
        argv_1->chars)
}

static NativeResult _fs_rmdir(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    if (rmdir(argv_0->chars) == 0)
        OK(new_nil());

    ERR("Could not remove directory '%.*s'.", argv_0->raw_length, argv_0->chars)
}

static void free_file(void *file) { fclose(file); }

static ObjInstance *set_file_instance(FILE *file, ObjInstance *instance) {
    struct stat st;
    fstat(file->_fileno, &st);

    SET_FIELD("$$internal", new_native_struct(file, free_file));
    SET_INT_FIELD("fd", file->_fileno);
    SET_INT_FIELD("device", st.st_dev);
    SET_INT_FIELD("inode", st.st_ino);
    SET_INT_FIELD("mode", st.st_mode);
    SET_INT_FIELD("num_links", st.st_nlink);
    SET_INT_FIELD("uid", st.st_uid);
    SET_INT_FIELD("gid", st.st_gid);
    SET_INT_FIELD("size", st.st_size);
    SET_INT_FIELD("access_time", st.st_atime);
    SET_INT_FIELD("modification_time", st.st_mtime);
    SET_INT_FIELD("status_change_time", st.st_ctime);

    return instance;
}

ObjModule *get_fs_module(int count, UNUSED(char **parts)) {
    CHECK_PART_COUNT

    if (_fs_module == NULL) {
        ObjModule *module = new_module("fs");

        SET_INT_MEMBER("CREATE", O_CREAT);
        SET_INT_MEMBER("EXCLUSIVE", O_EXCL);
        SET_INT_MEMBER("NO_C_TTY", O_NOCTTY);
        SET_INT_MEMBER("TRUNCATE", O_TRUNC);
        SET_INT_MEMBER("APPEND", O_APPEND);
        SET_INT_MEMBER("ACCESS_MODE", O_ACCMODE);
        SET_INT_MEMBER("READ_ONLY", O_RDONLY);
        SET_INT_MEMBER("WRITE_ONLY", O_WRONLY);
        SET_INT_MEMBER("READ_WRITE", O_RDWR);

        SET_INT_MEMBER("SEEK_END", SEEK_END);
        SET_INT_MEMBER("SEEK_SET", SEEK_SET);
        SET_INT_MEMBER("SEEK_CUR", SEEK_CUR);

        SET_BUILTIN_FN_MEMBER("dup2", _fs_dup2);
        SET_BUILTIN_FN_MEMBER("chown", _fs_chown);
        SET_BUILTIN_FN_MEMBER("link", _fs_link);
        SET_BUILTIN_FN_MEMBER("unlink", _fs_unlink);
        SET_BUILTIN_FN_MEMBER("symlink", _fs_symlink);
        SET_BUILTIN_FN_MEMBER("rmdir", _fs_rmdir);

        SET_MEMBER("STDIN", set_file_instance(stdin, new_instance(get_fs_file_class())));
        SET_MEMBER("STDOUT", set_file_instance(stdout, new_instance(get_fs_file_class())));
        SET_MEMBER("STDERR", set_file_instance(stderr, new_instance(get_fs_file_class())));

        SET_MEMBER("File", get_fs_file_class());

        _fs_module = module;
    }

    return _fs_module;
}
