#define PCRE2_CODE_UNIT_WIDTH 8

#include <pcre2.h>

#include "builtins.h"

static ObjModule *_regex_module = NULL;

ObjModule *get_regex_module() {
    if (_regex_module == NULL) {
        ObjModule *module = new_module(new_string("regex", 5));

        _regex_module = module;
    }

    return _regex_module;
}
