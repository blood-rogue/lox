#include "object.h"

#define SET_MODULE_TABLE(module_name, name)                                                        \
    table_set(                                                                                     \
        &module->globals,                                                                          \
        AS_OBJ(new_string(#name, (int)strlen(#name))),                                             \
        AS_OBJ(new_builtin_function(_##module_name##_##name, #name)))

#define SET_VAR(name, value)                                                                       \
    table_set(&module->globals, AS_OBJ(new_string(#name, (int)strlen(#name))), AS_OBJ(value))

#define SET_INT_VAR(name, value) SET_VAR(name, new_int(value))

ObjModule *get_module(char *);

ObjModule *get_math_module();
ObjModule *get_fs_module();
ObjModule *get_time_module();
ObjModule *get_random_module();
