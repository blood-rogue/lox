#include "builtins.h"

#define SET_TABLE(name) SET_MODULE_TABLE(random, name)

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

static ObjModule *__random_module = NULL;

ObjModule *get_random_module() {
    if (__random_module == NULL) {
        ObjModule *module = new_module(new_string("random", 6));

        SET_TABLE(seed);
        SET_TABLE(random);

        SET_INT_VAR(MAX_RANDOM, RAND_MAX);

        __random_module = module;
    }

    return __random_module;
}
