#include <unistd.h>

#include "builtins.h"

static ObjModule *_process_module = NULL;

ObjModule *get_process_module() {
    if (_process_module == NULL) {
        ObjModule *module = new_module(new_string("process", 7));

        _process_module = module;
    }

    return _process_module;
}
