#include "builtins.h"

BuiltinResult clock_builtin_function(int arg_count, Obj **args) {
    CHECK_ARG_COUNT(0)
    return OK(AS_OBJ(new_int(clock() / CLOCKS_PER_SEC)));
}

BuiltinResult exit_builtin_function(int arg_count, Obj **args) {

    CHECK_ARG_COUNT(1)

    if (IS_INT(args[0])) {
        int exit_code = AS_INT(args[0])->value;
        sweep();
        exit(exit_code);
    } else {
        return ERR("Cannot exit with non integer exit code");
    }

    return OK(AS_OBJ(new_nil()));
}

BuiltinResult print_builtin_function(int arg_count, Obj **args) {
    for (int i = 0; i < arg_count; i++) {
        print_object(args[i]);
        printf(" ");
    }

    printf("\n");
    return OK(AS_OBJ(new_nil()));
}

BuiltinResult input_builtin_function(int arg_count, Obj **args) {
    if (arg_count > 0)
        print_object(args[0]);

    int capacity = 8;
    char *s = (char *)malloc(capacity);
    int len = 0;

    char c;

    for (;;) {
        if (++len == capacity)
            s = (char *)realloc(s, (capacity *= 2));

        c = (char)getchar();
        if (c == '\n' || c == EOF)
            break;
        else
            s[len - 1] = c;
    }

    s[len - 1] = '\0';
    return OK(AS_OBJ(take_string(s, len)));
}

BuiltinResult len_builtin_function(int arg_count, Obj **args) {
    CHECK_ARG_COUNT(1)

    Obj *obj = args[0];
    switch (obj->type) {
        case OBJ_LIST:
            {
                ObjList *list = AS_LIST(obj);
                return OK(AS_OBJ(new_int(list->elems.count)));
            }
        case OBJ_MAP:
            {
                ObjMap *map = AS_MAP(obj);
                return OK(AS_OBJ(new_int(map->table.count)));
            }
        case OBJ_STRING:
            {
                ObjString *string = AS_STRING(obj);
                return OK(AS_OBJ(new_int(string->length)));
            }
        default:
            return ERR("len() is not defined for the type");
    }
}

BuiltinResult argv_builtin_function(int arg_count, Obj **args) {
    CHECK_ARG_COUNT(0)

    ObjList *argv = argv_list(_argc, _argv);
    return OK(AS_OBJ(argv));
}

BuiltinResult run_gc_builtin_function(int arg_count, Obj **args) {
    CHECK_ARG_COUNT(0)

    collect_garbage();

    return OK(AS_OBJ(new_nil()));
}