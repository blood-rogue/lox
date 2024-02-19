#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "builtins.h"

#define ERR(err) \
    (NativeResult) { .error = err, .value = OBJ_VAL(newNil()) }
#define OK(ok) \
    (NativeResult) { .value = ok, .error = NULL }

#define CHECK_ARG_COUNT(expected)                                             \
    if (argCount != expected)                                                 \
    {                                                                         \
        char buf[50];                                                         \
        sprintf(buf, "Expected %d arguments but got %d", expected, argCount); \
        return ERR(buf);                                                      \
    }

NativeResult clockNative(int argCount, Obj **args)
{
    CHECK_ARG_COUNT(0)
    return OK(OBJ_VAL(newInt(clock() / CLOCKS_PER_SEC)));
}

NativeResult exitNative(int argCount, Obj **args)
{

    CHECK_ARG_COUNT(1)

    if (IS_INT(args[0]))
    {
        int exitCode = AS_INT(args[0])->value;
        exit(exitCode);
    }
    else
    {
        return ERR("Cannot exit with non integer exit code");
    }

    return OK(OBJ_VAL(newNil()));
}

NativeResult printNative(int argCount, Obj **args)
{
    for (int i = 0; i < argCount; i++)
    {
        printObject(args[i]);
        printf(" ");
    }

    printf("\n");
    return OK(OBJ_VAL(newNil()));
}

NativeResult inputNative(int argCount, Obj **args)
{
    if (argCount > 0)
        printObject(args[0]);

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
    return OK(OBJ_VAL(takeString(s, len)));
}

NativeResult lenNative(int argCount, Obj **args)
{
    CHECK_ARG_COUNT(1)

    Obj *obj = args[0];
    switch (obj->type)
    {
    case OBJ_LIST:
    {
        ObjList *list = (ObjList *)(obj);
        return OK(OBJ_VAL(newInt(list->elems.count)));
    }
    case OBJ_MAP:
    {
        ObjMap *map = (ObjMap *)(obj);
        return OK(OBJ_VAL(newInt(map->table.count)));
    }
    case OBJ_STRING:
    {
        ObjString *string = (ObjString *)(obj);
        return OK(OBJ_VAL(newInt(string->length)));
    }
    default:
        return ERR("len() is not defined for the type");
    }
}
