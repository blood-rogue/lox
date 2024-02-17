#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "builtins.h"

Value clockNative(int argCount, Value *args)
{
    return NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
}

Value exitNative(int argCount, Value *args)
{
    if (argCount == 0)
    {
        exit(0);
    }
    else if (argCount == 1 && IS_NUMBER(args[0]))
    {
        int exitCode = AS_NUMBER(args[0]);
        exit(exitCode);
    }

    return NIL_VAL;
}

Value printNative(int argCount, Value *args)
{
    for (int i = 0; i < argCount; i++)
    {
        printValue(args[i]);
        printf(" ");
    }

    printf("\n");
    return NIL_VAL;
}

Value inputNative(int argCount, Value *args)
{
    if (argCount > 0)
    {
        printValue(args[0]);
    }

    size_t block_size = 256 * sizeof(char);
    char *s = (char *)malloc(block_size);
    int capacity = block_size;
    int len = 0;

    char c;

    for (;;)
    {
        if (++len == capacity)
        {
            capacity += block_size;
            s = (char *)realloc(s, capacity);
            if (s == NULL)
            {
                printf("Couldn't take input");
            }
        }

        c = (char)getchar();
        if (c == '\n' || c == EOF)
        {
            len--;
            break;
        }
        else
        {
            s[len - 1] = c;
        }
    }

    if (len > 0)
    {
        s = (char *)realloc(s, len);
    }

    return OBJ_VAL(newString(s, len));
}

Value lenNative(int argCount, Value *args)
{
    if (argCount != 0)
    {
    }

    return NIL_VAL;
}