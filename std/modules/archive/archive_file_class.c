#include <archive.h>
#include <archive_entry.h>

#include "native.h"

static ObjClass *_archive_file_class = NULL;

void set_entry_instance(ObjInstance *instance, struct archive_entry *entry) {
    SET_INT_FIELD("atime", archive_entry_atime(entry));
    SET_INT_FIELD("atime_nsec", archive_entry_atime_nsec(entry));
    SET_INT_FIELD("birthtime", archive_entry_birthtime(entry));
    SET_INT_FIELD("birthtime_nsec", archive_entry_birthtime_nsec(entry));
    SET_INT_FIELD("ctime", archive_entry_ctime(entry));
    SET_INT_FIELD("ctime_nsec", archive_entry_ctime_nsec(entry));
    SET_INT_FIELD("dev", archive_entry_dev(entry));
    SET_INT_FIELD("devmajor", archive_entry_devmajor(entry));
    SET_INT_FIELD("devminor", archive_entry_devminor(entry));
    SET_INT_FIELD("filetype", archive_entry_filetype(entry));
    SET_INT_FIELD("gid", archive_entry_gid(entry));
    SET_INT_FIELD("ino", archive_entry_ino(entry));
    SET_INT_FIELD("ino64", archive_entry_ino64(entry));
    SET_INT_FIELD("mode", archive_entry_mode(entry));
    SET_INT_FIELD("mtime", archive_entry_mtime(entry));
    SET_INT_FIELD("mtime_nsec", archive_entry_mtime_nsec(entry));
    SET_INT_FIELD("nlink", archive_entry_nlink(entry));
    SET_INT_FIELD("perm", archive_entry_perm(entry));
    SET_INT_FIELD("rdev", archive_entry_rdev(entry));
    SET_INT_FIELD("rdevmajor", archive_entry_rdevmajor(entry));
    SET_INT_FIELD("rdevminor", archive_entry_rdevminor(entry));
    SET_INT_FIELD("size", archive_entry_size(entry));
    SET_INT_FIELD("symlink_type", archive_entry_symlink_type(entry));
    SET_INT_FIELD("uid", archive_entry_uid(entry));

    SET_BOOL_FIELD("birthtime_is_set", archive_entry_birthtime_is_set(entry));
    SET_BOOL_FIELD("ctime_is_set", archive_entry_ctime_is_set(entry));
    SET_BOOL_FIELD("dev_is_set", archive_entry_dev_is_set(entry));
    SET_BOOL_FIELD("ino_is_set", archive_entry_ino_is_set(entry));
    SET_BOOL_FIELD("mtime_is_set", archive_entry_mtime_is_set(entry));
    SET_BOOL_FIELD("size_is_set", archive_entry_size_is_set(entry));
    SET_BOOL_FIELD("atime_is_set", archive_entry_atime_is_set(entry));
    SET_BOOL_FIELD("is_data_encrypted", archive_entry_is_data_encrypted(entry));
    SET_BOOL_FIELD("is_metadata_encrypted", archive_entry_is_metadata_encrypted(entry));
    SET_BOOL_FIELD("is_encrypted", archive_entry_is_encrypted(entry));

    const char *s;

    SET_FIELD(
        "fflags_text",
        ((s = archive_entry_fflags_text(entry)) == NULL) ? AS_OBJ(new_nil())
                                                         : AS_OBJ(new_string(s, strlen(s))));
    SET_FIELD(
        "gname",
        ((s = archive_entry_gname(entry)) == NULL) ? AS_OBJ(new_nil())
                                                   : AS_OBJ(new_string(s, strlen(s))));
    SET_FIELD(
        "hardlink",
        ((s = archive_entry_hardlink(entry)) == NULL) ? AS_OBJ(new_nil())
                                                      : AS_OBJ(new_string(s, strlen(s))));
    SET_FIELD(
        "pathname",
        ((s = archive_entry_pathname(entry)) == NULL) ? AS_OBJ(new_nil())
                                                      : AS_OBJ(new_string(s, strlen(s))));
    SET_FIELD(
        "sourcepath",
        ((s = archive_entry_sourcepath(entry)) == NULL) ? AS_OBJ(new_nil())
                                                        : AS_OBJ(new_string(s, strlen(s))));
    SET_FIELD(
        "strmode",
        ((s = archive_entry_strmode(entry)) == NULL) ? AS_OBJ(new_nil())
                                                     : AS_OBJ(new_string(s, strlen(s))));
    SET_FIELD(
        "symlink",
        ((s = archive_entry_symlink(entry)) == NULL) ? AS_OBJ(new_nil())
                                                     : AS_OBJ(new_string(s, strlen(s))));
    SET_FIELD(
        "uname",
        ((s = archive_entry_uname(entry)) == NULL) ? AS_OBJ(new_nil())
                                                   : AS_OBJ(new_string(s, strlen(s))));
}

static NativeResult _archive_file_list_entries(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInstance *a_instance = AS_INSTANCE(caller);
    GET_INTERNAL(struct archive *, a);

    ObjList *entries = new_list(NULL, 0);

    struct archive_entry *entry;
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        ObjInstance *instance = new_instance(get_archive_entry_class());
        set_entry_instance(instance, entry);

        write_array(&entries->elems, AS_OBJ(instance));

        archive_read_data_skip(a);
    }

    OK(entries);
}

static NativeResult _archive_file_extract(int argc, Obj **argv, Obj *caller) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjInt, INT, 0)

    ObjInstance *a_instance = AS_INSTANCE(caller);
    GET_INTERNAL(struct archive *, a);

    struct archive_entry *entry;

    struct archive *ext = archive_write_disk_new();
    archive_write_disk_set_options(ext, mpz_get_si(argv_0->value));
    archive_write_disk_set_standard_lookup(ext);

    int r;
    while ((r = archive_read_next_header(a, &entry)) == ARCHIVE_OK) {
        if (r == ARCHIVE_EOF)
            break;
        if (r < ARCHIVE_OK)
            ERR("Archive error: %s", archive_error_string(a))
        if (r < ARCHIVE_WARN)
            ERR("Could not read header.")

        r = archive_write_header(ext, entry);
        if (r < ARCHIVE_OK)
            ERR("Archive error: %s", archive_error_string(ext))
        else if (archive_entry_size(entry) > 0) {
            const void *buff;
            size_t size;
            la_int64_t offset;

            while ((r = archive_read_data_block(a, &buff, &size, &offset)) == ARCHIVE_OK) {
                if (r == ARCHIVE_EOF)
                    break;
                r = archive_write_data_block(ext, buff, size, offset);

                if (r < ARCHIVE_OK) {
                    ERR("Archive error: %s", archive_error_string(ext))
                }
            }

            if (r < ARCHIVE_OK)
                ERR("Archive error: %s", archive_error_string(ext))
            if (r < ARCHIVE_WARN)
                ERR("Could not write header.")
        }

        r = archive_write_finish_entry(ext);
        if (r < ARCHIVE_OK)
            ERR("Archive error: %s", archive_error_string(ext))
        if (r < ARCHIVE_WARN)
            ERR("Could not finish writing entry.")
    }

    archive_write_close(ext);
    archive_write_free(ext);

    OK(new_nil());
}

static NativeResult _archive_file_close(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInstance *a_instance = AS_INSTANCE(caller);
    GET_INTERNAL(struct archive *, a);

    archive_read_close(a);
    archive_read_free(a);

    native->ptr = NULL;

    OK(new_nil());
}

static NativeResult _archive_file_next_entry(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjInstance *a_instance = AS_INSTANCE(caller);
    GET_INTERNAL(struct archive *, a);

    struct archive_entry *entry;
    if (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        ObjInstance *instance = new_instance(get_archive_entry_class());

        set_entry_instance(instance, entry);

        archive_read_data_skip(a);

        OK(instance);
    }

    ERR("Archive error: %s", archive_error_string(a))
}

ObjClass *get_archive_file_class() {
    if (_archive_file_class == NULL) {
        ObjClass *klass = new_native_class("ArchiveFile");

        SET_NATIVE_FN_METHOD("list_entries", _archive_file_list_entries);
        SET_NATIVE_FN_METHOD("extract", _archive_file_extract);
        SET_NATIVE_FN_METHOD("close", _archive_file_close);
        SET_NATIVE_FN_METHOD("next_entry", _archive_file_next_entry);

        _archive_file_class = klass;
    }

    return _archive_file_class;
}
