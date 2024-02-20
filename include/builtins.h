#ifndef clox_native_h
#define clox_native_h

#include "common.h"
#include "object.h"

#define DEFINE_NATIVE(name) NativeResult name##_native(int, Obj **)

DEFINE_NATIVE(clock);
DEFINE_NATIVE(exit);
DEFINE_NATIVE(print);
DEFINE_NATIVE(input);
DEFINE_NATIVE(len);

#endif // clox_native_h
