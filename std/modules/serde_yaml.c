#include <yaml.h>

#include "builtins.h"

ObjModule *_serde_yaml_module = NULL;

static BuiltinResult _serde_yaml_deserialize(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(STRING, 0)

    yaml_parser_t parser;
    yaml_event_t event;

    if (!yaml_parser_initialize(&parser))
        return ERR("Failed to initialize parser.");

    yaml_parser_set_input_string(
        &parser, (uint8_t *)AS_STRING(argv[0])->chars, AS_STRING(argv[0])->raw_length);

    do {
        if (!yaml_parser_parse(&parser, &event)) {
            return ERR("Invalid document.");
        }

        switch (event.type) {
            case YAML_NO_EVENT:
                break;
            case YAML_STREAM_START_EVENT:
                break;
            case YAML_STREAM_END_EVENT:
                break;

            case YAML_DOCUMENT_START_EVENT:
                break;
            case YAML_DOCUMENT_END_EVENT:
                break;

            case YAML_SEQUENCE_START_EVENT:
                break;
            case YAML_SEQUENCE_END_EVENT:
                break;

            case YAML_MAPPING_START_EVENT:
                break;
            case YAML_MAPPING_END_EVENT:
                break;

            case YAML_ALIAS_EVENT:
                break;
            case YAML_SCALAR_EVENT:
                break;
        }
        if (event.type != YAML_STREAM_END_EVENT)
            yaml_event_delete(&event);
    } while (event.type != YAML_STREAM_END_EVENT);

    yaml_event_delete(&event);
    yaml_parser_delete(&parser);

    return OK(new_nil());
}

ObjModule *get_serde_yaml_module() {
    if (_serde_yaml_module == NULL) {
        ObjModule *module = new_module(new_string("yaml", 4));

        SET_BUILTIN_FN_MEMBER("deserialize", _serde_yaml_deserialize);

        _serde_yaml_module = module;
    }

    return _serde_yaml_module;
}
