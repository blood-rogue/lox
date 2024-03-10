#include "modules.h"

ObjModule *get_module(int part_count, char **parts) {
    char *name = parts[0];

    if (strcmp(name, "math") == 0)
        return get_math_module(part_count - 1, parts + 1);
    else if (strcmp(name, "fs") == 0)
        return get_fs_module(part_count - 1, parts + 1);
    else if (strcmp(name, "time") == 0)
        return get_time_module(part_count - 1, parts + 1);
    else if (strcmp(name, "random") == 0)
        return get_random_module(part_count - 1, parts + 1);
    else if (strcmp(name, "sys") == 0)
        return get_sys_module(part_count - 1, parts + 1);
    else if (strcmp(name, "regex") == 0)
        return get_regex_module(part_count - 1, parts + 1);

    return NULL;
}
