#ifndef clox_object_h
#define clox_object_h

#include "common.h"
#include "chunk.h"
#include "table.h"
#include "value.h"

typedef struct
{
    Value value;
    char *error;
} NativeResult;

#define OBJ_TYPE(value) (AS_OBJ(value)->type)

#define IS_INT(value) isObjType(value, OBJ_INT)
#define IS_BOOL(value) isObjType(value, OBJ_BOOL)
#define IS_BOUND_METHOD(value) isObjType(value, OBJ_BOUND_METHOD)
#define IS_CLASS(value) isObjType(value, OBJ_CLASS)
#define IS_CLOSURE(value) isObjType(value, OBJ_CLOSURE)
#define IS_INSTANCE(value) isObjType(value, OBJ_INSTANCE)
#define IS_FUNCTION(value) isObjType(value, OBJ_FUNCTION)
#define IS_NATIVE(value) isObjType(value, OBJ_NATIVE)
#define IS_STRING(value) isObjType(value, OBJ_STRING)
#define IS_LIST(value) isObjType(value, OBJ_LIST)
#define IS_MAP(value) isObjType(value, OBJ_MAP)

#define AS_INT(value) ((ObjInt *)AS_OBJ(value))
#define AS_BOOL(value) ((ObjBool *)AS_OBJ(value))
#define AS_BOUND_METHOD(value) ((ObjBoundMethod *)AS_OBJ(value))
#define AS_CLASS(value) ((ObjClass *)AS_OBJ(value))
#define AS_INSTANCE(value) ((ObjInstance *)AS_OBJ(value))
#define AS_CLOSURE(value) ((ObjClosure *)AS_OBJ(value))
#define AS_FUNCTION(value) ((ObjFunction *)AS_OBJ(value))
#define AS_NATIVE(value) (((ObjNative *)AS_OBJ(value)))
#define AS_STRING(value) ((ObjString *)AS_OBJ(value))
#define AS_LIST(value) ((ObjList *)AS_OBJ(value))
#define AS_MAP(value) ((ObjMap *)AS_OBJ(value))

typedef enum
{
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

struct ObjString
{
    Obj obj;
    int length;
    char *chars;
    uint32_t hash;
};

typedef struct ObjUpvalue
{
    Obj obj;
    Value *location;
    Value closed;
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

typedef NativeResult (*NativeFn)(int, Value *);

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
    Value receiver;
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

ObjInt *newInt(int64_t);
ObjBool *newBool(bool);
ObjMap *newMap(Value *, int);
ObjList *newList(Value *, int);
ObjBoundMethod *newBoundMethod(Value, ObjClosure *);
ObjClass *newClass(ObjString *);
ObjInstance *newInstance(ObjClass *);
ObjClosure *newClosure(ObjFunction *);
ObjFunction *newFunction();
ObjNative *newNative(NativeFn);
ObjString *newString(const char *, int);
ObjUpvalue *newUpvalue(Value *);
ObjString *takeString(char *, int);

void printObject(Value);
void reprObject(Value);
uint32_t getHash(Obj *);

static inline bool isObjType(Value value, ObjType type)
{
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#endif // clox_object_h
