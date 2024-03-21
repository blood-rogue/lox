#ifndef lox_methods_h
#define lox_methods_h

#include "object.h"

#define SET_BLTIN_METHOD(name, fn)                                                                 \
    method_table_set(table, name, hash_string(name, (int)strlen(name)), fn)

NativeTable **get_builtin_methods();

NativeTable *float_methods();
NativeTable *int_methods();
NativeTable *char_methods();
NativeTable *string_methods();
NativeTable *list_methods();
NativeTable *map_methods();
NativeTable *bytes_methods();

#endif // lox_methods_h
