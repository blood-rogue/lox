#ifndef clox_object_h
#define clox_object_h

#include "array.h"
// #include "builtin_table.h"
#include "chunk.h"
#include "common.h"
#include "method_table.h"
#include "table.h"

#define AS_OBJ(obj)              ((Obj *)(obj))

#define IS_FLOAT(obj)            (obj->type == OBJ_FLOAT)
#define IS_NIL(obj)              (obj->type == OBJ_NIL)
#define IS_INT(obj)              (obj->type == OBJ_INT)
#define IS_BOOL(obj)             (obj->type == OBJ_BOOL)
#define IS_BOUND_METHOD(obj)     (obj->type == OBJ_BOUND_METHOD)
#define IS_CLASS(obj)            (obj->type == OBJ_CLASS)
#define IS_CLOSURE(obj)          (obj->type == OBJ_CLOSURE)
#define IS_INSTANCE(obj)         (obj->type == OBJ_INSTANCE)
#define IS_FUNCTION(obj)         (obj->type == OBJ_FUNCTION)
#define IS_BUILTIN_CLASS(obj)    (obj->type == OBJ_BUILTIN_CLASS)
#define IS_BUILTIN_FUNCTION(obj) (obj->type == OBJ_BUILTIN_FUNCTION)
#define IS_STRING(obj)           (obj->type == OBJ_STRING)
#define IS_LIST(obj)             (obj->type == OBJ_LIST)
#define IS_MAP(obj)              (obj->type == OBJ_MAP)

#define AS_FLOAT(obj)            ((ObjFloat *)(obj))
#define AS_NIL(obj)              ((ObjNil *)(obj))
#define AS_INT(obj)              ((ObjInt *)(obj))
#define AS_BOOL(obj)             ((ObjBool *)(obj))
#define AS_BOUND_METHOD(obj)     ((ObjBoundMethod *)(obj))
#define AS_CLASS(obj)            ((ObjClass *)(obj))
#define AS_INSTANCE(obj)         ((ObjInstance *)(obj))
#define AS_CLOSURE(obj)          ((ObjClosure *)(obj))
#define AS_FUNCTION(obj)         ((ObjFunction *)(obj))
#define AS_BUILTIN_CLASS(obj)    ((ObjBuiltinClass *)(obj))
#define AS_BUILTIN_FUNCTION(obj) ((ObjBuiltinFunction *)(obj))
#define AS_STRING(obj)           ((ObjString *)(obj))
#define AS_LIST(obj)             ((ObjList *)(obj))
#define AS_MAP(obj)              ((ObjMap *)(obj))
#define AS_UPVALUE(obj)          ((ObjUpvalue *)(obj))

typedef enum {
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

struct Obj {
    ObjType type;
    bool is_marked;
    struct Obj *next;
    BuiltinMethodTable *statics;
};

typedef struct {
    Obj obj;
    int length;
    char *chars;
    uint32_t hash;
} ObjString;

typedef struct ObjUpvalue {
    Obj obj;
    Obj **location;
    Obj *closed;
    struct ObjUpvalue *next;
} ObjUpvalue;

typedef struct {
    Obj obj;
    int arity;
    int upvalue_count;
    Chunk chunk;
    ObjString *name;
} ObjFunction;

typedef struct {
    Obj obj;
    BuiltinMethodTable methods;
} ObjBuiltinClass;

typedef struct {
    Obj obj;
    BuiltinMethodFn method;
} ObjBuiltinFunction;

typedef struct {
    Obj obj;
    ObjFunction *function;
    ObjUpvalue **upvalues;
    int upvalue_count;
} ObjClosure;

typedef struct {
    Obj obj;
    ObjString *name;
    Table methods;
    Table statics;
} ObjClass;

typedef struct {
    Obj obj;
    ObjClass *klass;
    Table fields;
} ObjInstance;

typedef struct {
    Obj obj;
    Obj *receiver;
    ObjClosure *method;
} ObjBoundMethod;

typedef struct {
    Obj obj;
    Array elems;
} ObjList;

typedef struct {
    Obj obj;
    Table table;
} ObjMap;

typedef struct {
    Obj obj;
    bool value;
} ObjBool;

typedef struct {
    Obj obj;
    int64_t value;
} ObjInt;

typedef struct {
    Obj obj;
    double value;
} ObjFloat;

typedef struct {
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
ObjBuiltinClass *new_builtin_class(int);
ObjBuiltinFunction *new_builtin_method(BuiltinMethodFn);
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
