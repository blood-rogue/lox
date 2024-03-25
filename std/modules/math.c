#include "native.h"

static ObjModule *_math_module = NULL;

ObjModule *get_math_module(int count, UNUSED(char **parts)) {
    CHECK_PART_COUNT

    if (_math_module == NULL) {
        ObjModule *module = new_module("Math");

        _math_module = module;
    }

    return _math_module;
}
