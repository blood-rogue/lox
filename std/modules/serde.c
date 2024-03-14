#include "builtins.h"

static ObjModule *_serde_module = NULL;

ObjModule *get_serde_module(int count, char **parts) {

    if (_serde_module == NULL) {
        ObjModule *module = new_module(new_string("serde", 5));

        SET_MEMBER("json", get_serde_json_module());
        SET_MEMBER("yaml", get_serde_yaml_module());
        SET_MEMBER("toml", get_serde_toml_module());

        _serde_module = module;
    }

    if (count == 0)
        return _serde_module;
    else if (count == 1) {
        if (strcmp(parts[0], "json") == 0)
            return get_serde_json_module();
        else if (strcmp(parts[0], "yaml") == 0)
            return get_serde_yaml_module();
        else if (strcmp(parts[0], "toml") == 0)
            return get_serde_toml_module();

        return NULL;
    }

    return NULL;
}
