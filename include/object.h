#ifndef clox_object_h
#define clox_object_h

#include "common.h"
#include "chunk.h"
#include "table.h"
#include "value.h"

#define OBJ_VAL(value) ((Obj *)(value))

#define IS_NIL(value) (value->type == OBJ_NIL)
#define IS_INT(value) (value->type == OBJ_INT)
#define IS_BOOL(value) (value->type == OBJ_BOOL)
#define IS_BOUND_METHOD(value) (value->type == OBJ_BOUND_METHOD)
#define IS_CLASS(value) (value->type == OBJ_CLASS)
#define IS_CLOSURE(value) (value->type == OBJ_CLOSURE)
#define IS_INSTANCE(value) (value->type == OBJ_INSTANCE)
#define IS_FUNCTION(value) (value->type == OBJ_FUNCTION)
#define IS_NATIVE(value) (value->type == OBJ_NATIVE)
#define IS_STRING(value) (value->type == OBJ_STRING)
#define IS_LIST(value) (value->type == OBJ_LIST)
#define IS_MAP(value) (value->type == OBJ_MAP)

#define AS_NIL(value) ((ObjNil *)(value))
#define AS_INT(value) ((ObjInt *)(value))
#define AS_BOOL(value) ((ObjBool *)(value))
#define AS_BOUND_METHOD(value) ((ObjBoundMethod *)(value))
#define AS_CLASS(value) ((ObjClass *)(value))
#define AS_INSTANCE(value) ((ObjInstance *)(value))
#define AS_CLOSURE(value) ((ObjClosure *)(value))
#define AS_FUNCTION(value) ((ObjFunction *)(value))
#define AS_NATIVE(value) (((ObjNative *)(value)))
#define AS_STRING(value) ((ObjString *)(value))
#define AS_LIST(value) ((ObjList *)(value))
#define AS_MAP(value) ((ObjMap *)(value))

typedef enum
{
    OBJ_NIL,
    OBJ_INT,
    OBJ_BOOL,
    OBJ_BOUND_METHOD,
    OBJ_CLASS,
    OBJ_CLOSURE,
    OBJ_FUNCTION,
    OBJ_INSTANCE,
    OBJ_NATIVE,
    OBJ_STRING,
    OBJ_UPVALUE,
    OBJ_LIST,
    OBJ_MAP,
} ObjType;

struct Obj
{
    ObjType type;
    bool isMarked;
    struct Obj *next;
};

typedef struct
{
    Obj obj;
    int length;
    char *chars;
    uint32_t hash;
} ObjString;

typedef struct ObjUpvalue
{
    Obj obj;
    Obj **location;
    Obj *closed;
    struct ObjUpvalue *next;
} ObjUpvalue;

typedef struct
{
    Obj obj;
    int arity;
    int upvalueCount;
    Chunk chunk;
    ObjString *name;
    bool isStatic;
} ObjFunction;

typedef struct
{
    Obj *value;
    char *error;
} NativeResult;

typedef NativeResult (*NativeFn)(int argCount, Obj **args);

typedef struct
{
    Obj obj;
    NativeFn function;
} ObjNative;

typedef struct
{
    Obj obj;
    ObjFunction *function;
    ObjUpvalue **upvalues;
    int upvalueCount;
} ObjClosure;

typedef struct
{
    Obj obj;
    ObjString *name;
    Table methods;
} ObjClass;

typedef struct
{
    Obj obj;
    ObjClass *klass;
    Table fields;
} ObjInstance;

typedef struct
{
    Obj obj;
    Obj *receiver;
    ObjClosure *method;
} ObjBoundMethod;

typedef struct
{
    Obj obj;
    ValueArray elems;
} ObjList;

typedef struct
{
    Obj obj;
    Table table;
} ObjMap;

typedef struct
{
    Obj obj;
    bool value;
} ObjBool;

typedef struct
{
    Obj obj;
    int64_t value;
} ObjInt;

typedef struct
{
    Obj obj;
} ObjNil;

ObjNil *newNil();
ObjInt *newInt(int64_t);
ObjBool *newBool(bool);
ObjMap *newMap(Obj **, int);
ObjList *newList(Obj **, int);
ObjBoundMethod *newBoundMethod(Obj *, ObjClosure *);
ObjClass *newClass(ObjString *);
ObjInstance *newInstance(ObjClass *);
ObjClosure *newClosure(ObjFunction *);
ObjFunction *newFunction();
ObjNative *newNative(NativeFn);
ObjString *newString(const char *, int);
ObjUpvalue *newUpvalue(Obj **);
ObjString *takeString(char *, int);

void printObject(Obj *);
void reprObject(Obj *);
uint32_t getHash(Obj *);

bool objEqual(Obj *, Obj *);

#endif // clox_object_h
