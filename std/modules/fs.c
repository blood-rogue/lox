#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "builtins.h"

static ObjClass *__fs_file_class = NULL;

static BuiltinResult __fs_file_open(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(STRING, 0)
    CHECK_ARG_TYPE(INT, 1)

    int fd = open(AS_STRING(argv[0])->chars, AS_INT(argv[1])->value);

    ObjInstance *_instance = new_instance(__fs_file_class);

    table_set(&_instance->fields, AS_OBJ(new_string("fd", 2)), AS_OBJ(new_int(fd)));

    return OK(_instance);
}

static BuiltinResult __fs_file_read(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInstance *instance = AS_INSTANCE(caller);

    Obj *fd_obj;
    if (!table_get(&instance->fields, AS_OBJ(new_string("fd", 2)), &fd_obj))
        return ERR("Invalid file instance.");

    int64_t fd = AS_INT(fd_obj)->value;
    off_t fsize = lseek(fd, 0, SEEK_END);

    char *buf = malloc(fsize);

    lseek(fd, 0, SEEK_SET);

    if (read(fd, buf, fsize) == fsize) {
        return OK(take_string(buf, fsize));
    }

    return ERR("Could not read file.");
}

static BuiltinResult __fs_file_write(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(STRING, 0)

    ObjInstance *instance = AS_INSTANCE(caller);

    Obj *fd_obj;
    if (!table_get(&instance->fields, AS_OBJ(new_string("fd", 2)), &fd_obj))
        return ERR("Invalid file instance.");

    int64_t fd = AS_INT(fd_obj)->value;
    ObjString *str = AS_STRING(argv[0]);

    if (write(fd, str->chars, str->length) == str->length)
        return OK(new_nil());

    return ERR("Could not write file.");
}

static BuiltinResult __fs_file_close(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInstance *instance = AS_INSTANCE(caller);

    Obj *fd_obj;
    if (!table_get(&instance->fields, AS_OBJ(new_string("fd", 2)), &fd_obj))
        return ERR("Invalid file instance.");

    int64_t fd = AS_INT(fd_obj)->value;

    close(fd);

    return OK(new_nil());
}

static BuiltinResult __fs_file_create(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(STRING, 0)

    int fd =
        open(AS_STRING(argv[0])->chars, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP);

    ObjInstance *_instance = new_instance(__fs_file_class);

    table_set(&_instance->fields, AS_OBJ(new_string("fd", 2)), AS_OBJ(new_int(fd)));

    return OK(_instance);
}

static BuiltinResult __fs_file_seek(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(INT, 0)
    CHECK_ARG_TYPE(INT, 1)

    ObjInstance *instance = AS_INSTANCE(caller);

    Obj *fd_obj;
    if (!table_get(&instance->fields, AS_OBJ(new_string("fd", 2)), &fd_obj))
        return ERR("Invalid file instance.");

    int64_t fd = AS_INT(fd_obj)->value;
    int64_t offset = AS_INT(argv[0])->value;
    int64_t whence = AS_INT(argv[1])->value;

    return OK(new_int(lseek(fd, offset, whence)));
}

static BuiltinResult __fs_file_tell(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInstance *instance = AS_INSTANCE(caller);

    Obj *fd_obj;
    if (!table_get(&instance->fields, AS_OBJ(new_string("fd", 2)), &fd_obj))
        return ERR("Invalid file instance.");

    int64_t fd = AS_INT(fd_obj)->value;

    return OK(new_int(lseek(fd, 0, SEEK_CUR)));
}

static ObjModule *__fs_module = NULL;

ObjModule *get_fs_module() {
    if (__fs_module == NULL) {
        ObjModule *module = new_module(new_string("fs", 2));

        SET_INT_VAR(READONLY, O_RDONLY);
        SET_INT_VAR(WRITEONLY, O_WRONLY);
        SET_INT_VAR(CREATE, O_CREAT);
        SET_INT_VAR(APPEND, O_APPEND);
        SET_INT_VAR(READWRITE, O_RDWR);

        SET_INT_VAR(SEEKEND, SEEK_END);
        SET_INT_VAR(SEEKSET, SEEK_SET);
        SET_INT_VAR(SEEKCUR, SEEK_CUR);

        if (__fs_file_class == NULL) {
            ObjClass *klass = new_class(new_string("File", 4));

            table_set(
                &klass->statics,
                AS_OBJ(new_string("open", 4)),
                AS_OBJ(new_builtin_function(__fs_file_open, "open")));
            table_set(
                &klass->statics,
                AS_OBJ(new_string("create", 6)),
                AS_OBJ(new_builtin_function(__fs_file_create, "create")));

            table_set(
                &klass->methods,
                AS_OBJ(new_string("read", 4)),
                AS_OBJ(new_builtin_function(__fs_file_read, "read")));
            table_set(
                &klass->methods,
                AS_OBJ(new_string("write", 5)),
                AS_OBJ(new_builtin_function(__fs_file_write, "write")));
            table_set(
                &klass->methods,
                AS_OBJ(new_string("close", 5)),
                AS_OBJ(new_builtin_function(__fs_file_close, "close")));
            table_set(
                &klass->methods,
                AS_OBJ(new_string("tell", 4)),
                AS_OBJ(new_builtin_function(__fs_file_tell, "tell")));
            table_set(
                &klass->methods,
                AS_OBJ(new_string("seek", 4)),
                AS_OBJ(new_builtin_function(__fs_file_seek, "seek")));

            __fs_file_class = klass;
        }

        table_set(&module->globals, AS_OBJ(new_string("File", 4)), AS_OBJ(__fs_file_class));

        __fs_module = module;
    }

    return __fs_module;
}
