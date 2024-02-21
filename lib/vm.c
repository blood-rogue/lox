#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "builtins.h"
#include "common.h"
#include "compiler.h"
#include "memory.h"
#include "object.h"
#include "vm.h"

VM vm;

static void reset_stack() {
    vm.stack_top = vm.stack;
    vm.frame_count = 0;
    vm.open_upvalues = NULL;
}

void push(Obj *value) {
    *vm.stack_top = value;
    vm.stack_top++;
}

Obj *pop() {
    vm.stack_top--;
    return *vm.stack_top;
}

static Obj *peek(int distance) { return vm.stack_top[-1 - distance]; }

static void define_builtin_function(const char *name, BuiltinFn function) {
    push(OBJ_VAL(new_string(name, (int)strlen(name))));
    push(OBJ_VAL(new_builtin_function(function)));
    table_set(&vm.globals, vm.stack[0], vm.stack[1]);
    pop();
    pop();
}

static void define_builtin_class(const char *name, ObjBuiltinClass *klass) {
    push(OBJ_VAL(new_string(name, (int)strlen(name))));
    push(OBJ_VAL(klass));
    table_set(&vm.globals, vm.stack[0], vm.stack[1]);
    pop();
    pop();
}

static void runtime_error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    for (int i = vm.frame_count - 1; i >= 0; i--) {
        CallFrame *frame = &vm.frames[i];
        ObjFunction *function = frame->closure->function;
        size_t instruction = frame->ip - function->chunk.code - 1;
        fprintf(stderr, "[line %d] in ", function->chunk.lines[instruction]);
        if (function->name == NULL) {
            fprintf(stderr, "script\n");
        } else {
            fprintf(stderr, "%s()\n", function->name->chars);
        }
    }

    reset_stack();
}

void init_vm() {
    reset_stack();
    vm.objects = NULL;
    vm.bytes_allocated = 0;
    vm.next_gc = 1024 * 1024;

    vm.gray_count = 0;
    vm.gray_capacity = 0;
    vm.gray_stack = NULL;

    init_table(&vm.globals);
    init_table(&vm.strings);

    vm.init_string = NULL;
    vm.init_string = new_string("init", 4);

    define_builtin_function("clock", clock_builtin_function);
    define_builtin_function("exit", exit_builtin_function);
    define_builtin_function("print", print_builtin_function);
    define_builtin_function("input", input_builtin_function);
    define_builtin_function("len", len_builtin_function);
    define_builtin_function("argv", argv_builtin_function);

    define_builtin_class("int", int_builtin_class());
    define_builtin_class("float", float_builtin_class());
}

void free_vm() {
    free_table(&vm.globals);
    free_table(&vm.strings);

    vm.init_string = NULL;

    free_objects();
}

static bool call(ObjClosure *closure, int arg_count) {
    if (arg_count != closure->function->arity) {
        runtime_error("Expected %d arguments but got %d.",
                      closure->function->arity, arg_count);
        return false;
    }

    if (vm.frame_count == FRAMES_MAX) {
        runtime_error("Stack overflow.");
        return false;
    }

    CallFrame *frame = &vm.frames[vm.frame_count++];
    frame->closure = closure;
    frame->ip = closure->function->chunk.code;
    frame->slots = vm.stack_top - arg_count - 1;
    return true;
}

static bool call_value(Obj *callee, int arg_count) {
    switch (callee->type) {
    case OBJ_BOUND_METHOD: {
        ObjBoundMethod *bound = AS_BOUND_METHOD(callee);
        vm.stack_top[-arg_count - 1] = bound->receiver;
        return call(bound->method, arg_count);
    }
    case OBJ_CLASS: {
        ObjClass *klass = AS_CLASS(callee);
        vm.stack_top[-arg_count - 1] = OBJ_VAL(new_instance(klass));

        Obj *initializer;
        if (table_get(&klass->methods, (Obj *)vm.init_string, &initializer)) {
            return call(AS_CLOSURE(initializer), arg_count);
        } else if (arg_count != 0) {
            runtime_error("Expected 0 arguments but got %d.", arg_count);
            return false;
        }

        return true;
    }
    case OBJ_CLOSURE:
        return call(AS_CLOSURE(callee), arg_count);
    case OBJ_BUILTIN_FUNCTION: {
        BuiltinFn builtin = AS_BUILTIN_FUNCTION(callee)->function;
        BuiltinResult result = builtin(arg_count, vm.stack_top - arg_count);

        if (result.error != NULL) {
            runtime_error(result.error);
            return false;
        }

        vm.stack_top -= arg_count + 1;
        push(result.value);
        return true;
    }
    case OBJ_BUILTIN_CLASS: {
        runtime_error("Cannot create builtin classes");
        return false;
    }
    default:
        runtime_error("Can only call functions and classes.");
        return false;
    }
}

static bool invoke_from_class(ObjClass *klass, ObjString *name, int arg_count) {
    Obj *method;
    if (!table_get(&klass->methods, (Obj *)name, &method) &&
        !table_get(&klass->statics, (Obj *)name, &method)) {
        runtime_error("Undefined property '%s'.", name->chars);
        return false;
    }

    return call(AS_CLOSURE(method), arg_count);
}

static bool invoke(ObjString *name, int arg_count) {
    Obj *receiver = peek(arg_count);

    if (IS_INSTANCE(receiver)) {
        ObjInstance *instance = AS_INSTANCE(receiver);

        Obj *value;
        if (table_get(&instance->fields, (Obj *)name, &value)) {
            vm.stack_top[-arg_count - 1] = value;
            return call_value(value, arg_count);
        }

        return invoke_from_class(instance->klass, name, arg_count);
    } else if (IS_CLASS(receiver)) {
        ObjClass *klass = AS_CLASS(receiver);

        Obj *method;
        if (!table_get(&klass->statics, (Obj *)name, &method)) {
            runtime_error("Undefined method '%s'.", name->chars);
            return false;
        }

        return call(AS_CLOSURE(method), arg_count);
    } else if (IS_BUILTIN_CLASS(receiver)) {
        ObjBuiltinClass *klass = AS_BUILTIN_CLASS(receiver);

        BuiltinFn method;
        if (!builtin_table_get(&klass->statics, name->hash, &method)) {
            runtime_error("Undefined method '%s'.", name->chars);
            return false;
        }

        BuiltinResult result = method(arg_count, vm.stack_top - arg_count);

        if (result.error != NULL) {
            runtime_error(result.error);
            return false;
        }

        vm.stack_top -= arg_count + 1;
        push(result.value);
        return true;
    }

    runtime_error("Could not invode method.");
    return false;
}

static bool bind_method(ObjClass *klass, ObjString *name) {
    Obj *method;
    if (!table_get(&klass->methods, (Obj *)name, &method)) {
        runtime_error("Undefined property '%s'.", name->chars);
        return false;
    }

    ObjBoundMethod *bound = new_bound_method(peek(0), AS_CLOSURE(method));
    pop();
    push(OBJ_VAL(bound));
    return true;
}

static ObjUpvalue *capture_upvalue(Obj **local) {
    ObjUpvalue *prev_upvalue = NULL;
    ObjUpvalue *upvalue = vm.open_upvalues;
    while (upvalue != NULL && upvalue->location > local) {
        prev_upvalue = upvalue;
        upvalue = upvalue->next;
    }

    if (upvalue != NULL && upvalue->location == local) {
        return upvalue;
    }

    ObjUpvalue *created_upvalue = new_upvalue(local);

    created_upvalue->next = upvalue;

    if (prev_upvalue == NULL) {
        vm.open_upvalues = created_upvalue;
    } else {
        prev_upvalue->next = created_upvalue;
    }

    return created_upvalue;
}

static void close_upvalues(Obj **last) {
    while (vm.open_upvalues != NULL && vm.open_upvalues->location >= last) {
        ObjUpvalue *upvalue = vm.open_upvalues;
        upvalue->closed = *upvalue->location;
        upvalue->location = &upvalue->closed;
        vm.open_upvalues = upvalue->next;
    }
}

static void define_method(ObjString *name, bool is_static) {
    Obj *method = peek(0);
    ObjClass *klass = AS_CLASS(peek(1));
    if (is_static)
        table_set(&klass->statics, (Obj *)name, method);
    else
        table_set(&klass->methods, (Obj *)name, method);

    pop();
}

static bool is_falsey(Obj *value) {
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static void concatenate() {
    ObjString *b = AS_STRING(peek(0));
    ObjString *a = AS_STRING(peek(1));

    int length = a->length + b->length;
    char *chars = ALLOCATE(char, length + 1);
    memcpy(chars, a->chars, a->length);
    memcpy(chars + a->length, b->chars, b->length);
    chars[length] = '\0';

    ObjString *result = take_string(chars, length);
    pop();
    pop();
    push(OBJ_VAL(result));
}

static InterpretResult run() {
    CallFrame *frame = &vm.frames[vm.frame_count - 1];

#define READ_BYTE() (*frame->ip++)
#define READ_SHORT()                                                           \
    (frame->ip += 2, (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))
#define READ_CONSTANT()                                                        \
    (frame->closure->function->chunk.constants.values[READ_BYTE()])
#define READ_STRING() AS_STRING(READ_CONSTANT())
#define BINARY_OP(new_func_int, new_func_float, op)                            \
    {                                                                          \
        if (IS_INT(peek(0)) && IS_INT(peek(1))) {                              \
            push(OBJ_VAL(                                                      \
                new_func_int(AS_INT(pop())->value op AS_INT(pop())->value)));  \
        } else if (IS_FLOAT(peek(0)) && IS_FLOAT(peek(1))) {                   \
            push(OBJ_VAL(new_func_float(                                       \
                AS_FLOAT(pop())->value op AS_FLOAT(pop())->value)));           \
        } else {                                                               \
            runtime_error("Operands must be numbers.");                        \
            return INTERPRET_RUNTIME_ERROR;                                    \
        }                                                                      \
    }

    for (;;) {
        switch (READ_BYTE()) {
        case OP_CONSTANT: {
            Obj *constant = READ_CONSTANT();
            push(constant);
            break;
        }
        case OP_LIST: {
            int elem_count = READ_BYTE();
            ObjList *list = new_list(vm.stack_top - elem_count, elem_count);
            vm.stack_top -= elem_count;
            push(OBJ_VAL(list));

            break;
        }
        case OP_MAP: {
            int pair_count = READ_BYTE();
            ObjMap *map = new_map(vm.stack_top - pair_count * 2, pair_count);
            vm.stack_top -= pair_count * 2;
            push(OBJ_VAL(map));

            break;
        }
        case OP_GET_INDEX: {
            Obj *index_value = pop();
            Obj *value = pop();

            Obj *indexed;
            if (IS_LIST(value)) {
                if (!IS_INT(index_value)) {
                    runtime_error("Lists can only be indexed using numbers.");
                    return INTERPRET_RUNTIME_ERROR;
                }

                int64_t index = AS_INT(index_value)->value;
                ObjList *list = AS_LIST(value);

                if (index >= list->elems.count) {
                    runtime_error("Index out of bounds.");
                    return INTERPRET_RUNTIME_ERROR;
                }

                indexed = list->elems.values[index];
            } else if (IS_MAP(value)) {
                if (!IS_STRING(index_value)) {
                    runtime_error("Maps can only be indexed using strings.");
                    return INTERPRET_RUNTIME_ERROR;
                }

                ObjString *index = AS_STRING(index_value);
                ObjMap *map = AS_MAP(value);

                if (!table_get(&map->table, (Obj *)index, &indexed)) {
                    runtime_error("Key not found.");
                    return INTERPRET_RUNTIME_ERROR;
                }
            } else {
                runtime_error("Only lists and maps can be indexed.");
                return INTERPRET_RUNTIME_ERROR;
            }

            push(indexed);
            break;
        }
        case OP_SET_INDEX: {
            Obj *to_be_assigned = peek(0);
            Obj *index_value = peek(1);
            Obj *value = peek(2);

            if (IS_LIST(value)) {
                if (!IS_INT(index_value)) {
                    runtime_error("Lists can only be indexed using numbers.");
                    return INTERPRET_RUNTIME_ERROR;
                }

                int64_t index = AS_INT(index_value)->value;
                ObjList *list = AS_LIST(value);

                if (index >= list->elems.count) {
                    runtime_error("Index out of bounds.");
                    return INTERPRET_RUNTIME_ERROR;
                }

                list->elems.values[index] = to_be_assigned;
            } else if (IS_MAP(value)) {
                if (!IS_STRING(index_value)) {
                    runtime_error("Maps can only be indexed using strings.");
                    return INTERPRET_RUNTIME_ERROR;
                }

                ObjString *index = AS_STRING(index_value);
                ObjMap *map = AS_MAP(value);

                table_set(&map->table, (Obj *)index, to_be_assigned);
            } else {
                runtime_error("Only lists and maps can be indexed.");
                return INTERPRET_RUNTIME_ERROR;
            }

            pop();
            pop();
            pop();

            push(value);

            break;
        }
        case OP_NIL: {
            push(OBJ_VAL(new_nil()));
            break;
        }
        case OP_TRUE: {
            push(OBJ_VAL(new_bool(true)));
            break;
        }
        case OP_FALSE: {
            push(OBJ_VAL(new_bool(false)));
            break;
        }
        case OP_POP: {
            pop();
            break;
        }
        case OP_GET_LOCAL: {
            uint8_t slot = READ_BYTE();
            push(frame->slots[slot]);
            break;
        }
        case OP_SET_LOCAL: {
            uint8_t slot = READ_BYTE();
            frame->slots[slot] = peek(0);
            break;
        }
        case OP_GET_GLOBAL: {
            ObjString *name = READ_STRING();
            Obj *value;
            if (!table_get(&vm.globals, (Obj *)name, &value)) {
                runtime_error("Undefined variable '%s'.", name->chars);
                return INTERPRET_RUNTIME_ERROR;
            }
            push(value);
            break;
        }
        case OP_DEFINE_GLOBAL: {
            ObjString *name = READ_STRING();
            table_set(&vm.globals, (Obj *)name, peek(0));
            pop();
            break;
        }
        case OP_SET_GLOBAL: {
            ObjString *name = READ_STRING();
            if (table_set(&vm.globals, (Obj *)name, peek(0))) {
                table_delete(&vm.globals, (Obj *)name);
                runtime_error("Undefined variable '%s'.", name->chars);
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OP_GET_UPVALUE: {
            uint8_t slot = READ_BYTE();
            push(*frame->closure->upvalues[slot]->location);
            break;
        }
        case OP_SET_UPVALUE: {
            uint8_t slot = READ_BYTE();
            *frame->closure->upvalues[slot]->location = peek(0);
            break;
        }
        case OP_GET_PROPERTY: {
            if (IS_INSTANCE(peek(0))) {
                ObjInstance *instance = AS_INSTANCE(peek(0));
                ObjString *name = READ_STRING();

                Obj *value;
                if (table_get(&instance->fields, (Obj *)name, &value)) {
                    pop();
                    push(value);
                    break;
                }

                if (!bind_method(instance->klass, name)) {
                    return INTERPRET_RUNTIME_ERROR;
                }
                break;
            }

            runtime_error("Only instances have properties.");
            return INTERPRET_RUNTIME_ERROR;
        }
        case OP_SET_PROPERTY: {
            if (IS_INSTANCE(peek(1))) {

                ObjInstance *instance = AS_INSTANCE(peek(1));
                table_set(&instance->fields, (Obj *)READ_STRING(), peek(0));
                Obj *value = pop();
                pop();
                push(value);
                break;
            }

            runtime_error("Only instances have properties.");
            return INTERPRET_RUNTIME_ERROR;
        }
        case OP_GET_SUPER: {
            ObjString *name = READ_STRING();
            ObjClass *superclass = AS_CLASS(pop());

            if (!bind_method(superclass, name)) {
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OP_EQUAL: {
            Obj *b = pop();
            Obj *a = pop();
            push(OBJ_VAL(new_bool(obj_equal(a, b))));
            break;
        }
        case OP_GREATER:
            BINARY_OP(new_bool, new_bool, >);
            break;
        case OP_LESS:
            BINARY_OP(new_bool, new_bool, <);
            break;
        case OP_ADD: {
            if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
                concatenate();
            } else if (IS_INT(peek(0)) && IS_INT(peek(1))) {
                push(OBJ_VAL(
                    new_int(AS_INT(pop())->value + AS_INT(pop())->value)));
            } else if (IS_FLOAT(peek(0)) && IS_FLOAT(peek(1))) {
                push(OBJ_VAL(new_float(AS_FLOAT(pop())->value +
                                       AS_FLOAT(pop())->value)));
            } else {
                runtime_error("Operands must be two numbers or two strings.");
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OP_SUBTRACT: {
            BINARY_OP(new_int, new_float, -);
            break;
        }
        case OP_MULTIPLY: {
            BINARY_OP(new_int, new_float, *);
            break;
        }
        case OP_DIVIDE: {
            BINARY_OP(new_int, new_float, /);
            break;
        }
        case OP_NOT: {
            push(OBJ_VAL(new_bool(is_falsey(pop()))));
            break;
        }
        case OP_NEGATE: {
            if (!IS_INT(peek(0))) {
                runtime_error("Operand must be a number.");
                return INTERPRET_RUNTIME_ERROR;
            }
            push(OBJ_VAL(new_int(-(AS_INT(pop())->value))));
            break;
        }
        case OP_JUMP: {
            uint16_t offset = READ_SHORT();
            frame->ip += offset;
            break;
        }
        case OP_JUMP_IF_FALSE: {
            uint16_t offset = READ_SHORT();
            if (is_falsey(peek(0)))
                frame->ip += offset;
            break;
        }
        case OP_LOOP: {
            uint16_t offset = READ_SHORT();
            frame->ip -= offset;
            break;
        }
        case OP_CALL: {
            int arg_count = READ_BYTE();
            if (!call_value(peek(arg_count), arg_count)) {
                return INTERPRET_RUNTIME_ERROR;
            }
            frame = &vm.frames[vm.frame_count - 1];
            break;
        }
        case OP_INVOKE: {
            ObjString *method = READ_STRING();
            int arg_count = READ_BYTE();
            if (!invoke(method, arg_count)) {
                return INTERPRET_RUNTIME_ERROR;
            }
            frame = &vm.frames[vm.frame_count - 1];
            break;
        }
        case OP_SUPER_INVOKE: {
            ObjString *method = READ_STRING();
            int arg_count = READ_BYTE();
            ObjClass *superclass = AS_CLASS(pop());
            if (!invoke_from_class(superclass, method, arg_count)) {
                return INTERPRET_RUNTIME_ERROR;
            }
            frame = &vm.frames[vm.frame_count - 1];
            break;
        }
        case OP_CLOSURE: {
            ObjFunction *function = AS_FUNCTION(READ_CONSTANT());
            ObjClosure *closure = new_closure(function);
            push(OBJ_VAL(closure));

            for (int i = 0; i < closure->upvalue_count; i++) {
                uint8_t is_local = READ_BYTE();
                uint8_t index = READ_BYTE();
                if (is_local) {
                    closure->upvalues[i] =
                        capture_upvalue(frame->slots + index);
                } else {
                    closure->upvalues[i] = frame->closure->upvalues[index];
                }
            }

            break;
        }
        case OP_CLOSE_UPVALUE:
            close_upvalues(vm.stack_top - 1);
            pop();
            break;
        case OP_RETURN: {
            Obj *result = pop();
            close_upvalues(frame->slots);
            vm.frame_count--;
            if (vm.frame_count == 0) {
                pop();
                return INTERPRET_OK;
            }

            vm.stack_top = frame->slots;
            push(result);
            frame = &vm.frames[vm.frame_count - 1];
            break;
        }
        case OP_CLASS:
            push(OBJ_VAL(new_class(READ_STRING())));
            break;
        case OP_INHERIT: {
            Obj *superclass = peek(1);

            if (!IS_CLASS(superclass)) {
                runtime_error("Superclass must be a class.");
                return INTERPRET_RUNTIME_ERROR;
            }

            ObjClass *subclass = AS_CLASS(peek(0));
            table_add_all(&AS_CLASS(superclass)->methods, &subclass->methods);
            pop();
            break;
        }
        case OP_METHOD:
            define_method(READ_STRING(), false);
            break;
        case OP_STATIC_METHOD:
            define_method(READ_STRING(), true);
            break;
        }
    }

#undef READ_BYTE
#undef READ_SHORT
#undef READ_CONSTANT
#undef READ_STRING
#undef BINARY_OP_OBJ
}

InterpretResult interpret(const char *source) {
    ObjFunction *function = compile(source);
    if (function == NULL)
        return INTERPRET_COMPILE_ERROR;

    push(OBJ_VAL(function));

    ObjClosure *closure = new_closure(function);
    pop();
    push(OBJ_VAL(closure));
    call(closure, 0);

    return run();
}