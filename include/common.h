#ifndef clox_common_h
#define clox_common_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UINT8_COUNT (UINT8_MAX + 1)

typedef struct Obj Obj;

extern int _argc;
extern const char **_argv;

#endif // clox_common_h
