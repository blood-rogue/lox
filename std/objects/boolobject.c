#include "builtins.h"

BuiltinTable *bool_methods() {
    BuiltinTable *table = malloc(sizeof(BuiltinTable));
    init_method_table(table, 8);

    return table;
}
