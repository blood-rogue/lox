#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, objectType) (type *)allocateObject(sizeof(type), objectType)

static Obj *allocateObject(size_t size, ObjType type)
{
    Obj *object = (Obj *)reallocate(NULL, 0, size);
    object->type = type;
    object->isMarked = false;

    object->next = vm.objects;
    vm.objects = object;

    return object;
}

ObjMap *newMap(Value *elems, int pairCount)
{
    ObjMap *map = ALLOCATE_OBJ(ObjMap, OBJ_MAP);
    initTable(&map->table);
    map->table.count = pairCount;

    for (int i = 0; i < pairCount; i++)
    {
        tableSet(&map->table, AS_STRING(elems[i * 2]), elems[i * 2 + 1]);
    }

    return map;
}

ObjList *newList(Value *elems, int elemCount)
{
    ObjList *list = ALLOCATE_OBJ(ObjList, OBJ_LIST);

    ValueArray arr;
    initValueArray(&arr);

    for (int i = 0; i < elemCount; i++)
    {
        writeValueArray(&arr, elems[i]);
    }

    list->elems = arr;

    return list;
}

ObjBoundMethod *newBoundMethod(Value receiver, ObjClosure *method)
{
    ObjBoundMethod *bound = ALLOCATE_OBJ(ObjBoundMethod, OBJ_BOUND_METHOD);
    bound->receiver = receiver;
    bound->method = method;

    return bound;
}

ObjClass *newClass(ObjString *name)
{
    ObjClass *klass = ALLOCATE_OBJ(ObjClass, OBJ_CLASS);
    klass->name = name;
    initTable(&klass->methods);

    return klass;
}

ObjInstance *newInstance(ObjClass *klass)
{
    ObjInstance *instance = ALLOCATE_OBJ(ObjInstance, OBJ_INSTANCE);
    instance->klass = klass;
    initTable(&instance->fields);

    return instance;
}

ObjClosure *newClosure(ObjFunction *function)
{
    ObjUpvalue **upvalues = ALLOCATE(ObjUpvalue *, function->upvalueCount);
    for (int i = 0; i < function->upvalueCount; i++)
    {
        upvalues[i] = NULL;
    }

    ObjClosure *closure = ALLOCATE_OBJ(ObjClosure, OBJ_CLOSURE);
    closure->function = function;

    closure->upvalues = upvalues;
    closure->upvalueCount = function->upvalueCount;

    return closure;
}

ObjFunction *newFunction()
{
    ObjFunction *function = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION);
    function->arity = 0;
    function->name = NULL;
    function->upvalueCount = 0;

    initChunk(&function->chunk);
    return function;
}

ObjNative *newNative(NativeFn function)
{
    ObjNative *native = ALLOCATE_OBJ(ObjNative, OBJ_NATIVE);
    native->function = function;
    return native;
}

static ObjString *allocateString(char *chars, int length, uint32_t hash)
{
    ObjString *string = ALLOCATE_OBJ(ObjString, OBJ_STRING);

    string->length = length;
    string->chars = chars;
    string->hash = hash;

    push(OBJ_VAL(string));
    tableSet(&vm.strings, string, NIL_VAL);
    pop();

    return string;
}

static uint32_t hashString(const char *key, int length)
{
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++)
    {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;
}

ObjString *takeString(char *chars, int length)
{
    uint32_t hash = hashString(chars, length);
    ObjString *interned = tableFindString(&vm.strings, chars, length, hash);
    if (interned != NULL)
    {
        FREE_ARRAY(char, chars, length + 1);
        return interned;
    }

    return allocateString(chars, length, hash);
}

ObjString *newString(const char *chars, int length)
{
    uint32_t hash = hashString(chars, length);

    ObjString *interned = tableFindString(&vm.strings, chars, length, hash);

    if (interned != NULL)
        return interned;

    char *heapChars = ALLOCATE(char, length + 1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';
    return allocateString(heapChars, length, hash);
}

ObjUpvalue *newUpvalue(Value *slot)
{
    ObjUpvalue *upvalue = ALLOCATE_OBJ(ObjUpvalue, OBJ_UPVALUE);
    upvalue->closed = NIL_VAL;
    upvalue->location = slot;

    upvalue->next = NULL;
    return upvalue;
}

static void printFunction(ObjFunction *function)
{
    if (function->name == NULL)
    {
        printf("<script>");
        return;
    }
    printf("<fn %s>", function->name->chars);
}

static void printList(ValueArray *elems)
{
    Value *values = elems->values;

    printf("[");
    if (elems->count > 0)
    {
        printValue(values[0]);
        for (int i = 1; i < elems->count; i++)
        {
            printf(", ");
            printValue(values[i]);
        }
    }
    printf("]");
}

static void printMap(ObjMap *map)
{
    printf("{");
    int count = map->table.count;
    Value value;

    while (count > 0)
    {
        for (int i = 0; i < map->table.capacity; i++)
        {
            Entry *entry = &map->table.entries[i];

            if (entry->key == NULL)
                continue;

            printf("\n\t\"%s\": ", entry->key->chars);
            tableGet(&map->table, entry->key, &value);
            reprValue(value);
            printf(",");

            count--;
        }
    }

    printf("\n}");
}

void printObject(Value value)
{
    switch (OBJ_TYPE(value))
    {
    case OBJ_MAP:
        printMap(AS_MAP(value));
        break;
    case OBJ_LIST:
        printList(&AS_LIST(value)->elems);
        break;
    case OBJ_BOUND_METHOD:
        printFunction(AS_BOUND_METHOD(value)->method->function);
        break;
    case OBJ_CLASS:
        printf("<class '%s'>", AS_CLASS(value)->name->chars);
        break;
    case OBJ_CLOSURE:
        printFunction(AS_CLOSURE(value)->function);
        break;
    case OBJ_STRING:
        printf("%s", AS_STRING(value)->chars);
        break;
    case OBJ_INSTANCE:
        printf("<'%s' instance>", AS_INSTANCE(value)->klass->name->chars);
        break;
    case OBJ_NATIVE:
        printf("<native fn>");
        break;
    case OBJ_FUNCTION:
        printFunction(AS_FUNCTION(value));
        break;
    case OBJ_UPVALUE:
        printf("upvalue");
        break;
    }
}

void reprObject(Value value)
{
    switch (OBJ_TYPE(value))
    {
    case OBJ_STRING:
        printf("\"%s\"", AS_STRING(value)->chars);
        break;
    default:
        printObject(value);
        break;
    }
}