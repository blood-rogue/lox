#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, object_type) (type *)allocate_object(sizeof(type), object_type)

static Obj *allocate_object(size_t size, ObjType type)
{
    Obj *object = (Obj *)reallocate(NULL, 0, size);
    object->type = type;
    object->is_marked = false;

    object->next = vm.objects;
    vm.objects = object;

    return object;
}

ObjNil *new_nil()
{
    return ALLOCATE_OBJ(ObjNil, OBJ_NIL);
}

ObjInt *new_int(int64_t value)
{
    ObjInt *integer = ALLOCATE_OBJ(ObjInt, OBJ_INT);
    integer->value = value;

    return integer;
}

ObjBool *new_bool(bool value)
{
    ObjBool *boolean = ALLOCATE_OBJ(ObjBool, OBJ_BOOL);
    boolean->value = value;

    return boolean;
}

ObjMap *new_map(Obj **elems, int pair_count)
{
    ObjMap *map = ALLOCATE_OBJ(ObjMap, OBJ_MAP);
    init_table(&map->table);
    map->table.count = pair_count;

    for (int i = 0; i < pair_count; i++)
    {
        table_set(&map->table, elems[i * 2], elems[i * 2 + 1]);
    }

    return map;
}

ObjList *new_list(Obj **elems, int elem_count)
{
    ObjList *list = ALLOCATE_OBJ(ObjList, OBJ_LIST);

    Array arr;
    init_array(&arr);

    for (int i = 0; i < elem_count; i++)
    {
        write_array(&arr, elems[i]);
    }

    list->elems = arr;

    return list;
}

ObjBoundMethod *new_bound_method(Obj *receiver, ObjClosure *method)
{
    ObjBoundMethod *bound = ALLOCATE_OBJ(ObjBoundMethod, OBJ_BOUND_METHOD);
    bound->receiver = receiver;
    bound->method = method;

    return bound;
}

ObjClass *new_class(ObjString *name)
{
    ObjClass *klass = ALLOCATE_OBJ(ObjClass, OBJ_CLASS);
    klass->name = name;
    init_table(&klass->methods);

    return klass;
}

ObjInstance *new_instance(ObjClass *klass)
{
    ObjInstance *instance = ALLOCATE_OBJ(ObjInstance, OBJ_INSTANCE);
    instance->klass = klass;
    init_table(&instance->fields);

    return instance;
}

ObjClosure *new_closure(ObjFunction *function)
{
    ObjUpvalue **upvalues = ALLOCATE(ObjUpvalue *, function->upvalue_count);
    for (int i = 0; i < function->upvalue_count; i++)
    {
        upvalues[i] = NULL;
    }

    ObjClosure *closure = ALLOCATE_OBJ(ObjClosure, OBJ_CLOSURE);
    closure->function = function;

    closure->upvalues = upvalues;
    closure->upvalue_count = function->upvalue_count;

    return closure;
}

ObjFunction *new_function()
{
    ObjFunction *function = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION);
    function->arity = 0;
    function->name = NULL;
    function->upvalue_count = 0;

    init_chunk(&function->chunk);
    return function;
}

ObjBuiltin *new_builtin(BuiltinFn function)
{
    ObjBuiltin *builtin = ALLOCATE_OBJ(ObjBuiltin, OBJ_NATIVE);
    builtin->function = function;
    return builtin;
}

static ObjString *allocate_string(char *chars, int length, uint32_t hash)
{
    ObjString *string = ALLOCATE_OBJ(ObjString, OBJ_STRING);

    string->length = length;
    string->chars = chars;
    string->hash = hash;

    push(OBJ_VAL(string));
    table_set(&vm.strings, (Obj *)string, OBJ_VAL(new_nil()));
    pop();

    return string;
}

static uint32_t hash_string(const char *key, int length)
{
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++)
    {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;
}

ObjString *take_string(char *chars, int length)
{
    uint32_t hash = hash_string(chars, length);
    Obj *interned = table_findString(&vm.strings, chars, length, hash);
    if (interned != NULL && interned->type == OBJ_STRING)
    {
        FREE_ARRAY(char, chars, length + 1);
        return (ObjString *)interned;
    }

    return allocate_string(chars, length, hash);
}

ObjString *new_string(const char *chars, int length)
{
    uint32_t hash = hash_string(chars, length);

    Obj *interned = table_findString(&vm.strings, chars, length, hash);

    if (interned != NULL && interned->type == OBJ_STRING)
        return (ObjString *)interned;

    char *heap_chars = ALLOCATE(char, length + 1);
    memcpy(heap_chars, chars, length);
    heap_chars[length] = '\0';
    return allocate_string(heap_chars, length, hash);
}

ObjUpvalue *new_upvalue(Obj **slot)
{
    ObjUpvalue *upvalue = ALLOCATE_OBJ(ObjUpvalue, OBJ_UPVALUE);
    upvalue->closed = OBJ_VAL(new_nil());
    upvalue->location = slot;

    upvalue->next = NULL;
    return upvalue;
}

static void print_function(ObjFunction *function)
{
    if (function->name == NULL)
    {
        printf("<script>");
        return;
    }
    printf("<fn %s>", function->name->chars);
}

static void print_list(Array *elems)
{
    Obj **values = elems->values;

    printf("[");
    if (elems->count > 0)
    {
        repr_object(values[0]);
        for (int i = 1; i < elems->count; i++)
        {
            printf(", ");
            repr_object(values[i]);
        }
    }
    printf("]");
}

static void print_map(ObjMap *map)
{
    int count = map->table.count;
    Obj *value;

    printf("{");
    for (int i = 0; i < map->table.capacity && count > 0; i++)
    {
        Entry *entry = &map->table.entries[i];

        if (entry->key == NULL)
            continue;

        repr_object(OBJ_VAL(entry->key));
        printf(": ");
        table_get(&map->table, (Obj *)entry->key, &value);
        repr_object(value);
        printf(",");

        count--;
    }

    printf("}");
}

void print_object(Obj *value)
{
    switch (value->type)
    {
    case OBJ_NIL:
        printf("(nil)");
        break;
    case OBJ_INT:
        printf("%ld", AS_INT(value)->value);
        break;
    case OBJ_BOOL:
        printf(AS_BOOL(value)->value ? "true" : "false");
        break;
    case OBJ_MAP:
        print_map(AS_MAP(value));
        break;
    case OBJ_LIST:
        print_list(&AS_LIST(value)->elems);
        break;
    case OBJ_BOUND_METHOD:
        print_function(AS_BOUND_METHOD(value)->method->function);
        break;
    case OBJ_CLASS:
        printf("<class '%s'>", AS_CLASS(value)->name->chars);
        break;
    case OBJ_CLOSURE:
        print_function(AS_CLOSURE(value)->function);
        break;
    case OBJ_STRING:
        printf("%s", AS_STRING(value)->chars);
        break;
    case OBJ_INSTANCE:
        printf("<'%s' instance>", AS_INSTANCE(value)->klass->name->chars);
        break;
    case OBJ_NATIVE:
        printf("<builtin fn>");
        break;
    case OBJ_FUNCTION:
        print_function(AS_FUNCTION(value));
        break;
    case OBJ_UPVALUE:
        printf("upvalue");
        break;
    }
}

void repr_object(Obj *value)
{
    switch (value->type)
    {
    case OBJ_STRING:
        printf("\"%s\"", AS_STRING(value)->chars);
        break;
    default:
        print_object(value);
        break;
    }
}

uint32_t get_hash(Obj *obj)
{
    switch (obj->type)
    {
    case OBJ_STRING:
        return ((ObjString *)obj)->hash;
    default:
        return 0;
    }
}

bool obj_equal(Obj *a, Obj *b)
{
    if (a->type != b->type)
        return false;

    switch (a->type)
    {
    case OBJ_BOOL:
    {
        ObjBool *a = AS_BOOL(a);
        ObjBool *b = AS_BOOL(b);
        return a->value == b->value;
    }
    case OBJ_NIL:
        return true;
    case OBJ_INT:
    {
        ObjInt *a = AS_INT(a);
        ObjInt *b = AS_INT(b);
        return a->value == b->value;
    }
    default:
        return false;
    }
}

ObjList *argv_list(int argc, const char **argv)
{
    ObjList *list = ALLOCATE_OBJ(ObjList, OBJ_LIST);

    Array arr;
    init_array(&arr);

    for (int i = 0; i < argc; i++)
    {
        write_array(&arr, (Obj *)new_string(argv[i], strlen(argv[i])));
    }

    list->elems = arr;

    return list;
}