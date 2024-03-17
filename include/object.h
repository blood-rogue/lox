#ifndef lox_object_h
#define lox_object_h

#include <unitypes.h>

#include "array.h"
#include "chunk.h"
#include "common.h"
#include "method_table.h"
#include "table.h"

#define NUM_OBJS                     19

#define AS_OBJ(obj)                  ((Obj *)(obj))

#define IS_NIL(obj)                  (obj->type == OBJ_NIL)
#define IS_INT(obj)                  (obj->type == OBJ_INT)
#define IS_MAP(obj)                  (obj->type == OBJ_MAP)
#define IS_CHAR(obj)                 (obj->type == OBJ_CHAR)
#define IS_LIST(obj)                 (obj->type == OBJ_LIST)
#define IS_BOOL(obj)                 (obj->type == OBJ_BOOL)
#define IS_BYTES(obj)                (obj->type == OBJ_BYTES)
#define IS_FLOAT(obj)                (obj->type == OBJ_FLOAT)
#define IS_STRING(obj)               (obj->type == OBJ_STRING)
#define IS_CLOSURE(obj)              (obj->type == OBJ_CLOSURE)
#define IS_FUNCTION(obj)             (obj->type == OBJ_FUNCTION)
#define IS_UPVALUE(obj)              (obj->type == OBJ_UPVALUE)
#define IS_CLASS(obj)                (obj->type == OBJ_CLASS)
#define IS_INSTANCE(obj)             (obj->type == OBJ_INSTANCE)
#define IS_BOUND_METHOD(obj)         (obj->type == OBJ_BOUND_METHOD)
#define IS_MODULE(obj)               (obj->type == OBJ_MODULE)
#define IS_BUILTIN_FUNCTION(obj)     (obj->type == OBJ_BUILTIN_FUNCTION)
#define IS_BUILTIN_BOUND_METHOD(obj) (obj->type == OBJ_BUILTIN_BOUND_METHOD)
#define IS_NATIVE_STRUCT(obj)        (obj->type == OBJ_NATIVE_STRUCT)

#define AS_NIL(obj)                  ((ObjNil *)(obj))
#define AS_INT(obj)                  ((ObjInt *)(obj))
#define AS_MAP(obj)                  ((ObjMap *)(obj))
#define AS_CHAR(obj)                 ((ObjChar *)(obj))
#define AS_LIST(obj)                 ((ObjList *)(obj))
#define AS_BOOL(obj)                 ((ObjBool *)(obj))
#define AS_BYTES(obj)                ((ObjBytes *)(obj))
#define AS_FLOAT(obj)                ((ObjFloat *)(obj))
#define AS_STRING(obj)               ((ObjString *)(obj))
#define AS_CLOSURE(obj)              ((ObjClosure *)(obj))
#define AS_FUNCTION(obj)             ((ObjFunction *)(obj))
#define AS_UPVALUE(obj)              ((ObjUpvalue *)(obj))
#define AS_CLASS(obj)                ((ObjClass *)(obj))
#define AS_INSTANCE(obj)             ((ObjInstance *)(obj))
#define AS_BOUND_METHOD(obj)         ((ObjBoundMethod *)(obj))
#define AS_MODULE(obj)               ((ObjModule *)(obj))
#define AS_BUILTIN_FUNCTION(obj)     ((ObjBuiltinFunction *)(obj))
#define AS_BUILTIN_BOUND_METHOD(obj) ((ObjBuiltinBoundMethod *)(obj))
#define AS_NATIVE_STRUCT(obj)        ((ObjNativeStruct *)(obj))

typedef enum {
    OBJ_NIL = 1 << 0,
    OBJ_INT = 1 << 1,
    OBJ_MAP = 1 << 2,
    OBJ_CHAR = 1 << 3,
    OBJ_LIST = 1 << 4,
    OBJ_BOOL = 1 << 5,
    OBJ_BYTES = 1 << 6,
    OBJ_FLOAT = 1 << 7,
    OBJ_STRING = 1 << 8,

    OBJ_CLOSURE = 1 << 9,
    OBJ_FUNCTION = 1 << 10,
    OBJ_UPVALUE = 1 << 11,

    OBJ_CLASS = 1 << 12,
    OBJ_INSTANCE = 1 << 13,
    OBJ_BOUND_METHOD = 1 << 14,

    OBJ_MODULE = 1 << 15,

    OBJ_BUILTIN_FUNCTION = 1 << 16,
    OBJ_BUILTIN_BOUND_METHOD = 1 << 17,
    OBJ_NATIVE_STRUCT = 1 << 18
} ObjType;

struct Obj {
    ObjType type;
    bool is_marked;
    uint64_t hash;
    struct Obj *next;
};

typedef struct {
    Obj obj;
} ObjNil;

typedef struct {
    Obj obj;
    int64_t value;
} ObjInt;

typedef struct {
    Obj obj;
    Table table;
} ObjMap;

typedef struct {
    Obj obj;
    ucs4_t value;
} ObjChar;

typedef struct {
    Obj obj;
    Array elems;
} ObjList;

typedef struct {
    Obj obj;
    bool value;
} ObjBool;

typedef struct {
    Obj obj;
    int length;
    uint8_t *bytes;
} ObjBytes;

typedef struct {
    Obj obj;
    double value;
} ObjFloat;

typedef struct {
    Obj obj;
    int raw_length;
    int length;
    char *chars;
} ObjString;

typedef struct {
    Obj obj;
    int arity;
    int upvalue_count;
    Chunk chunk;
    ObjString *name;
} ObjFunction;

typedef struct ObjUpvalue {
    Obj obj;
    Obj **location;
    Obj *closed;
    struct ObjUpvalue *next;
} ObjUpvalue;

typedef struct {
    Obj obj;
    ObjFunction *function;
    ObjUpvalue **upvalues;
    int upvalue_count;
} ObjClosure;

typedef struct {
    Obj obj;
    ObjString *name;
    bool is_builtin;
    Table fields;
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
    ObjString *name;
    Table globals;
} ObjModule;

typedef struct {
    Obj obj;
    char *name;
    BuiltinFn method;
} ObjBuiltinFunction;

typedef struct {
    Obj obj;
    Obj *caller;
    char *name;
    BuiltinFn function;
} ObjBuiltinBoundMethod;

typedef struct {
    Obj obj;
    void *ptr;
} ObjNativeStruct;

ObjNil *new_nil();
ObjInt *new_int(int64_t);
ObjMap *new_map(Obj **, int);
ObjChar *new_char(ucs4_t);
ObjList *new_list(Obj **, int);
ObjBool *new_bool(bool);
ObjBytes *new_bytes(const uint8_t *, int);
ObjFloat *new_float(double);
ObjString *new_string(const char *, int);

ObjClosure *new_closure(ObjFunction *);
ObjFunction *new_function();
ObjUpvalue *new_upvalue(Obj **);

ObjClass *new_class(ObjString *);
ObjInstance *new_instance(ObjClass *);
ObjBoundMethod *new_bound_method(Obj *, ObjClosure *);
ObjModule *new_module(ObjString *);

ObjBuiltinFunction *new_builtin_function(BuiltinFn, char *);
ObjBuiltinBoundMethod *new_builtin_bound_method(BuiltinFn, Obj *, char *);
ObjNativeStruct *new_native_struct(void *);

ObjList *argv_list(int, const char **);
ObjBytes *take_bytes(uint8_t *, int);
ObjString *take_string(char *, int);

int utf_strlen(char *);

void print_object(Obj *);
void repr_object(Obj *);

uint32_t hash_string(const char *, int);

bool obj_equal(Obj *, Obj *);
bool is_hashable(Obj *);
char *get_obj_kind(Obj *);

void init_literals();
void free_literals();

#endif // lox_object_h
