#include "modules.h"

ObjModule *get_module(char *name) {
    if (strcmp(name, "math") == 0)
        return get_math_module();
    else if (strcmp(name, "fs") == 0)
        return get_fs_module();
    else if (strcmp(name, "time") == 0)
        return get_time_module();
    else if (strcmp(name, "random") == 0)
        return get_random_module();

    return NULL;
}
