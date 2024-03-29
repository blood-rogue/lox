#include <yaml.h>

#include "native.h"

ObjModule *_serde_yaml_module = NULL;

static NativeResult _serde_yaml_deserialize(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    yaml_parser_t parser;
    yaml_event_t event;

    if (!yaml_parser_initialize(&parser))
        ERR("Failed to initialize parser.")

    yaml_parser_set_input_string(&parser, (uint8_t *)argv_0->chars, argv_0->raw_length);

    yaml_event_delete(&event);
    yaml_parser_delete(&parser);

    OK(new_nil());
}

ObjModule *get_serde_yaml_module() {
    if (_serde_yaml_module == NULL) {
        ObjModule *module = new_module("yaml");

        SET_NATIVE_FN_MEMBER("deserialize", _serde_yaml_deserialize);

        _serde_yaml_module = module;
    }

    return _serde_yaml_module;
}
