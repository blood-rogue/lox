#include "object.h"

#define SET_MEMBER(name, value)                                                                    \
    table_set(&module->globals, AS_OBJ(new_string(name, (int)strlen(name))), AS_OBJ(value))

#define SET_INT_MEMBER(name, value)        SET_MEMBER(name, new_int(value))
#define SET_FLOAT_MEMBER(name, value)      SET_MEMBER(name, new_float(value))
#define SET_BUILTIN_FN_MEMBER(name, value) SET_MEMBER(name, new_builtin_function(value, name))

#define SET_FIELD(name, value)                                                                     \
    table_set(&instance->fields, AS_OBJ(new_string(name, (int)strlen(name))), AS_OBJ(value))

#define SET_INT_FIELD(name, value) SET_FIELD(name, new_int(value))

#define SET_STATIC(name, value)                                                                    \
    table_set(&klass->statics, AS_OBJ(new_string(name, (int)strlen(name))), AS_OBJ(value))

#define SET_BUILTIN_FN_STATIC(name, value) SET_STATIC(name, new_builtin_function(value, name))

#define SET_METHOD(name, value)                                                                    \
    table_set(&klass->methods, AS_OBJ(new_string(name, (int)strlen(name))), AS_OBJ(value))

#define SET_BUILTIN_FN_METHOD(name, value) SET_STATIC(name, new_builtin_function(value, name))

ObjModule *get_module(char *);

ObjModule *get_math_module();
ObjModule *get_fs_module();
ObjModule *get_time_module();
ObjModule *get_random_module();
ObjModule *get_process_module();
