#include "builtins.h"

static ObjClass *_archive_tar_info = NULL;

ObjClass *get_archive_tar_info() {
    if (_archive_tar_info == NULL) {
        ObjClass *klass = new_class(new_string("TarInfo", 7));

        _archive_tar_info = klass;
    }

    return _archive_tar_info;
}
