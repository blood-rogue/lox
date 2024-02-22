#include "builtins.h"

BuiltinTable *list_methods() {
    BuiltinTable *table = malloc(sizeof(BuiltinTable));
    init_builtin_table(table);

    return table;
}