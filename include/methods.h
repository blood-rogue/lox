#include "object.h"

#define SET_BLTIN_METHOD(name, fn)                                                                 \
    method_table_set(table, name, hash_string(name, (int)strlen(name)), fn)

BuiltinTable **get_builtin_methods();

BuiltinTable *float_methods();
BuiltinTable *int_methods();
BuiltinTable *bool_methods();
BuiltinTable *char_methods();
BuiltinTable *string_methods();
BuiltinTable *list_methods();
BuiltinTable *map_methods();
