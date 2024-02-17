#ifndef clox_native_h
#define clox_native_h

#include "common.h"
#include "object.h"

#define DEFINE_NATIVE(name) Value name##Native(int argCount, Value *args)

DEFINE_NATIVE(clock);
DEFINE_NATIVE(exit);
DEFINE_NATIVE(print);
DEFINE_NATIVE(input);
DEFINE_NATIVE(len);

#endif // clox_native_h
