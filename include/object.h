#ifndef clox_object_h
#define clox_object_h

#include "common.h"
#include "chunk.h"
#include "table.h"
#include "array.h"
#include "builtin_table.h"

#define OBJ_VAL(value) ((Obj *)(value))

#define IS_FLOAT(value) (value->type == OBJ_FLOAT)
#define IS_NIL(value) (value->type == OBJ_NIL)
#define IS_INT(value) (value->type == OBJ_INT)
#define IS_BOOL(value) (value->type == OBJ_BOOL)
#define IS_BOUND_METHOD(value) (value->type == OBJ_BOUND_METHOD)
#define IS_CLASS(value) (value->type == OBJ_CLASS)
#define IS_CLOSURE(value) (value->type == OBJ_CLOSURE)
#define IS_INSTANCE(value) (value->type == OBJ_INSTANCE)
#define IS_FUNCTION(value) (value->type == OBJ_FUNCTION)
#define IS_BUILTIN_CLASS(value) (value->type == OBJ_BUILTIN_CLASS)
#define IS_BUILTIN_FUNCTION(value) (value->type == OBJ_BUILTIN_FUNCTION)
#define IS_STRING(value) (value->type == OBJ_STRING)
#define IS_LIST(value) (value->type == OBJ_LIST)
#define IS_MAP(value) (value->type == OBJ_MAP)

#define AS_FLOAT(value) ((ObjFloat *)(value))
#define AS_NIL(value) ((ObjNil *)(value))
#define AS_INT(value) ((ObjInt *)(value))
#define AS_BOOL(value) ((ObjBool *)(value))
#define AS_BOUND_METHOD(value) ((ObjBoundMethod *)(value))
#define AS_CLASS(value) ((ObjClass *)(value))
#define AS_INSTANCE(value) ((ObjInstance *)(value))
#define AS_CLOSURE(value) ((ObjClosure *)(value))
#define AS_FUNCTION(value) ((ObjFunction *)(value))
#define AS_BUILTIN_CLASS(value) (((ObjBuiltinClass *)(value)))
#define AS_BUILTIN_FUNCTION(value) (((ObjBuiltinFunction *)(value)))
#define AS_STRING(value) ((ObjString *)(value))
#define AS_LIST(value) ((ObjList *)(value))
#define AS_MAP(value) ((ObjMap *)(value))
#define AS_UPVALUE(value) ((ObjUpvalue *)(value))

typedef enum
{
    OBJ_NIL,
    OBJ_FLOAT,
    OBJ_INT,
    OBJ_BOOL,
    OBJ_BOUND_METHOD,
    OBJ_CLASS,
    OBJ_CLOSURE,
    OBJ_FUNCTION,
    OBJ_INSTANCE,
    OBJ_BUILTIN_CLASS,
    OBJ_BUILTIN_FUNCTION,
    OBJ_STRING,
    OBJ_UPVALUE,
    OBJ_LIST,
    OBJ_MAP,
} ObjType;

struct Obj
{
    ObjType type;
    bool is_marked;
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
    int upvalue_count;
    Chunk chunk;
    ObjString *name;
} ObjFunction;

typedef struct
{
    Obj obj;
    BuiltinTable statics;
    BuiltinTable methods;
} ObjBuiltinClass;

typedef struct
{
    Obj obj;
    BuiltinFn function;
} ObjBuiltinFunction;

typedef struct
{
    Obj obj;
    ObjFunction *function;
    ObjUpvalue **upvalues;
    int upvalue_count;
} ObjClosure;

typedef struct
{
    Obj obj;
    ObjString *name;
    Table methods;
    Table statics;
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
    Array elems;
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
    double value;
} ObjFloat;

typedef struct
{
    Obj obj;
} ObjNil;

ObjNil *new_nil();
ObjFloat *new_float(double);
ObjInt *new_int(int64_t);
ObjBool *new_bool(bool);
ObjMap *new_map(Obj **, int);
ObjList *new_list(Obj **, int);
ObjBoundMethod *new_bound_method(Obj *, ObjClosure *);
ObjClass *new_class(ObjString *);
ObjInstance *new_instance(ObjClass *);
ObjClosure *new_closure(ObjFunction *);
ObjFunction *new_function();
ObjBuiltinClass *new_builtin_class();
ObjBuiltinFunction *new_builtin_function(BuiltinFn);
ObjString *new_string(const char *, int);
ObjUpvalue *new_upvalue(Obj **);

ObjList *argv_list(int, const char **);
ObjString *take_string(char *, int);

void print_object(Obj *);
void repr_object(Obj *);

uint32_t hash_string(const char *, int);
uint32_t get_hash(Obj *);

bool obj_equal(Obj *, Obj *);

#endif // clox_object_h
