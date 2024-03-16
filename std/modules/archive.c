#include <archive.h>

#include "builtins.h"

static ObjModule *_archive_module = NULL;

ObjModule *get_archive_module(int count, char **parts) {
    if (_archive_module == NULL) {
        ObjModule *module = new_module(new_string("archive", 7));

        _archive_module = module;
    }

    return _archive_module;
}
