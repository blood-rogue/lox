#include <readline/history.h>
#include <readline/readline.h>
#include <unistd.h>

#include "builtins.h"
#include "vm.h"

extern void free_vm();

NativeResult exit_builtin_function(int argc, Obj **argv, UNUSED(Obj *callee)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjInt, INT, 0)

    int exit_code = mpz_get_si(argv_0->value);
    free_vm();

    exit(exit_code);

    OK(new_nil());
}

NativeResult print_builtin_function(int argc, Obj **argv, UNUSED(Obj *callee)) {
    for (int i = 0; i < argc; i++) {
        print_object(argv[i]);
        printf(" ");
    }

    printf("\n");
    OK(new_nil());
}

NativeResult repr_builtin_function(int argc, Obj **argv, UNUSED(Obj *callee)) {
    for (int i = 0; i < argc; i++) {
        repr_object(argv[i]);
        printf(" ");
    }

    printf("\n");
    OK(new_nil());
}

NativeResult input_builtin_function(int argc, Obj **argv, UNUSED(Obj *callee)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    char *s = readline(argv_0->chars);

    if (s && *s)
        add_history(s);

    OK(take_string(s, strlen(s)));
}

NativeResult argv_builtin_function(int argc, UNUSED(Obj **argv), UNUSED(Obj *callee)) {
    CHECK_ARG_COUNT(0)

    ObjList *args = argv_list(_argc, _argv);
    OK(args);
}

NativeResult run_gc_builtin_function(int argc, UNUSED(Obj **argv), UNUSED(Obj *callee)) {
    CHECK_ARG_COUNT(0)

    collect_garbage();

    OK(new_nil());
}

NativeResult sleep_builtin_function(int argc, Obj **argv, UNUSED(Obj *callee)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjInt, INT, 0)

    sleep(mpz_get_si(argv_0->value));

    OK(new_nil());
}

NativeResult type_builtin_function(int argc, Obj **argv, UNUSED(Obj *callee)) {
    CHECK_ARG_COUNT(1)

    OK(new_string(get_obj_kind(argv[0]), (int)strlen(get_obj_kind(argv[0]))));
}

NativeResult assert_builtin_function(int argc, Obj **argv, UNUSED(Obj *callee)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjBool, BOOL, 0)

    if (argv_0->value)
        OK(new_nil());
    else
        ERR("Assertion error")
}
