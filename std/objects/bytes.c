#include <unistr.h>

#include "builtins.h"

static BuiltinResult _bytes_len(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    OK(new_int(AS_BYTES(caller)->length));
}

static BuiltinResult _bytes_decode(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    ObjBytes *bytes = AS_BYTES(caller);

    OK(new_string((char *)bytes->bytes, bytes->length));
}

static char *bytes_to_hex(uint8_t *bytes, int bytes_len, const char *format) {
    char *hex_str = malloc(bytes_len * 2 + 1);

    for (int i = 0, j = 0; i < bytes_len; i++, j += 2) {
        sprintf(hex_str + j, format, bytes[i]);
    }

    hex_str[bytes_len * 2] = '\0';

    return hex_str;
}

static BuiltinResult _bytes_hex_lower(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    ObjBytes *bytes = AS_BYTES(caller);

    OK(take_string(bytes_to_hex(bytes->bytes, bytes->length, "%02x"), bytes->length * 2 + 1));
}

static BuiltinResult _bytes_hex_upper(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    ObjBytes *bytes = AS_BYTES(caller);

    OK(take_string(bytes_to_hex(bytes->bytes, bytes->length, "%02X"), bytes->length * 2 + 1));
}

BuiltinTable *bytes_methods() {
    BuiltinTable *table = malloc(sizeof(BuiltinTable));
    init_method_table(table, 8);

    SET_BLTIN_METHOD("len", _bytes_len);
    SET_BLTIN_METHOD("hex_lower", _bytes_hex_lower);
    SET_BLTIN_METHOD("hex_upper", _bytes_hex_upper);
    SET_BLTIN_METHOD("decode", _bytes_decode);

    return table;
}
