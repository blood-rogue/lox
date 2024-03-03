#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "builtins.h"

static ObjModule *_fs_module = NULL;
static ObjClass *_fs_file_class = NULL;

static BuiltinResult _fs_dup(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(INT, 0)

    return OK(new_int(dup(AS_INT(argv[0])->value)));
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

static BuiltinResult _fs_file_init(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(INT, 0)

    int fd = AS_INT(argv[0])->value;
    struct stat st;
    fstat(fd, &st);

    ObjInstance *instance = AS_INSTANCE(caller);

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

    return OK(new_nil());
}

static BuiltinResult _fs_file_open(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(STRING, 0)
    CHECK_ARG_TYPE(INT, 1)

    int fd = open(AS_STRING(argv[0])->chars, AS_INT(argv[1])->value);
    struct stat st;
    fstat(fd, &st);

    ObjInstance *instance = new_instance(_fs_file_class);

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

    return OK(instance);
}

static BuiltinResult _fs_file_create(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(STRING, 0)

    int fd =
        open(AS_STRING(argv[0])->chars, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP);
    struct stat st;
    fstat(fd, &st);

    ObjInstance *instance = new_instance(_fs_file_class);

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

static BuiltinResult _fs_file_write(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(STRING, 0)

    ObjInstance *instance = AS_INSTANCE(caller);

    Obj *fd_obj;
    table_get(&instance->fields, AS_OBJ(new_string("fd", 2)), &fd_obj);

    int64_t fd = AS_INT(fd_obj)->value;
    ObjString *str = AS_STRING(argv[0]);

    if (write(fd, str->chars, str->length) == str->length)
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

ObjModule *get_fs_module() {
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

        SET_BUILTIN_FN_MEMBER("dup", _fs_dup);
        SET_BUILTIN_FN_MEMBER("chown", _fs_chown);

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

            _fs_file_class = klass;
        }

        SET_MEMBER("File", _fs_file_class);

        _fs_module = module;
    }

    return _fs_module;
}
