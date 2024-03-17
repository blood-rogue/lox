#include "builtins.h"

static ObjClass *_archive_tar_entry_class = NULL;

ObjClass *get_archive_tar_entry_class() {
    if (_archive_tar_entry_class == NULL) {
        ObjClass *klass = new_class(new_string("TarEntry", 8));
        klass->is_builtin = true;

        _archive_tar_entry_class = klass;
    }

    return _archive_tar_entry_class;
}
