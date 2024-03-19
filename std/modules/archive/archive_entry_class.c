#include "builtins.h"

static ObjClass *_archive_entry_class = NULL;

ObjClass *get_archive_entry_class() {
    if (_archive_entry_class == NULL) {
        ObjClass *klass = new_builtin_class("ArchiveEntry");

        _archive_entry_class = klass;
    }

    return _archive_entry_class;
}
