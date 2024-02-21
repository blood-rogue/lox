#include "builtins.h"

#define STATIC(name) BuiltinResult __int_##name##_builtin_static(int arg_count, Obj **args)
#define SET_STATIC(name, len)    \
    builtin_table_set(           \
        &int_class->statics,     \
        #name,                   \
        hash_string(#name, len), \
        __int_##name##_builtin_static)

STATIC(parse);

ObjBuiltinClass *int_builtin_class()
{
    ObjBuiltinClass *int_class = new_builtin_class();

    SET_STATIC(parse, 5);

    return int_class;
}

STATIC(parse)
{
    CHECK_ARG_COUNT(1)
    Obj *arg = args[0];

    switch (arg->type)
    {
    case OBJ_INT:
        return OK(OBJ_VAL(arg));
    case OBJ_STRING:
        return OK(OBJ_VAL(new_int((int64_t)strtol(AS_STRING(arg)->chars, NULL, 10))));
    default:
        return ERR("Cannot parse to int");
    }
    return OK(OBJ_VAL(AS_INT(arg)));
}