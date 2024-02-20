#ifndef clox_builtin_h
#define clox_builtin_h

#include "common.h"
#include "object.h"

#define DEFINE_BUILTIN(name) BuiltinResult name##_builtin(int, Obj **)

DEFINE_BUILTIN(clock);
DEFINE_BUILTIN(exit);
DEFINE_BUILTIN(print);
DEFINE_BUILTIN(input);
DEFINE_BUILTIN(len);
DEFINE_BUILTIN(argv);

#endif // clox_builtin_h
