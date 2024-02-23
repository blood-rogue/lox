#include <stdlib.h>

#include "compiler.h"
#include "memory.h"
#include "vm.h"

#define GC_HEAP_GROW_FACTOR 2

void *reallocate(void *pointer, size_t old_size, size_t new_size) {
    vm.bytes_allocated += new_size - old_size;

    if (new_size > old_size) {
        if (vm.bytes_allocated > vm.next_gc) {
            collect_garbage();
        }
    }

    if (new_size == 0) {
        free(pointer);
        return NULL;
    }

    void *result = realloc(pointer, new_size);
    if (result == NULL)
        exit(1);
    return result;
}

static void free_object(Obj *object) {
    switch (object->type) {
        case OBJ_NIL:
            {
                FREE(ObjNil, object);
                break;
            }
        case OBJ_FLOAT:
            {
                FREE(ObjFloat, object);
                break;
            }
        case OBJ_INT:
            {
                FREE(ObjInt, object);
                break;
            }
        case OBJ_BOOL:
            {
                FREE(ObjBool, object);
                break;
            }
        case OBJ_MAP:
            {
                ObjMap *map = AS_MAP(object);
                free_table(&map->table);
                FREE(ObjMap, object);
                break;
            }
        case OBJ_LIST:
            {
                ObjList *list = AS_LIST(object);
                free_array(&list->elems);
                FREE(ObjList, object);
                break;
            }
        case OBJ_BOUND_METHOD:
            {
                FREE(ObjBoundMethod, object);
                break;
            }
        case OBJ_CLASS:
            {
                ObjClass *klass = AS_CLASS(object);
                free_table(&klass->methods);
                free_table(&klass->statics);
                FREE(ObjClass, object);
                break;
            }
        case OBJ_CLOSURE:
            {
                ObjClosure *closure = AS_CLOSURE(object);
                FREE_ARRAY(ObjUpvalue *, closure->upvalues, closure->upvalue_count);
                FREE(ObjClosure, object);
                break;
            }
        case OBJ_STRING:
            {
                ObjString *string = AS_STRING(object);
                FREE_ARRAY(char, string->chars, string->length + 1);
                FREE(ObjString, object);
                break;
            }
        case OBJ_FUNCTION:
            {
                ObjFunction *function = AS_FUNCTION(object);
                free_chunk(&function->chunk);
                FREE(ObjFunction, object);
                break;
            }
        case OBJ_INSTANCE:
            {
                ObjInstance *instance = AS_INSTANCE(object);
                free_table(&instance->fields);
                FREE(ObjInstance, object);
                break;
            }
        case OBJ_BUILTIN_BOUND_METHOD:
            break;
        case OBJ_UPVALUE:
            {
                FREE(ObjUpvalue, object);
                break;
            }
        case OBJ_BUILTIN_FUNCTION:
            {
                FREE(ObjBuiltinFunction, object);
                break;
            }
    }
}

void free_objects() {
    Obj *object = vm.objects;
    while (object != NULL) {
        Obj *next = object->next;
        free_object(object);
        object = next;
    }

    free(vm.gray_stack);
}

void mark_object(Obj *object) {
    if (object == NULL)
        return;

    if (object->is_marked)
        return;

    object->is_marked = true;

    if (vm.gray_capacity < vm.gray_count + 1) {
        vm.gray_capacity = GROW_CAPACITY(vm.gray_capacity);
        vm.gray_stack = (Obj **)realloc(vm.gray_stack, sizeof(Obj *) * vm.gray_capacity);

        if (vm.gray_stack == NULL)
            exit(1);
    }

    vm.gray_stack[vm.gray_count++] = object;
}

static void mark_array(Array *array) {
    for (int i = 0; i < array->count; i++) {
        mark_object(array->values[i]);
    }
}

static void blacken_object(Obj *object) {
    switch (object->type) {
        case OBJ_MAP:
            {
                ObjMap *map = AS_MAP(object);
                mark_table(&map->table);
                break;
            }
        case OBJ_LIST:
            {
                ObjList *list = AS_LIST(object);
                mark_array(&list->elems);
                break;
            }
        case OBJ_BOUND_METHOD:
            {
                ObjBoundMethod *bound = AS_BOUND_METHOD(object);
                mark_object(bound->receiver);
                mark_object((Obj *)bound->method);
                break;
            }
        case OBJ_CLASS:
            {
                ObjClass *klass = AS_CLASS(object);
                mark_object((Obj *)klass->name);
                mark_table(&klass->methods);
                mark_table(&klass->statics);
                break;
            }
        case OBJ_CLOSURE:
            {
                ObjClosure *closure = AS_CLOSURE(object);
                mark_object((Obj *)closure->function);
                for (int i = 0; i < closure->upvalue_count; i++) {
                    mark_object((Obj *)closure->upvalues[i]);
                }
                break;
            }
        case OBJ_FUNCTION:
            {
                ObjFunction *function = AS_FUNCTION(object);
                mark_object((Obj *)function->name);
                mark_array(&function->chunk.constants);
                break;
            }
        case OBJ_INSTANCE:
            {
                ObjInstance *instance = AS_INSTANCE(object);
                mark_object((Obj *)instance->klass);
                mark_table(&instance->fields);
                break;
            }
        case OBJ_UPVALUE:
            mark_object(AS_UPVALUE(object)->closed);
            break;
        case OBJ_BUILTIN_BOUND_METHOD:
        case OBJ_BUILTIN_FUNCTION:
        case OBJ_STRING:
        case OBJ_BOOL:
        case OBJ_INT:
        case OBJ_NIL:
        case OBJ_FLOAT:
            break;
    }
}

static void mark_roots() {
    for (Obj **slot = vm.stack; slot < vm.stack_top; slot++) {
        mark_object(*slot);
    }

    for (int i = 0; i < vm.frame_count; i++) {
        mark_object((Obj *)vm.frames[i].closure);
    }

    for (ObjUpvalue *upvalue = vm.open_upvalues; upvalue != NULL; upvalue = upvalue->next) {
        mark_object((Obj *)upvalue);
    }

    mark_table(&vm.globals);
    mark_compiler_roots();
    mark_object((Obj *)vm.init_string);
}

static void trace_references() {
    while (vm.gray_count > 0) {
        Obj *object = vm.gray_stack[--vm.gray_count];
        blacken_object(object);
    }
}

void sweep() {
    Obj *previous = NULL;
    Obj *object = vm.objects;
    while (object != NULL) {
        if (object->is_marked) {
            object->is_marked = false;
            previous = object;
            object = object->next;
        } else {
            Obj *unreached = object;
            object = object->next;
            if (previous != NULL) {
                previous->next = object;
            } else {
                vm.objects = object;
            }

            free_object(unreached);
        }
    }
}

void collect_garbage() {
    mark_roots();
    trace_references();
    table_remove_white(&vm.strings);
    sweep();

    vm.next_gc = vm.bytes_allocated * GC_HEAP_GROW_FACTOR;
}