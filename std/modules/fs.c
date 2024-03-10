#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "builtins.h"

static ObjModule *_fs_module = NULL;
static ObjClass *_fs_file_class = NULL;

static BuiltinResult _fs_dup2(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(INT, 0)
    CHECK_ARG_TYPE(INT, 1)

    return OK(new_int(dup2(AS_INT(argv[0])->value, AS_INT(argv[1])->value)));
}

static BuiltinResult _fs_chown(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(3)
    CHECK_ARG_TYPE(STRING, 0)
    CHECK_ARG_TYPE(INT, 1)
    CHECK_ARG_TYPE(INT, 2)

    if (chown(AS_STRING(argv[0])->chars, AS_INT(argv[1])->value, AS_INT(argv[2])->value) == 0)
        return OK(new_nil());

    return ERR("Could not change ownership");
}

static BuiltinResult _fs_link(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(STRING, 0)
    CHECK_ARG_TYPE(STRING, 1)

    if (link(AS_STRING(argv[0])->chars, AS_STRING(argv[1])->chars) == 0)
        return OK(new_nil());

    return ERR("Could not create link.");
}

static BuiltinResult _fs_unlink(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(STRING, 0)

    if (unlink(AS_STRING(argv[0])->chars) == 0)
        return OK(new_nil());

    return ERR("Could not delete link.");
}

static BuiltinResult _fs_symlink(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(STRING, 0)
    CHECK_ARG_TYPE(STRING, 1)

    if (symlink(AS_STRING(argv[0])->chars, AS_STRING(argv[1])->chars) == 0)
        return OK(new_nil());

    return ERR("Could not create symlink.");
}

static BuiltinResult _fs_rmdir(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(STRING, 0)

    if (rmdir(AS_STRING(argv[0])->chars) == 0)
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

static BuiltinResult _fs_file_init(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(INT, 0)

    int fd = AS_INT(argv[0])->value;
    ObjInstance *instance = AS_INSTANCE(caller);

    set_file_instance(fd, instance);

    return OK(new_nil());
}

static BuiltinResult _fs_file_open(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(STRING, 0)
    CHECK_ARG_TYPE(INT, 1)

    int fd = open(AS_STRING(argv[0])->chars, AS_INT(argv[1])->value);
    ObjInstance *instance = new_instance(_fs_file_class);

    set_file_instance(fd, instance);

    return OK(instance);
}

static BuiltinResult _fs_file_create(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(STRING, 0)

    int fd =
        open(AS_STRING(argv[0])->chars, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP);
    ObjInstance *instance = new_instance(_fs_file_class);

    set_file_instance(fd, instance);

    return OK(instance);
}

static BuiltinResult _fs_file_read(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInstance *instance = AS_INSTANCE(caller);

    Obj *fd_obj;
    table_get(&instance->fields, AS_OBJ(new_string("fd", 2)), &fd_obj);

    int64_t fd = AS_INT(fd_obj)->value;
    off_t fsize = lseek(fd, 0, SEEK_END);

    char *buf = malloc(fsize);

    lseek(fd, 0, SEEK_SET);

    if (read(fd, buf, fsize) == fsize) {
        return OK(take_string(buf, fsize));
    }

    return ERR("Could not read file.");
}

static BuiltinResult _fs_file_is_a_tty(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInstance *instance = AS_INSTANCE(caller);

    Obj *fd_obj;
    table_get(&instance->fields, AS_OBJ(new_string("fd", 2)), &fd_obj);

    int64_t fd = AS_INT(fd_obj)->value;
    return OK(new_bool(isatty(fd)));
}

static BuiltinResult _fs_file_write(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(STRING, 0)

    ObjInstance *instance = AS_INSTANCE(caller);

    Obj *fd_obj;
    table_get(&instance->fields, AS_OBJ(new_string("fd", 2)), &fd_obj);

    int64_t fd = AS_INT(fd_obj)->value;
    ObjString *str = AS_STRING(argv[0]);

    if (write(fd, str->chars, str->raw_length) == str->raw_length)
        return OK(new_nil());

    return ERR("Could not write file.");
}

static BuiltinResult _fs_file_close(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInstance *instance = AS_INSTANCE(caller);

    Obj *fd_obj;
    table_get(&instance->fields, AS_OBJ(new_string("fd", 2)), &fd_obj);

    int64_t fd = AS_INT(fd_obj)->value;

    close(fd);

    return OK(new_nil());
}

static BuiltinResult _fs_file_seek(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(INT, 0)
    CHECK_ARG_TYPE(INT, 1)

    ObjInstance *instance = AS_INSTANCE(caller);

    Obj *fd_obj;
    table_get(&instance->fields, AS_OBJ(new_string("fd", 2)), &fd_obj);

    int64_t fd = AS_INT(fd_obj)->value;
    int64_t offset = AS_INT(argv[0])->value;
    int64_t whence = AS_INT(argv[1])->value;

    return OK(new_int(lseek(fd, offset, whence)));
}

static BuiltinResult _fs_file_tell(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInstance *instance = AS_INSTANCE(caller);

    Obj *fd_obj;
    table_get(&instance->fields, AS_OBJ(new_string("fd", 2)), &fd_obj);

    int64_t fd = AS_INT(fd_obj)->value;

    return OK(new_int(lseek(fd, 0, SEEK_CUR)));
}

static BuiltinResult _fs_file_dup(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInstance *instance = AS_INSTANCE(caller);

    Obj *fd_obj;
    table_get(&instance->fields, AS_OBJ(new_string("fd", 2)), &fd_obj);

    int64_t fd = AS_INT(fd_obj)->value;

    return OK(new_int(dup(fd)));
}

ObjModule *get_fs_module(int count, UNUSED(char **, parts)) {
    CHECK_PART_COUNT(0)

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

        if (_fs_file_class == NULL) {
            ObjClass *klass = new_class(new_string("File", 4));
            klass->is_builtin = true;

            SET_BUILTIN_FN_STATIC("open", _fs_file_open);
            SET_BUILTIN_FN_STATIC("create", _fs_file_create);

            SET_BUILTIN_FN_METHOD("init", _fs_file_init);
            SET_BUILTIN_FN_METHOD("read", _fs_file_read);
            SET_BUILTIN_FN_METHOD("write", _fs_file_write);
            SET_BUILTIN_FN_METHOD("close", _fs_file_close);
            SET_BUILTIN_FN_METHOD("tell", _fs_file_tell);
            SET_BUILTIN_FN_METHOD("seek", _fs_file_seek);
            SET_BUILTIN_FN_METHOD("is_a_tty", _fs_file_is_a_tty);
            SET_BUILTIN_FN_METHOD("dup", _fs_file_dup);

            _fs_file_class = klass;
        }

        ObjInstance *_stdin = new_instance(_fs_file_class);
        set_file_instance(STDIN_FILENO, _stdin);

        ObjInstance *_stdout = new_instance(_fs_file_class);
        set_file_instance(STDOUT_FILENO, _stdout);

        ObjInstance *_stderr = new_instance(_fs_file_class);
        set_file_instance(STDERR_FILENO, _stderr);

        SET_MEMBER("STDIN", _stdin);
        SET_MEMBER("STDOUT", _stdout);
        SET_MEMBER("STDERR", _stderr);

        SET_MEMBER("File", _fs_file_class);

        _fs_module = module;
    }

    return _fs_module;
}
