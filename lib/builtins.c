#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "builtins.h"

#define ERR(err) \
    (NativeResult) { .error = err, .value = NIL_VAL }
#define OK(ok) \
    (NativeResult) { .value = ok, .error = NULL }

#define CHECK_ARG_COUNT(expected)                                             \
    if (argCount != expected)                                                 \
    {                                                                         \
        char buf[50];                                                         \
        sprintf(buf, "Expected %d arguments but got %d", expected, argCount); \
        return ERR(buf);                                                      \
    }

NativeResult clockNative(int argCount, Value *args)
{
    CHECK_ARG_COUNT(0)
    return OK(OBJ_VAL(newInt(clock() / CLOCKS_PER_SEC)));
}

NativeResult exitNative(int argCount, Value *args)
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

    return OK(NIL_VAL);
}

NativeResult printNative(int argCount, Value *args)
{
    for (int i = 0; i < argCount; i++)
    {
        printValue(args[i]);
        printf(" ");
    }

    printf("\n");
    return OK(NIL_VAL);
}

NativeResult inputNative(int argCount, Value *args)
{
    if (argCount > 0)
        printValue(args[0]);

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

NativeResult lenNative(int argCount, Value *args)
{
    CHECK_ARG_COUNT(1)

    if (IS_OBJ(args[0]))
    {
        Obj *obj = AS_OBJ(args[0]);
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

    return ERR("len() is not defined for the type");
}
