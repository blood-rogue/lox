#include "builtins.h"

ObjModule *get_module(char *name) {
    if (strcmp(name, "math") == 0)
        return get_math_module();
    else if (strcmp(name, "fs") == 0)
        return get_fs_module();

    return NULL;
}
