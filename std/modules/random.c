#include "native.h"

static ObjModule *_random_module = NULL;
static gmp_randstate_t rstate;

static NativeResult _random_seed(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjInt, INT, 0)

    gmp_randseed(rstate, argv_0->value);
    OK(new_nil());
}

static NativeResult _random_randint(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjInt, INT, 0)

    mpz_t random;
    mpz_init(random);
    mpz_urandomm(random, rstate, argv_0->value);

    OK(new_int(random));
}

ObjModule *get_random_module(int count, UNUSED(char **parts)) {
    CHECK_PART_COUNT

    if (_random_module == NULL) {
        ObjModule *module = new_module("random");

        gmp_randinit_mt(rstate);

        SET_NATIVE_FN_MEMBER("seed", _random_seed);
        SET_NATIVE_FN_MEMBER("randint", _random_randint);
        SET_INT_MEMBER("MAX_RANDOM", RAND_MAX);

        _random_module = module;
    }

    return _random_module;
}
