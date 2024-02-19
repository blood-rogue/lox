#ifndef clox_value_h
#define clox_value_h

#include "common.h"

typedef struct Obj Obj;
typedef struct ObjString ObjString;

typedef enum
{
    VAL_NIL,
    VAL_OBJ,
} ValueType;

typedef struct
{
    ValueType type;
    union
    {
        double number;
        Obj *obj;
    } as;
} Value;

#define IS_NIL(value) ((value).type == VAL_NIL)
#define IS_OBJ(value) ((value).type == VAL_OBJ)

#define AS_OBJ(value) ((value).as.obj)

#define NIL_VAL ((Value){VAL_NIL, {.number = 0}})
#define OBJ_VAL(object) ((Value){VAL_OBJ, {.obj = (Obj *)object}})

typedef struct
{
    int capacity;
    int count;
    Value *values;
} ValueArray;

bool valuesEqual(Value, Value);

void initValueArray(ValueArray *);
void writeValueArray(ValueArray *, Value);
void freeValueArray(ValueArray *);

void printValue(Value);
void reprValue(Value);

#endif // clox_value_h
