#include <unistr.h>

#include "native.h"

static NativeResult _bytes_new(int argc, UNUSED(Obj **argv), UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(0)
    OK(new_bytes(NULL, 0));
}

static NativeResult _bytes_len(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    OK(new_int_i(AS_BYTES(caller)->length));
}

static NativeResult _bytes_decode(int argc, UNUSED(Obj **argv), Obj *caller) {
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

static NativeResult _bytes_hex_lower(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    ObjBytes *bytes = AS_BYTES(caller);

    OK(take_string(bytes_to_hex(bytes->bytes, bytes->length, "%02x"), bytes->length * 2 + 1));
}

static NativeResult _bytes_hex_upper(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    ObjBytes *bytes = AS_BYTES(caller);

    OK(take_string(bytes_to_hex(bytes->bytes, bytes->length, "%02X"), bytes->length * 2 + 1));
}

static ObjClass *_bytes_class = NULL;

ObjClass *get_bytes_class() {
    if (_bytes_class == NULL) {
        ObjClass *klass = new_native_class("Bytes");

        SET_NATIVE_FN_STATIC("__new", _bytes_new);

        SET_NATIVE_FN_METHOD("len", _bytes_len);
        SET_NATIVE_FN_METHOD("hex_lower", _bytes_hex_lower);
        SET_NATIVE_FN_METHOD("hex_upper", _bytes_hex_upper);
        SET_NATIVE_FN_METHOD("decode", _bytes_decode);

        _bytes_class = klass;
    }

    return _bytes_class;
}
