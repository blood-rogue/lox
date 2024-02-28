#include "object.h"

#define SET_BLTIN_METHOD(obj, name)                                                                \
    method_table_set(table, #name, hash_string(#name, (int)strlen(#name)), _##obj##_##name)

BuiltinTable **get_builtin_methods();

BuiltinTable *float_methods();
BuiltinTable *int_methods();
BuiltinTable *bool_methods();
BuiltinTable *char_methods();
BuiltinTable *string_methods();
BuiltinTable *list_methods();
BuiltinTable *map_methods();
