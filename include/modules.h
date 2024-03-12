#include "object.h"

#define SET_MEMBER(name, value)                                                                    \
    table_set(&module->globals, AS_OBJ(new_string(name, (int)strlen(name))), AS_OBJ(value))

#define SET_INT_MEMBER(name, value)        SET_MEMBER(name, new_int(value))
#define SET_FLOAT_MEMBER(name, value)      SET_MEMBER(name, new_float(value))
#define SET_BUILTIN_FN_MEMBER(name, value) SET_MEMBER(name, new_builtin_function(value, name))

#define SET_FIELD(name, value)                                                                     \
    table_set(&instance->fields, AS_OBJ(new_string(name, (int)strlen(name))), AS_OBJ(value))

#define SET_INT_FIELD(name, value)    SET_FIELD(name, new_int(value))
#define SET_FLOAT_FIELD(name, value)  SET_FIELD(name, new_float(value))
#define SET_STRING_FIELD(name, value) SET_FIELD(name, new_string(value, (int)strlen(value)))

#define SET_STATIC(name, value)                                                                    \
    table_set(&klass->statics, AS_OBJ(new_string(name, (int)strlen(name))), AS_OBJ(value))

#define SET_BUILTIN_FN_STATIC(name, value) SET_STATIC(name, new_builtin_function(value, name))

#define SET_METHOD(name, value)                                                                    \
    table_set(&klass->methods, AS_OBJ(new_string(name, (int)strlen(name))), AS_OBJ(value))

#define SET_BUILTIN_FN_METHOD(name, value) SET_METHOD(name, new_builtin_function(value, name))

#define CHECK_PART_COUNT(value)                                                                    \
    if (count != value)                                                                            \
        return NULL;

ObjModule *get_module(int, char **);

ObjModule *get_math_module(int, char **);
ObjModule *get_fs_module(int, char **);
ObjModule *get_time_module(int, char **);
ObjModule *get_random_module(int, char **);
ObjModule *get_sys_module(int, char **);
ObjModule *get_regex_module(int, char **);

ObjModule *get_compress_module(int, char **);
ObjModule *get_compress_zlib_module();
ObjModule *get_compress_brotli_module();
ObjModule *get_compress_zstd_module();
ObjModule *get_compress_lzf_module();
ObjModule *get_compress_lz4_module();

ObjModule *get_serde_module(int, char **);
ObjModule *get_serde_json_module();
ObjModule *get_serde_yaml_module();
