#include <readline/history.h>
#include <readline/readline.h>
#include <unistd.h>

#include "builtins.h"
#include "vm.h"

extern void free_vm();

BuiltinResult exit_builtin_function(int argc, Obj **argv, UNUSED(Obj *, callee)) {
    CHECK_ARG_COUNT(1)

    if (IS_INT(argv[0])) {
        int exit_code = AS_INT(argv[0])->value;
        free_vm();

        exit(exit_code);
    } else {
        return ERR("Cannot exit with non integer exit code");
    }

    return OK(new_nil());
}

BuiltinResult print_builtin_function(int argc, Obj **argv, UNUSED(Obj *, callee)) {
    for (int i = 0; i < argc; i++) {
        print_object(argv[i]);
        printf(" ");
    }

    printf("\n");
    return OK(new_nil());
}

BuiltinResult repr_builtin_function(int argc, Obj **argv, UNUSED(Obj *, callee)) {
    for (int i = 0; i < argc; i++) {
        repr_object(argv[i]);
        printf(" ");
    }

    printf("\n");
    return OK(new_nil());
}

BuiltinResult input_builtin_function(int argc, Obj **argv, UNUSED(Obj *, callee)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(STRING, 0)

    char *s = readline(AS_STRING(argv[0])->chars);

    if (s && *s)
        add_history(s);

    return OK(take_string(s, strlen(s)));
}

BuiltinResult argv_builtin_function(int argc, UNUSED(Obj **, argv), UNUSED(Obj *, callee)) {
    CHECK_ARG_COUNT(0)

    ObjList *args = argv_list(_argc, _argv);
    return OK(args);
}

BuiltinResult run_gc_builtin_function(int argc, UNUSED(Obj **, argv), UNUSED(Obj *, callee)) {
    CHECK_ARG_COUNT(0)

    collect_garbage();

    return OK(new_nil());
}

BuiltinResult parse_int_builtin_function(int argc, Obj **argv, UNUSED(Obj *, callee)) {
    CHECK_ARG_COUNT(1)

    switch (argv[0]->type) {
        case OBJ_STRING:
            return OK(new_int((int64_t)strtol(AS_STRING(argv[0])->chars, NULL, 10)));
        case OBJ_INT:
            return OK(argv[0]);
        case OBJ_FLOAT:
            return OK(new_int((int64_t)(AS_FLOAT(argv[0])->value)));
        default:
            return ERR("Cannot parse to int.");
    }
}

BuiltinResult parse_float_builtin_function(int argc, Obj **argv, UNUSED(Obj *, callee)) {
    CHECK_ARG_COUNT(1)

    switch (argv[0]->type) {
        case OBJ_STRING:
            return OK(new_float(strtod(AS_STRING(argv[0])->chars, NULL)));
        case OBJ_FLOAT:
            return OK(argv[0]);
        case OBJ_INT:
            return OK(new_float((double)(AS_INT(argv[0])->value)));
        default:
            return ERR("Cannot parse to int.");
    }
}

BuiltinResult sleep_builtin_function(int argc, Obj **argv, UNUSED(Obj *, callee)) {
    CHECK_ARG_COUNT(1)
    sleep(AS_INT(argv[0])->value);

    return OK(new_nil());
}

BuiltinResult type_builtin_function(int argc, Obj **argv, UNUSED(Obj *, callee)) {
    CHECK_ARG_COUNT(1)

    return OK(new_string(get_obj_kind(argv[0]), (int)strlen(get_obj_kind(argv[0]))));
}
