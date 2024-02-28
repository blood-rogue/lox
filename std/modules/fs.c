#include "builtins.h"

ObjModule *get_fs_module() {
    ObjModule *module = new_module(new_string("fs", 2));

    return module;
}
