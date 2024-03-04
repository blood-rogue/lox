#include <unistd.h>

#include "builtins.h"

static ObjModule *_process_module = NULL;

static BuiltinResult _process_chdir(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(STRING, 0)

    if (chdir(AS_STRING(argv[0])->chars) == 0)
        return OK(new_nil());

    return ERR("Could not change working directory of current process.");
}

ObjModule *get_process_module() {
    if (_process_module == NULL) {
        ObjModule *module = new_module(new_string("process", 7));

        SET_BUILTIN_FN_MEMBER("chdir", _process_chdir);

        _process_module = module;
    }

    return _process_module;
}
