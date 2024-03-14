#include <unistr.h>

#include "builtins.h"

static BuiltinResult _bytes_len(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(new_int(AS_BYTES(caller)->length));
}

static BuiltinResult _bytes_decode(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjBytes *bytes = AS_BYTES(caller);

    return OK(new_string((char *)bytes->bytes, bytes->length));
}

BuiltinTable *bytes_methods() {
    BuiltinTable *table = malloc(sizeof(BuiltinTable));
    init_method_table(table, 8);

    SET_BLTIN_METHOD("len", _bytes_len);
    SET_BLTIN_METHOD("decode", _bytes_decode);

    return table;
}
