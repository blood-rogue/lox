#include "builtins.h"

static BuiltinResult _random_seed(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)

    if (IS_INT(argv[0])) {
        ObjInt *arg = AS_INT(argv[0]);
        if (arg->value < 0)
            return ERR("Cannot seed using negative number.");

        srand(arg->value);
        return OK(new_nil());
    }

    return ERR("Seeding requires INT.");
}

static BuiltinResult _random_random(int argc, UNUSED(Obj **, argv), UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(0)

    return OK(new_int(rand()));
}

static ObjModule *_random_module = NULL;

ObjModule *get_random_module() {
    if (_random_module == NULL) {
        ObjModule *module = new_module(new_string("random", 6));

        SET_BUILTIN_FN_MEMBER("seed", _random_seed);
        SET_BUILTIN_FN_MEMBER("random", _random_random);
        SET_INT_MEMBER("MAX_RANDOM", RAND_MAX);

        _random_module = module;
    }

    return _random_module;
}
