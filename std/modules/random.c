#include "builtins.h"

static ObjModule *_random_module = NULL;

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

static BuiltinResult _random_randint(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(2)
    CHECK_ARG_TYPE(INT, 0)
    CHECK_ARG_TYPE(INT, 1)

    uint64_t min = AS_INT(argv[0])->value;
    uint64_t max = AS_INT(argv[1])->value;

    return OK(new_int(rand() % (max + 1 - min) + min));
}

ObjModule *get_random_module(int count, UNUSED(char **, parts)) {
    CHECK_PART_COUNT(0)

    if (_random_module == NULL) {
        ObjModule *module = new_module(new_string("random", 6));

        SET_BUILTIN_FN_MEMBER("seed", _random_seed);
        SET_BUILTIN_FN_MEMBER("random", _random_random);
        SET_BUILTIN_FN_MEMBER("randint", _random_randint);
        SET_INT_MEMBER("MAX_RANDOM", RAND_MAX);

        _random_module = module;
    }

    return _random_module;
}
