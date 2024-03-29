#ifndef lox_modules_h
#define lox_modules_h

#include "crypto.h"
#include "native.h"

#define SET_MEMBER(name, value)                                                                    \
    table_set(&module->globals, AS_OBJ(new_string(name, (int)strlen(name))), AS_OBJ(value))

#define SET_INT_MEMBER(name, value)       SET_MEMBER(name, new_int_i(value))
#define SET_FLOAT_MEMBER(name, value)     SET_MEMBER(name, new_float(value))
#define SET_NATIVE_FN_MEMBER(name, value) SET_MEMBER(name, new_native_function(value, name))

#define SET_FIELD(name, value)                                                                     \
    table_set(&instance->fields, AS_OBJ(new_string(name, (int)strlen(name))), AS_OBJ(value))

#define SET_INT_FIELD(name, value)    SET_FIELD(name, new_int_i(value))
#define SET_BOOL_FIELD(name, value)   SET_FIELD(name, new_bool(value))
#define SET_FLOAT_FIELD(name, value)  SET_FIELD(name, new_float(value))
#define SET_STRING_FIELD(name, value) SET_FIELD(name, new_string(value, (int)strlen(value)))

#define SET_STATIC(name, value)                                                                    \
    table_set(&klass->statics, AS_OBJ(new_string(name, (int)strlen(name))), AS_OBJ(value))

#define SET_NATIVE_FN_STATIC(name, value) SET_STATIC(name, new_native_function(value, name))

#define SET_METHOD(name, value)                                                                    \
    table_set(&klass->methods, AS_OBJ(new_string(name, (int)strlen(name))), AS_OBJ(value))

#define SET_NATIVE_FN_METHOD(name, value) SET_METHOD(name, new_native_function(value, name))

#define CHECK_PART_COUNT                                                                           \
    if (count != 0)                                                                                \
        return NULL;

ObjModule *get_module(int, char **);

ObjModule *get_random_module(int, char **);
ObjModule *get_math_module(int, char **);

ObjModule *get_regex_module(int, char **);
ObjClass *get_regex_match_class();
ObjClass *get_regex_pattern_class();

ObjModule *get_sys_module(int, char **);
ObjClass *get_sys_user_class();
ObjClass *get_sys_group_class();

ObjModule *get_fs_module(int, char **);
ObjClass *get_fs_file_class();

ObjModule *get_compress_module(int, char **);
ObjModule *get_compress_zlib_module();
ObjModule *get_compress_zstd_module();
ObjModule *get_compress_lz4_module();
ObjModule *get_compress_snappy_module();

ObjModule *get_serde_module(int, char **);
ObjModule *get_serde_json_module();
ObjModule *get_serde_yaml_module();
ObjModule *get_serde_toml_module();

ObjModule *get_archive_module(int, char **);
ObjClass *get_archive_file_class();
ObjClass *get_archive_entry_class();

ObjModule *get_db_module();
ObjClass *get_db_kv_class();
ObjClass *get_db_sqlite3_class();

#endif // lox_modules_h
