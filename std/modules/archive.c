#include "builtins.h"

static ObjModule *_archive_module = NULL;

ObjModule *get_archive_module(int count, char **parts) {
    if (_archive_module == NULL) {
        ObjModule *module = new_module(new_string("archive", 7));

        _archive_module = module;
    }

    if (count == 0)
        return _archive_module;
    else if (count == 1) {
        if (strcmp(parts[0], "tar") == 0)
            return get_archive_tar_module();

        return NULL;
    }

    return NULL;
}
