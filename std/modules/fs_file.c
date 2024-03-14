#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "builtins.h"

static ObjClass *_fs_file_class = NULL;

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
    CHECK_ARG_TYPE(ObjInt, INT, 0)

    int fd = argv_0->value;
    ObjInstance *instance = AS_INSTANCE(caller);

    set_file_instance(fd, instance);

    return OK(new_nil());
}

static BuiltinResult _fs_file_open(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjString, STRING, 0)
    CHECK_ARG_TYPE(ObjInt, INT, 1)

    int fd = open(argv_0->chars, argv_1->value);
    ObjInstance *instance = new_instance(_fs_file_class);

    set_file_instance(fd, instance);

    return OK(instance);
}

static BuiltinResult _fs_file_create(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    int fd = open(argv_0->chars, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP);
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
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    ObjInstance *instance = AS_INSTANCE(caller);

    Obj *fd_obj;
    table_get(&instance->fields, AS_OBJ(new_string("fd", 2)), &fd_obj);

    int64_t fd = AS_INT(fd_obj)->value;

    if (write(fd, argv_0->chars, argv_0->raw_length) == argv_0->raw_length)
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
    CHECK_ARG_TYPE(ObjInt, INT, 0)
    CHECK_ARG_TYPE(ObjInt, INT, 1)

    ObjInstance *instance = AS_INSTANCE(caller);

    Obj *fd_obj;
    table_get(&instance->fields, AS_OBJ(new_string("fd", 2)), &fd_obj);

    int64_t fd = AS_INT(fd_obj)->value;
    int64_t offset = argv_0->value;
    int64_t whence = argv_1->value;

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

ObjClass *get_fs_file_class() {

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

    return _fs_file_class;
}
