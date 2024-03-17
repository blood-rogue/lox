#include <archive.h>
#include <archive_entry.h>

#include "builtins.h"

static ObjModule *_archive_tar_module = NULL;

static BuiltinResult _archive_tar_open(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    struct archive *a = archive_read_new();

    archive_read_support_filter_gzip(a);
    archive_read_support_filter_bzip2(a);
    archive_read_support_filter_xz(a);

    archive_read_support_format_tar(a);
    archive_read_support_format_gnutar(a);

    ObjNativeStruct *archive_obj = new_native_struct(a);

    if (archive_read_open_filename(a, argv_0->chars, 10240) != ARCHIVE_OK)
        ERR("Could not open archive.")

    ObjInstance *instance = new_instance(get_archive_tar_file_class());

    SET_FIELD("$$internal", archive_obj);

    OK(instance);
}

ObjModule *get_archive_tar_module() {
    if (_archive_tar_module == NULL) {
        ObjModule *module = new_module(new_string("tar", 3));

        SET_BUILTIN_FN_MEMBER("open", _archive_tar_open);

        SET_INT_MEMBER("SYMLINK_TYPE_DIRECTORY", AE_SYMLINK_TYPE_DIRECTORY);
        SET_INT_MEMBER("SYMLINK_TYPE_FILE", AE_SYMLINK_TYPE_FILE);
        SET_INT_MEMBER("SYMLINK_TYPE_UNDEFINED", AE_SYMLINK_TYPE_UNDEFINED);

        _archive_tar_module = module;
    }

    return _archive_tar_module;
}
