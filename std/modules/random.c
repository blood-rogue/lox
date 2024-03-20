#include "builtins.h"

static ObjModule *_random_module = NULL;

static BuiltinResult _random_seed(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjInt, INT, 0)

    if (argv_0->value < 0)
        ERR("Cannot seed using negative number %ld.", argv_0->value)

    srand(argv_0->value);
    OK(new_nil());
}

static BuiltinResult _random_random(int argc, UNUSED(Obj **argv), UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(0)

    OK(new_int(rand()));
}

static BuiltinResult _random_randint(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(ObjInt, INT, 0)
    CHECK_ARG_TYPE(ObjInt, INT, 1)

    OK(new_int(rand() % (argv_1->value + 1 - argv_0->value) + argv_0->value));
}

ObjModule *get_random_module(int count, UNUSED(char **parts)) {
    CHECK_PART_COUNT

    if (_random_module == NULL) {
        ObjModule *module = new_module("random");

        SET_BUILTIN_FN_MEMBER("seed", _random_seed);
        SET_BUILTIN_FN_MEMBER("random", _random_random);
        SET_BUILTIN_FN_MEMBER("randint", _random_randint);
        SET_INT_MEMBER("MAX_RANDOM", RAND_MAX);

        _random_module = module;
    }

    return _random_module;
}
