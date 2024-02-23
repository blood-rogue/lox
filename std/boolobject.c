#include "builtins.h"

BuiltinMethodTable *bool_methods() {
    BuiltinMethodTable *table = malloc(sizeof(BuiltinMethodTable));
    init_method_table(table, 8);

    return table;
}