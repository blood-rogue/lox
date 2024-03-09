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
    else if (strcmp(name, "process") == 0)
        return get_process_module();
    else if (strcmp(name, "sys") == 0)
        return get_sys_module();
    else if (strcmp(name, "regex") == 0)
        return get_regex_module();

    return NULL;
}
