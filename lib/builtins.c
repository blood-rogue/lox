#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "builtins.h"

#define ERR(err) \
    (BuiltinResult) { .error = err, .value = OBJ_VAL(new_nil()) }
#define OK(ok) \
    (BuiltinResult) { .value = ok, .error = NULL }

#define CHECK_ARG_COUNT(expected)                                              \
    if (arg_count != expected)                                                 \
    {                                                                          \
        char buf[50];                                                          \
        sprintf(buf, "Expected %d arguments but got %d", expected, arg_count); \
        return ERR(buf);                                                       \
    }

BuiltinResult clock_builtin(int arg_count, Obj **args)
{
    CHECK_ARG_COUNT(0)
    return OK(OBJ_VAL(new_int(clock() / CLOCKS_PER_SEC)));
}

BuiltinResult exit_builtin(int arg_count, Obj **args)
{

    CHECK_ARG_COUNT(1)

    if (IS_INT(args[0]))
    {
        int exit_code = AS_INT(args[0])->value;
        exit(exit_code);
    }
    else
    {
        return ERR("Cannot exit with non integer exit code");
    }

    return OK(OBJ_VAL(new_nil()));
}

BuiltinResult print_builtin(int arg_count, Obj **args)
{
    for (int i = 0; i < arg_count; i++)
    {
        print_object(args[i]);
        printf(" ");
    }

    printf("\n");
    return OK(OBJ_VAL(new_nil()));
}

BuiltinResult input_builtin(int arg_count, Obj **args)
{
    if (arg_count > 0)
        print_object(args[0]);

    int capacity = 8;
    char *s = (char *)malloc(capacity);
    int len = 0;

    char c;

    for (;;)
    {
        if (++len == capacity)
            s = (char *)realloc(s, (capacity *= 2));

        c = (char)getchar();
        if (c == '\n' || c == EOF)
            break;
        else
            s[len - 1] = c;
    }

    s[len - 1] = '\0';
    return OK(OBJ_VAL(take_string(s, len)));
}

BuiltinResult len_builtin(int arg_count, Obj **args)
{
    CHECK_ARG_COUNT(1)

    Obj *obj = args[0];
    switch (obj->type)
    {
    case OBJ_LIST:
    {
        ObjList *list = AS_LIST(obj);
        return OK(OBJ_VAL(new_int(list->elems.count)));
    }
    case OBJ_MAP:
    {
        ObjMap *map = AS_MAP(obj);
        return OK(OBJ_VAL(new_int(map->table.count)));
    }
    case OBJ_STRING:
    {
        ObjString *string = AS_STRING(obj);
        return OK(OBJ_VAL(new_int(string->length)));
    }
    default:
        return ERR("len() is not defined for the type");
    }
}

BuiltinResult argv_builtin(int arg_count, Obj **args)
{
    CHECK_ARG_COUNT(0)

    ObjList *argv = argv_list(_argc, _argv);
    return OK(OBJ_VAL(argv));
}