#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "builtins.h"
#include "common.h"
#include "compiler.h"
#include "memory.h"
#include "object.h"
#include "vm.h"

#ifdef DEBUG
#include "debug.h"
#endif // DEBUG

VM vm;
extern char *_source;

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

static char *read_file(char *path) {
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        runtime_error("Could not open file \"%s\".\n", path);
        return NULL;
    }

    fseek(file, 0L, SEEK_END);
    size_t file_size = (size_t)ftell(file);
    rewind(file);

    char *buffer = malloc(file_size + 1);
    if (buffer == NULL) {
        runtime_error("Not enough memory to read \"%s\".\n", path);
        return NULL;
    }

    size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
    if (bytes_read < file_size) {
        runtime_error("Could not read file \"%s\".\n", path);
        return NULL;
    }

    buffer[bytes_read] = '\0';

    fclose(file);
    return buffer;
}

void init_vm() {
    init_literals();

    reset_stack();
    vm.objects = NULL;
    vm.bytes_allocated = 0;
    vm.next_gc = 1024 * 1024;

    vm.gray_count = 0;
    vm.gray_capacity = 0;
    vm.gray_stack = NULL;

    init_table(&vm.globals);
    init_table(&vm.strings);
    init_table(&vm.modules);

    vm.current_module = NULL;
    vm.module_count = 0;

    vm.builtin_methods = get_builtin_methods();

    vm.init_string = NULL;
    vm.init_string = new_string("init", 4);

    init_method_table(&vm.builtin_functions, 16);

#define SET_BLTIN_FN(name)                                                                         \
    method_table_set(                                                                              \
        &vm.builtin_functions,                                                                     \
        #name,                                                                                     \
        hash_string(#name, (int)strlen(#name)),                                                    \
        name##_builtin_function)

    SET_BLTIN_FN(exit);
    SET_BLTIN_FN(print);
    SET_BLTIN_FN(input);
    SET_BLTIN_FN(argv);
    SET_BLTIN_FN(run_gc);
    SET_BLTIN_FN(parse_int);
    SET_BLTIN_FN(parse_float);
    SET_BLTIN_FN(sleep);
    SET_BLTIN_FN(type);
    SET_BLTIN_FN(repr);

#undef SET_BLTIN_FN
}

void free_vm() {
    free_table(&vm.globals);
    free_table(&vm.strings);
    free_table(&vm.modules);

    vm.init_string = NULL;

    if (vm.current_module != NULL) {
        free(vm.current_module);
    }

    vm.module_count = 0;

    free_literals();

    free_objects();
    free_method_table(&vm.builtin_functions);

    for (int i = 0; i < NUM_OBJS; i++) {
        if (vm.builtin_methods[i] != NULL) {
            free_method_table(vm.builtin_methods[i]);
            free(vm.builtin_methods[i]);
        }
    }

    free(vm.builtin_methods);

    if (_source != NULL)
        free(_source);
}

static Table *get_current_global() {
    if (vm.current_module == NULL) {
        return &vm.globals;
    }

    if (vm.current_module != NULL)
        return &vm.current_module->current->globals;

    runtime_error("Could not import module '%s'.", vm.current_module->current->name->chars);
    return NULL;
}

static bool is_std_import(ObjString *path) {
    return path->length > 6 && memcmp(path->chars, "@std/", 5) == 0;
}

static bool call(ObjClosure *closure, int argc) {
    if (argc != closure->function->arity) {
        runtime_error("Expected %d arguments but got %d.", closure->function->arity, argc);
        return false;
    }

    if (vm.frame_count == FRAMES_MAX) {
        runtime_error("Stack overflow.");
        return false;
    }

    CallFrame *frame = &vm.frames[vm.frame_count++];
    frame->closure = closure;
    frame->ip = closure->function->chunk.code;
    frame->slots = vm.stack_top - argc - 1;
    return true;
}

static bool call_value(Obj *callee, int argc, Obj *caller) {
    switch (callee->type) {
        case OBJ_BOUND_METHOD:
            {
                ObjBoundMethod *bound = AS_BOUND_METHOD(callee);
                vm.stack_top[-argc - 1] = bound->receiver;
                return call(bound->method, argc);
            }
        case OBJ_CLASS:
            {
                ObjClass *klass = AS_CLASS(callee);
                ObjInstance *instance = new_instance(klass);

                table_add_all(&klass->fields, &instance->fields);

                vm.stack_top[-argc - 1] = AS_OBJ(instance);

                Obj *initializer;
                if (table_get(&klass->methods, AS_OBJ(vm.init_string), &initializer)) {
                    switch (initializer->type) {
                        case OBJ_CLOSURE:
                            return call(AS_CLOSURE(initializer), argc);
                        case OBJ_BUILTIN_FUNCTION:
                            {
                                ObjBuiltinFunction *builtin = AS_BUILTIN_FUNCTION(initializer);
                                BuiltinResult result =
                                    builtin->method(argc, vm.stack_top - argc, AS_OBJ(instance));

                                if (result.error != NULL) {
                                    runtime_error(result.error);
                                    return false;
                                }

                                vm.stack_top -= argc;

                                return true;
                            }
                        default:
                            runtime_error("Invalid initializer.");
                            return false;
                    }

                } else if (argc != 0) {
                    runtime_error("Expected 0 arguments but got %d.", argc);
                    return false;
                }

                return true;
            }
        case OBJ_CLOSURE:
            return call(AS_CLOSURE(callee), argc);
        case OBJ_BUILTIN_FUNCTION:
            {
                BuiltinFn builtin = AS_BUILTIN_FUNCTION(callee)->method;
                BuiltinResult result = builtin(argc, vm.stack_top - argc, caller);

                if (result.error != NULL) {
                    runtime_error(result.error);
                    return false;
                }

                vm.stack_top -= argc + 1;
                push(result.value);
                return true;
            }
        case OBJ_BUILTIN_BOUND_METHOD:
            {
                ObjBuiltinBoundMethod *bound_method = AS_BUILTIN_BOUND_METHOD(callee);

                BuiltinResult result =
                    bound_method->function(argc, vm.stack_top - argc, bound_method->caller);

                if (result.error != NULL) {
                    runtime_error(result.error);
                    return false;
                }

                vm.stack_top -= argc + 1;
                push(result.value);
                return true;
            }
        default:
            runtime_error("Cannot call %s", OBJ_NAMES[callee->type]);
            return false;
    }
}

static bool invoke_from_class(ObjClass *klass, ObjString *name, int argc, ObjInstance *caller) {
    Obj *method;
    if (!table_get(&klass->methods, AS_OBJ(name), &method) &&
        !table_get(&klass->statics, AS_OBJ(name), &method) &&
        !table_get(&klass->fields, AS_OBJ(name), &method)) {
        runtime_error("Undefined property '%s' for class '%s'.", name->chars, klass->name->chars);
        return false;
    }

    if (!call_value(method, argc, AS_OBJ(caller))) {
        runtime_error(
            "Property '%s' of class '%s' is not callable.", name->chars, klass->name->chars);
        return false;
    }

    return true;
}

static bool invoke_scoped_member(ObjModule *module, ObjString *name, int argc) {
    Obj *member;
    if (table_get(&module->globals, AS_OBJ(name), &member)) {
        return call_value(member, argc, AS_OBJ(module));
    }

    runtime_error("No member named '%s' in module '%s'.", name->chars, module->name->chars);
    return false;
}

static bool invoke(ObjString *name, int argc) {
    Obj *receiver = peek(argc);

    switch (receiver->type) {
        case OBJ_INSTANCE:
            {
                ObjInstance *instance = AS_INSTANCE(receiver);

                Obj *value;
                if (table_get(&instance->fields, AS_OBJ(name), &value)) {
                    vm.stack_top[-argc - 1] = value;
                    return call_value(value, argc, AS_OBJ(instance));
                }

                return invoke_from_class(instance->klass, name, argc, instance);
            }
        case OBJ_CLASS:
            {
                ObjClass *klass = AS_CLASS(receiver);

                Obj *method;
                if (!table_get(&klass->statics, AS_OBJ(name), &method) &&
                    !table_get(&klass->fields, AS_OBJ(name), &method)) {
                    runtime_error(
                        "Undefined property '%s' for class '%s'.", name->chars, klass->name->chars);
                    return false;
                }

                if (!call_value(method, argc, AS_OBJ(klass))) {
                    runtime_error(
                        "Property '%s' for class '%s' is not callable.",
                        name->chars,
                        klass->name->chars);
                    return false;
                }

                return true;
            }
        default:
            {
                BuiltinFn method;
                if (vm.builtin_methods[receiver->type] == NULL ||
                    !method_table_get(
                        vm.builtin_methods[receiver->type], name->obj.hash, &method)) {
                    runtime_error(
                        "Could not invode method '%s' on '%s'.",
                        name->chars,
                        OBJ_NAMES[receiver->type]);
                    return false;
                }

                BuiltinResult result = method(argc, vm.stack_top - argc, receiver);

                if (result.error != NULL) {
                    runtime_error(result.error);
                    return false;
                }

                vm.stack_top -= argc + 1;
                push(result.value);
                return true;
            }
    }
}

static bool bind_method(ObjClass *klass, ObjString *name) {
    Obj *method;
    if (!table_get(&klass->methods, AS_OBJ(name), &method)) {
        runtime_error("Undefined property '%s' of class '%s'.", name->chars, klass->name->chars);
        return false;
    }

    ObjBoundMethod *bound = new_bound_method(peek(0), AS_CLOSURE(method));
    pop();
    push(AS_OBJ(bound));
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
        table_set(&klass->statics, AS_OBJ(name), method);
    else
        table_set(&klass->methods, AS_OBJ(name), method);

    pop();
}

static bool is_falsey(Obj *value) {
    return IS_NIL(value) || (IS_BOOL(value) && !(AS_BOOL(value)->value));
}

static InterpretResult run() {
    CallFrame *frame = &vm.frames[vm.frame_count - 1];

#define READ_BYTE()     (*frame->ip++)
#define READ_SHORT()    (frame->ip += 2, (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))
#define READ_CONSTANT() (frame->closure->function->chunk.constants.values[READ_BYTE()])
#define READ_STRING()   AS_STRING(READ_CONSTANT())
#define BINARY_OP(new_func_int, new_func_float, op)                                                \
    {                                                                                              \
        if (IS_INT(peek(0)) && IS_INT(peek(1))) {                                                  \
            push(AS_OBJ(new_func_int(AS_INT(pop())->value op AS_INT(pop())->value)));              \
        } else if (IS_FLOAT(peek(0)) && IS_FLOAT(peek(1))) {                                       \
            push(AS_OBJ(new_func_float(AS_FLOAT(pop())->value op AS_FLOAT(pop())->value)));        \
        } else {                                                                                   \
            runtime_error(                                                                         \
                "Unsupported operand types for '%s': '%s' and '%s'.",                              \
                #op,                                                                               \
                OBJ_NAMES[peek(0)->type],                                                          \
                OBJ_NAMES[peek(1)->type]);                                                         \
            return INTERPRET_RUNTIME_ERROR;                                                        \
        }                                                                                          \
    }
#define BINARY_INT_OP(new_func, op)                                                                \
    {                                                                                              \
        if (IS_INT(peek(0)) && IS_INT(peek(1))) {                                                  \
            push(AS_OBJ(new_func(AS_INT(pop())->value op AS_INT(pop())->value)));                  \
        } else {                                                                                   \
            runtime_error(                                                                         \
                "Unsupported operand types for '%s': '%s' and '%s'.",                              \
                #op,                                                                               \
                OBJ_NAMES[peek(0)->type],                                                          \
                OBJ_NAMES[peek(1)->type]);                                                         \
            return INTERPRET_RUNTIME_ERROR;                                                        \
        }                                                                                          \
    }

    for (;;) {
#ifdef DEBUG
        printf("          ");
        for (Obj **slot = vm.stack; slot < vm.stack_top; slot++) {
            printf("[ ");
            repr_object(*slot);
            printf(" ]");
        }
        printf("\n");
        disassemble_instruction(
            &frame->closure->function->chunk,
            (int)(frame->ip - frame->closure->function->chunk.code));
#endif // DEBUG

        switch ((OpCode)READ_BYTE()) {
            case OP_CONSTANT:
                {
                    Obj *constant = READ_CONSTANT();
                    push(constant);
                    break;
                }
            case OP_LIST:
                {
                    int elem_count = READ_BYTE();
                    ObjList *list = new_list(vm.stack_top - elem_count, elem_count);
                    vm.stack_top -= elem_count;
                    push(AS_OBJ(list));

                    break;
                }
            case OP_MAP:
                {
                    int pair_count = READ_BYTE();
                    ObjMap *map = new_map(vm.stack_top - pair_count * 2, pair_count);
                    vm.stack_top -= pair_count * 2;
                    push(AS_OBJ(map));

                    break;
                }
            case OP_GET_INDEX:
                {
                    Obj *index_value = pop();
                    Obj *value = pop();

                    Obj *indexed;
                    switch (value->type) {
                        case OBJ_LIST:
                            {
                                if (!IS_INT(index_value)) {
                                    runtime_error("Lists can only be indexed "
                                                  "using integers.");
                                    return INTERPRET_RUNTIME_ERROR;
                                }

                                int64_t index = AS_INT(index_value)->value;
                                ObjList *list = AS_LIST(value);

                                if (index < 0)
                                    index = list->elems.count + index;

                                if (index >= list->elems.count) {
                                    runtime_error("Index out of bounds.");
                                    return INTERPRET_RUNTIME_ERROR;
                                }

                                indexed = list->elems.values[index];
                                break;
                            }
                        case OBJ_MAP:
                            {
                                if (!IS_STRING(index_value)) {
                                    runtime_error("Maps can only be indexed "
                                                  "using strings.");
                                    return INTERPRET_RUNTIME_ERROR;
                                }

                                ObjString *index = AS_STRING(index_value);
                                ObjMap *map = AS_MAP(value);

                                if (!table_get(&map->table, AS_OBJ(index), &indexed)) {
                                    runtime_error("Key not found.");
                                    return INTERPRET_RUNTIME_ERROR;
                                }
                                break;
                            }
                        case OBJ_STRING:
                            {
                                if (!IS_INT(index_value)) {
                                    runtime_error("Strings can only be indexed "
                                                  "using integers.");
                                    return INTERPRET_RUNTIME_ERROR;
                                }

                                int64_t index = AS_INT(index_value)->value;
                                ObjString *string = AS_STRING(value);

                                if (index < 0)
                                    index = string->length + index;

                                if (index >= string->length) {
                                    runtime_error("Index out of bounds.");
                                    return INTERPRET_RUNTIME_ERROR;
                                }

                                indexed = AS_OBJ(new_char(string->chars[index]));
                                break;
                            }
                        default:
                            {
                                runtime_error("'%s' cannot be indexed.", OBJ_NAMES[value->type]);
                                return INTERPRET_RUNTIME_ERROR;
                            }
                    }

                    push(indexed);
                    break;
                }
            case OP_SET_INDEX:
                {
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

                        table_set(&map->table, AS_OBJ(index), to_be_assigned);
                    } else {
                        runtime_error("'%s' cannot be indexed.", OBJ_NAMES[value->type]);
                        return INTERPRET_RUNTIME_ERROR;
                    }

                    pop();
                    pop();
                    pop();

                    push(value);

                    break;
                }
            case OP_NIL:
                {
                    push(AS_OBJ(new_nil()));
                    break;
                }
            case OP_TRUE:
                {
                    push(AS_OBJ(new_bool(true)));
                    break;
                }
            case OP_FALSE:
                {
                    push(AS_OBJ(new_bool(false)));
                    break;
                }
            case OP_POP:
                {
                    pop();
                    break;
                }
            case OP_GET_LOCAL:
                {
                    uint8_t slot = READ_BYTE();
                    push(frame->slots[slot]);
                    break;
                }
            case OP_SET_LOCAL:
                {
                    uint8_t slot = READ_BYTE();
                    frame->slots[slot] = peek(0);
                    break;
                }
            case OP_GET_GLOBAL:
                {
                    ObjString *name = READ_STRING();
                    Obj *value;
                    BuiltinFn fn;
                    Table *globals = get_current_global();

                    if (globals == NULL) {
                        return INTERPRET_RUNTIME_ERROR;
                    }

                    if (table_get(globals, AS_OBJ(name), &value)) {
                        push(value);
                        break;
                    } else if (method_table_get(&vm.builtin_functions, name->obj.hash, &fn)) {
                        push(AS_OBJ(new_builtin_function(fn, name->chars)));
                        break;
                    }
                    runtime_error("Undefined variable '%s'.", name->chars);
                    return INTERPRET_RUNTIME_ERROR;
                }
            case OP_DEFINE_GLOBAL:
                {
                    ObjString *name = READ_STRING();
                    Table *globals = get_current_global();

                    if (globals == NULL) {
                        return INTERPRET_RUNTIME_ERROR;
                    }

                    table_set(globals, AS_OBJ(name), peek(0));
                    pop();
                    break;
                }
            case OP_SET_GLOBAL:
                {
                    ObjString *name = READ_STRING();
                    Table *globals = get_current_global();

                    if (globals == NULL) {
                        return INTERPRET_RUNTIME_ERROR;
                    }

                    if (table_set(globals, AS_OBJ(name), peek(0))) {
                        table_delete(globals, AS_OBJ(name));
                        runtime_error("Undefined variable '%s'.", name->chars);
                        return INTERPRET_RUNTIME_ERROR;
                    }
                    break;
                }
            case OP_GET_UPVALUE:
                {
                    uint8_t slot = READ_BYTE();
                    push(*frame->closure->upvalues[slot]->location);
                    break;
                }
            case OP_SET_UPVALUE:
                {
                    uint8_t slot = READ_BYTE();
                    *frame->closure->upvalues[slot]->location = peek(0);
                    break;
                }
            case OP_GET_PROPERTY:
                {
                    Obj *obj = peek(0);
                    ObjString *name = READ_STRING();
                    switch (obj->type) {
                        case OBJ_INSTANCE:
                            {
                                ObjInstance *instance = AS_INSTANCE(obj);

                                Obj *value;
                                if (table_get(&instance->fields, AS_OBJ(name), &value)) {
                                    pop();
                                    push(value);
                                    break;
                                }

                                if (!bind_method(instance->klass, name)) {
                                    runtime_error(
                                        "Could not bind '%s' from class '%s'.",
                                        name->chars,
                                        instance->klass->name->chars);
                                    return INTERPRET_RUNTIME_ERROR;
                                }
                                break;
                            }
                        case OBJ_CLASS:
                            {
                                ObjClass *klass = AS_CLASS(obj);

                                Obj *value;
                                if (table_get(&klass->statics, AS_OBJ(name), &value) ||
                                    table_get(&klass->fields, AS_OBJ(name), &value)) {
                                    pop();
                                    push(value);
                                    break;
                                } else {
                                    runtime_error(
                                        "No static method '%s' on class '%s'.",
                                        name->chars,
                                        klass->name->chars);
                                    return INTERPRET_RUNTIME_ERROR;
                                }
                                break;
                            }
                        default:
                            if (vm.builtin_methods[obj->type] != NULL) {
                                BuiltinFn method;
                                if (method_table_get(
                                        vm.builtin_methods[obj->type], name->obj.hash, &method)) {
                                    pop();
                                    push(
                                        AS_OBJ(new_builtin_bound_method(method, obj, name->chars)));
                                } else {
                                    runtime_error(
                                        "No method named '%s' on '%s'",
                                        name->chars,
                                        OBJ_NAMES[obj->type]);
                                    return INTERPRET_RUNTIME_ERROR;
                                }
                            } else {
                                runtime_error(
                                    "Properties and methods do not exist for "
                                    "'%s'.",
                                    OBJ_NAMES[obj->type]);
                                return INTERPRET_RUNTIME_ERROR;
                            }
                    }
                    break;
                }
            case OP_SET_PROPERTY:
                {
                    if (IS_INSTANCE(peek(1))) {
                        ObjInstance *instance = AS_INSTANCE(peek(1));

                        if (instance->klass->is_builtin) {
                            runtime_error(
                                "Cannot set property of instance of builtin class '%s'.",
                                instance->klass->name->chars);
                            return INTERPRET_RUNTIME_ERROR;
                        }

                        table_set(&instance->fields, AS_OBJ(READ_STRING()), peek(0));

                        Obj *value = pop();
                        pop();

                        push(value);
                        break;
                    } else if (IS_CLASS(peek(1))) {
                        ObjClass *klass = AS_CLASS(peek(1));

                        if (klass->is_builtin) {
                            runtime_error(
                                "Cannot set property of builtin class '%s'.", klass->name->chars);
                            return INTERPRET_RUNTIME_ERROR;
                        }

                        table_set(&klass->fields, AS_OBJ(READ_STRING()), peek(0));

                        pop();
                        break;
                    }

                    runtime_error("Only instances and classes have properties.");
                    return INTERPRET_RUNTIME_ERROR;
                }
            case OP_GET_SUPER:
                {
                    ObjString *name = READ_STRING();
                    ObjClass *superclass = AS_CLASS(pop());

                    if (!bind_method(superclass, name)) {
                        return INTERPRET_RUNTIME_ERROR;
                    }
                    break;
                }
            case OP_GET_SCOPED:
                {
                    if (!IS_MODULE(peek(0))) {
                        runtime_error("Cannot scope %s.", OBJ_NAMES[peek(0)->type]);
                        return INTERPRET_RUNTIME_ERROR;
                    }

                    ObjModule *module = AS_MODULE(peek(0));
                    ObjString *name = READ_STRING();

                    Obj *member;
                    if (table_get(&module->globals, AS_OBJ(name), &member)) {
                        pop();
                        push(member);
                        break;
                    }

                    runtime_error(
                        "No method named '%s' found in module '%s'.",
                        name->chars,
                        module->name->chars);
                    return INTERPRET_RUNTIME_ERROR;
                }
            case OP_EQUAL:
                push(AS_OBJ(new_bool(obj_equal(pop(), pop()))));
                break;
            case OP_GREATER:
                BINARY_OP(new_bool, new_bool, <);
                break;
            case OP_LESS:
                BINARY_OP(new_bool, new_bool, >);
                break;
            case OP_ADD:
                {
                    if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
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
                        push(AS_OBJ(result));
                    } else if (IS_INT(peek(0)) && IS_INT(peek(1))) {
                        push(AS_OBJ(new_int(AS_INT(pop())->value + AS_INT(pop())->value)));
                    } else if (IS_FLOAT(peek(0)) && IS_FLOAT(peek(1))) {
                        push(AS_OBJ(new_float(AS_FLOAT(pop())->value + AS_FLOAT(pop())->value)));
                    } else {
                        runtime_error(
                            "Unsupported operand types for '+', '%s' and '%s'.",
                            OBJ_NAMES[peek(0)->type],
                            OBJ_NAMES[peek(1)->type]);
                        return INTERPRET_RUNTIME_ERROR;
                    }
                    break;
                }
            case OP_SUBTRACT:
                {
                    BINARY_OP(new_int, new_float, -);
                    break;
                }
            case OP_BINARY_AND:
                {
                    BINARY_INT_OP(new_int, &);
                    break;
                }
            case OP_BINARY_OR:
                {
                    BINARY_INT_OP(new_int, |);
                    break;
                }
            case OP_BINARY_XOR:
                {
                    BINARY_INT_OP(new_int, ^);
                    break;
                }
            case OP_MULTIPLY:
                {
                    BINARY_OP(new_int, new_float, *);
                    break;
                }
            case OP_DIVIDE:
                {
                    BINARY_OP(new_int, new_float, /);
                    break;
                }
            case OP_NOT:
                {
                    vm.stack_top[-1] = AS_OBJ(new_bool(is_falsey(peek(0))));
                    break;
                }
            case OP_NEGATE:
                {
                    if (IS_INT(peek(0))) {
                        vm.stack_top[-1] = AS_OBJ(new_int(-(AS_INT(peek(0))->value)));
                        break;
                    } else if (IS_FLOAT(peek(0))) {
                        vm.stack_top[-1] = AS_OBJ(new_float(-(AS_FLOAT(peek(0))->value)));
                        break;
                    }
                    runtime_error("Cannot negate '%s'.", OBJ_NAMES[peek(0)->type]);
                    return INTERPRET_RUNTIME_ERROR;
                }
            case OP_JUMP:
                {
                    uint16_t offset = READ_SHORT();
                    frame->ip += offset;
                    break;
                }
            case OP_JUMP_IF_FALSE:
                {
                    uint16_t offset = READ_SHORT();
                    if (is_falsey(peek(0)))
                        frame->ip += offset;
                    break;
                }
            case OP_LOOP:
                {
                    uint16_t offset = READ_SHORT();
                    frame->ip -= offset;
                    break;
                }
            case OP_CALL:
                {
                    int argc = READ_BYTE();
                    if (!call_value(peek(argc), argc, NULL)) {
                        return INTERPRET_RUNTIME_ERROR;
                    }
                    frame = &vm.frames[vm.frame_count - 1];
                    break;
                }
            case OP_METHOD_INVOKE:
                {
                    ObjString *method = READ_STRING();
                    int argc = READ_BYTE();
                    if (!invoke(method, argc)) {
                        return INTERPRET_RUNTIME_ERROR;
                    }
                    frame = &vm.frames[vm.frame_count - 1];
                    break;
                }
            case OP_SUPER_INVOKE:
                {
                    ObjString *method = READ_STRING();
                    int argc = READ_BYTE();
                    ObjClass *superclass = AS_CLASS(pop());
                    if (!invoke_from_class(superclass, method, argc, NULL)) {
                        return INTERPRET_RUNTIME_ERROR;
                    }
                    frame = &vm.frames[vm.frame_count - 1];
                    break;
                }
            case OP_SCOPE_INVOKE:
                {
                    ObjString *method = READ_STRING();
                    int argc = READ_BYTE();

                    if (!invoke_scoped_member(AS_MODULE(peek(argc)), method, argc))
                        return INTERPRET_RUNTIME_ERROR;

                    frame = &vm.frames[vm.frame_count - 1];
                    break;
                }
            case OP_CLOSURE:
                {
                    ObjFunction *function = AS_FUNCTION(READ_CONSTANT());
                    ObjClosure *closure = new_closure(function);
                    push(AS_OBJ(closure));

                    for (int i = 0; i < closure->upvalue_count; i++) {
                        uint8_t is_local = READ_BYTE();
                        uint8_t index = READ_BYTE();
                        if (is_local) {
                            closure->upvalues[i] = capture_upvalue(frame->slots + index);
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
            case OP_RETURN:
                {
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
            case OP_END:
                {
                    close_upvalues(frame->slots);
                    vm.frame_count--;
                    if (vm.frame_count == 0) {
                        return INTERPRET_OK;
                    }

                    if (vm.current_module != NULL) {
                        push(AS_OBJ(vm.current_module->current));
                        table_set(
                            &vm.modules,
                            AS_OBJ(vm.current_module->current->name),
                            AS_OBJ(vm.current_module->current));

                        Module *prev = vm.current_module->prev;
                        free(vm.current_module);

                        vm.current_module = prev;
                        vm.module_count--;
                    }

                    frame = &vm.frames[vm.frame_count - 1];

                    break;
                }
            case OP_CLASS:
                push(AS_OBJ(new_class(READ_STRING())));
                break;
            case OP_INHERIT:
                {
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
            case OP_IMPORT:
                {
                    if (vm.module_count == UINT8_MAX) {
                        runtime_error("Exceeded max import limit.");
                        return INTERPRET_RUNTIME_ERROR;
                    }

                    ObjString *import_path = READ_STRING();

                    Obj *module;
                    if (table_get(&vm.modules, AS_OBJ(import_path), &module)) {
                        push(module);
                        break;
                    }

                    if (is_std_import(import_path)) {
                        char *temp = malloc(import_path->length + 1);
                        strcpy(temp, import_path->chars);

                        strtok(temp, "/");

                        ObjModule *std_module = get_module(strtok(NULL, "/"));

                        free(temp);

                        if (std_module == NULL) {
                            runtime_error("No standard module named '%s'.", import_path->chars);
                            return INTERPRET_RUNTIME_ERROR;
                        } else
                            push(AS_OBJ(std_module));
                    } else {
                        char *source = read_file(import_path->chars);

                        ObjFunction *module_function = compile(source);
                        if (module_function == NULL) {
                            return INTERPRET_COMPILE_ERROR;
                        }

                        push(AS_OBJ(module_function));

                        ObjClosure *closure = new_closure(module_function);
                        pop();
                        call(closure, 0);

                        frame = &vm.frames[vm.frame_count - 1];

                        Module *module = malloc(sizeof(Module));

                        module->prev = vm.current_module;
                        module->current = new_module(import_path);

                        vm.current_module = module;
                        vm.module_count++;

                        free(source);
                    }
                    break;
                }
            case OP_BREAK:
                {
                    uint16_t offset = READ_SHORT();
                    frame->ip += offset;
                    break;
                }
            case OP_CONTINUE:
                {
                    uint16_t offset = READ_SHORT();
                    frame->ip -= offset;
                    break;
                }
        }
    }

#undef READ_BYTE
#undef READ_SHORT
#undef READ_CONSTANT
#undef READ_STRING
#undef BINARY_OP_OBJ
}

InterpretResult interpret(char *source) {
    ObjFunction *function = compile(source);
    if (function == NULL)
        return INTERPRET_COMPILE_ERROR;

    push(AS_OBJ(function));

    ObjClosure *closure = new_closure(function);
    pop();
    push(AS_OBJ(closure));
    call(closure, 0);

    return run();
}
