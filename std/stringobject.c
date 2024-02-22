#include "builtins.h"

BuiltinTable *string_methods() {
    BuiltinTable *table = malloc(sizeof(BuiltinTable));
    init_builtin_table(table);

    return table;
}