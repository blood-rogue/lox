#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "builtins.h"

static ObjClass *_fs_file_class = NULL;

static void free_file(void *file) { fclose(file); }

static void set_file_instance(FILE *file, ObjInstance *instance) {
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
}

static NativeResult _fs_file_open(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjString, STRING, 0)
    CHECK_ARG_TYPE(ObjString, STRING, 1)

    FILE *file = fopen(argv_0->chars, argv_1->chars);
    ObjInstance *instance = new_instance(_fs_file_class);

    set_file_instance(file, instance);

    OK(instance);
}

static NativeResult _fs_file_create(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    FILE *file = fopen(argv_0->chars, "w");
    ObjInstance *instance = new_instance(_fs_file_class);

    set_file_instance(file, instance);

    OK(instance);
}

static NativeResult _fs_file_read(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInstance *file_instance = AS_INSTANCE(caller);
    GET_INTERNAL(FILE *, file);

    size_t fsize = fseek(file, 0, SEEK_END);

    char *buf = malloc(fsize);

    fseek(file, 0, SEEK_SET);

    if (fread(buf, 1, fsize, file) == fsize) {
        OK(take_string(buf, fsize));
    }

    ERR("Could not read file.")
}

static NativeResult _fs_file_is_a_tty(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInstance *instance = AS_INSTANCE(caller);

    Obj *fd_obj;
    table_get(&instance->fields, AS_OBJ(new_string("fd", 2)), &fd_obj);

    int64_t fd = mpz_get_si(AS_INT(fd_obj)->value);
    OK(new_bool(isatty(fd)));
}

static NativeResult _fs_file_write(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    ObjInstance *file_instance = AS_INSTANCE(caller);
    GET_INTERNAL(FILE *, file);

    if (fwrite(argv_0->chars, 1, argv_0->raw_length, file) == (size_t)argv_0->raw_length)
        OK(new_nil());

    ERR("Could not write file.")
}

static NativeResult _fs_file_close(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInstance *file_instance = AS_INSTANCE(caller);
    GET_INTERNAL(FILE *, file);

    fclose(file);

    native->ptr = NULL;

    OK(new_nil());
}

static NativeResult _fs_file_seek(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjInt, INT, 0)
    CHECK_ARG_TYPE(ObjInt, INT, 1)

    ObjInstance *file_instance = AS_INSTANCE(caller);
    GET_INTERNAL(FILE *, file);

    int64_t offset = mpz_get_si(argv_0->value);
    int64_t whence = mpz_get_si(argv_1->value);

    OK(new_int_i(fseek(file, offset, whence)));
}

static NativeResult _fs_file_tell(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInstance *file_instance = AS_INSTANCE(caller);
    GET_INTERNAL(FILE *, file);

    OK(new_int_i(ftell(file)));
}

static NativeResult _fs_file_dup(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInstance *instance = AS_INSTANCE(caller);

    Obj *fd_obj;
    table_get(&instance->fields, AS_OBJ(new_string("fd", 2)), &fd_obj);

    int64_t fd = mpz_get_si(AS_INT(fd_obj)->value);

    OK(new_int_i(dup(fd)));
}

ObjClass *get_fs_file_class() {
    if (_fs_file_class == NULL) {
        ObjClass *klass = new_builtin_class("File");

        SET_BUILTIN_FN_STATIC("open", _fs_file_open);
        SET_BUILTIN_FN_STATIC("create", _fs_file_create);

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
