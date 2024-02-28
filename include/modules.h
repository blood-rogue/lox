#include "object.h"

#define SET_MODULE_TABLE(module_name, name)                                                        \
    table_set(                                                                                     \
        &module->globals,                                                                          \
        AS_OBJ(new_string(#name, (int)strlen(#name))),                                             \
        AS_OBJ(new_builtin_function(_##module_name##_##name, #name)))

ObjModule *get_module(char *);

ObjModule *get_math_module();
ObjModule *get_fs_module();
ObjModule *get_time_module();
ObjModule *get_random_module();
