#include "builtins.h"

BuiltinMethodTable *char_methods() {
    BuiltinMethodTable *table = malloc(sizeof(BuiltinMethodTable));
    init_method_table(table, 8);

    return table;
}