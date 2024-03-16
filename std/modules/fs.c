#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "builtins.h"

static ObjModule *_fs_module = NULL;

static BuiltinResult _fs_dup2(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjInt, INT, 0)
    CHECK_ARG_TYPE(ObjInt, INT, 1)

    return OK(new_int(dup2(argv_0->value, argv_1->value)));
}

static BuiltinResult _fs_chown(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(3)
    CHECK_ARG_TYPE(ObjString, STRING, 0)
    CHECK_ARG_TYPE(ObjInt, INT, 1)
    CHECK_ARG_TYPE(ObjInt, INT, 2)

    if (chown(argv_0->chars, argv_1->value, argv_2->value) == 0)
        return OK(new_nil());

    return ERR("Could not change ownership");
}

static BuiltinResult _fs_link(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjString, STRING, 0)
    CHECK_ARG_TYPE(ObjString, STRING, 1)

    if (link(argv_0->chars, argv_1->chars) == 0)
        return OK(new_nil());

    return ERR("Could not create link.");
}

static BuiltinResult _fs_unlink(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    if (unlink(argv_0->chars) == 0)
        return OK(new_nil());

    return ERR("Could not delete link.");
}

static BuiltinResult _fs_symlink(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjString, STRING, 0)
    CHECK_ARG_TYPE(ObjString, STRING, 1)

    if (symlink(argv_0->chars, argv_1->chars) == 0)
        return OK(new_nil());

    return ERR("Could not create symlink.");
}

static BuiltinResult _fs_rmdir(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    if (rmdir(argv_0->chars) == 0)
        return OK(new_nil());

    return ERR("Could not remove directory.");
}

static void set_file_instance(int fd, ObjInstance *instance) {
    struct stat st;
    fstat(fd, &st);

    SET_INT_FIELD("fd", fd);
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
}

ObjModule *get_fs_module(int count, UNUSED(char **, parts)) {
    CHECK_PART_COUNT

    if (_fs_module == NULL) {
        ObjModule *module = new_module(new_string("fs", 2));

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

        ObjInstance *_stdin = new_instance(get_fs_file_class());
        set_file_instance(STDIN_FILENO, _stdin);

        ObjInstance *_stdout = new_instance(get_fs_file_class());
        set_file_instance(STDOUT_FILENO, _stdout);

        ObjInstance *_stderr = new_instance(get_fs_file_class());
        set_file_instance(STDERR_FILENO, _stderr);

        SET_MEMBER("STDIN", _stdin);
        SET_MEMBER("STDOUT", _stdout);
        SET_MEMBER("STDERR", _stderr);

        SET_MEMBER("File", get_fs_file_class());

        _fs_module = module;
    }

    return _fs_module;
}
