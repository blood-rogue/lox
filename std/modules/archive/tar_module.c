#include <archive.h>

#include "builtins.h"

static ObjModule *_archive_tar_module = NULL;

static BuiltinResult _compress_tar_open(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    struct archive *a = archive_read_new();

    archive_read_support_filter_gzip(a);
    archive_read_support_filter_bzip2(a);
    archive_read_support_filter_xz(a);

    archive_read_support_format_tar(a);
    archive_read_support_format_gnutar(a);

    ObjNativeStruct *archive_obj = new_native_struct(a, 0);

    if (archive_read_open_filename(a, argv_0->chars, 10240) != ARCHIVE_OK)
        return ERR("Could not open archive.");

    ObjInstance *instance = new_instance(get_archive_tar_file());

    SET_FIELD("_internal", archive_obj);

    return OK(instance);
}

ObjModule *get_archive_tar_module() {
    if (_archive_tar_module == NULL) {
        ObjModule *module = new_module(new_string("tar", 3));

        SET_BUILTIN_FN_MEMBER("open", _compress_tar_open);

        _archive_tar_module = module;
    }

    return _archive_tar_module;
}
