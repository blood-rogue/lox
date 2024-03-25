#include <dlfcn.h>
#include <readline/history.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistr.h>
#include <unitypes.h>

#include "common.h"
#include "compiler.h"
#include "memory.h"
#include "native.h"
#include "object.h"
#include "vm.h"

#ifdef DEBUG
#include "debug.h"
#endif // DEBUG

VM vm;
extern char *_source;

static ObjString *_init_string = NULL;
static ObjString *_new_string = NULL;
static ObjString *_add_string = NULL;
static ObjString *_sub_string = NULL;
static ObjString *_mul_string = NULL;
static ObjString *_div_string = NULL;
static ObjString *_eq_string = NULL;
static ObjString *_neg_string = NULL;
static ObjString *_not_string = NULL;
static ObjString *_lt_string = NULL;
static ObjString *_gt_string = NULL;
static ObjString *_bitshr_string = NULL;
static ObjString *_bitshl_string = NULL;
static ObjString *_bitand_string = NULL;
static ObjString *_bitor_string = NULL;
static ObjString *_bitxor_string = NULL;
static ObjString *_bitcomp_string = NULL;

static void init_method_names() {
    _init_string = new_string("__init", (int)strlen("__init"));
    _new_string = new_string("__new", (int)strlen("__new"));
    _add_string = new_string("__add", (int)strlen("__add"));
    _sub_string = new_string("__sub", (int)strlen("__sub"));
    _mul_string = new_string("__mul", (int)strlen("__mul"));
    _div_string = new_string("__div", (int)strlen("__div"));
    _eq_string = new_string("__eq", (int)strlen("__eq"));
    _neg_string = new_string("__neg", (int)strlen("__neg"));
    _not_string = new_string("__not", (int)strlen("__not"));
    _lt_string = new_string("__lt", (int)strlen("__lt"));
    _gt_string = new_string("__gt", (int)strlen("__gt"));
    _bitand_string = new_string("__bitand", (int)strlen("__bitand"));
    _bitor_string = new_string("__bitor", (int)strlen("__bitor"));
    _bitshr_string = new_string("__bitshr", (int)strlen("__bitshr"));
    _bitshl_string = new_string("__bitshl", (int)strlen("__bitshl"));
    _bitxor_string = new_string("__bitxor", (int)strlen("__bitxor"));
    _bitcomp_string = new_string("__bitcomp", (int)strlen("__bitcomp"));

    vm.method_names[0] = _init_string;
    vm.method_names[1] = _new_string;
    vm.method_names[2] = _add_string;
    vm.method_names[3] = _sub_string;
    vm.method_names[4] = _mul_string;
    vm.method_names[5] = _div_string;
    vm.method_names[6] = _eq_string;
    vm.method_names[7] = _neg_string;
    vm.method_names[8] = _not_string;
    vm.method_names[9] = _lt_string;
    vm.method_names[10] = _gt_string;
    vm.method_names[11] = _bitshr_string;
    vm.method_names[12] = _bitshl_string;
    vm.method_names[13] = _bitand_string;
    vm.method_names[14] = _bitor_string;
    vm.method_names[15] = _bitxor_string;
    vm.method_names[16] = _bitcomp_string;
}

static void free_method_names() {
    _init_string = NULL;
    _new_string = NULL;
    _add_string = NULL;
    _sub_string = NULL;
    _mul_string = NULL;
    _div_string = NULL;
    _eq_string = NULL;
    _neg_string = NULL;
    _not_string = NULL;
    _lt_string = NULL;
    _gt_string = NULL;
    _bitshr_string = NULL;
    _bitshl_string = NULL;
    _bitand_string = NULL;
    _bitor_string = NULL;
    _bitxor_string = NULL;
    _bitcomp_string = NULL;
}

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
            fprintf(stderr, "%.*s()\n", function->name->raw_length, function->name->chars);
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
    read_history(NULL);

    reset_stack();
    vm.objects = NULL;
    vm.bytes_allocated = 0;
    vm.next_gc = 1024 * 1024;

    vm.gray_count = 0;
    vm.gray_capacity = 0;
    vm.gray_stack = NULL;

    init_literals();
    init_table(&vm.globals);
    init_table(&vm.strings);
    init_table(&vm.modules);

    init_method_names();

    vm.current_module = NULL;
    vm.module_count = 0;

    table_set(&vm.globals, AS_OBJ(get_bytes_class()->name), AS_OBJ(get_bytes_class()));
    table_set(&vm.globals, AS_OBJ(get_float_class()->name), AS_OBJ(get_float_class()));
    table_set(&vm.globals, AS_OBJ(get_char_class()->name), AS_OBJ(get_char_class()));
    table_set(&vm.globals, AS_OBJ(get_int_class()->name), AS_OBJ(get_int_class()));
    table_set(&vm.globals, AS_OBJ(get_list_class()->name), AS_OBJ(get_list_class()));
    table_set(&vm.globals, AS_OBJ(get_map_class()->name), AS_OBJ(get_map_class()));

#define SET_NATIVE_FN(name, fn)                                                                    \
    table_set(                                                                                     \
        &vm.globals,                                                                               \
        AS_OBJ(new_string(name, strlen(name))),                                                    \
        AS_OBJ(new_native_function(fn, name)))

    SET_NATIVE_FN("exit", exit_native_function);
    SET_NATIVE_FN("print", print_native_function);
    SET_NATIVE_FN("input", input_native_function);
    SET_NATIVE_FN("argv", argv_native_function);
    SET_NATIVE_FN("run_gc", run_gc_native_function);
    SET_NATIVE_FN("sleep", sleep_native_function);
    SET_NATIVE_FN("type", type_native_function);
    SET_NATIVE_FN("repr", repr_native_function);
    SET_NATIVE_FN("assert", assert_native_function);

#undef SET_NATIVE_FN
}

void free_vm() {
    free_table(&vm.globals);
    free_table(&vm.strings);
    free_table(&vm.modules);

    if (vm.current_module != NULL) {
        free(vm.current_module);
    }

    vm.module_count = 0;

    free_literals();
    free_method_names();
    free_objects();

    if (_source != NULL)
        free(_source);

    write_history(NULL);
}

static Table *get_current_global() {
    if (vm.current_module == NULL) {
        return &vm.globals;
    }

    if (vm.current_module != NULL)
        return &vm.current_module->current->globals;

    runtime_error("Could not import module '%s'.", vm.current_module->current->name);
    return NULL;
}

static bool is_std_import(ObjString *path) {
    return path->raw_length > 5 && memcmp(path->chars, "std:", 4) == 0;
}

static bool is_foreign_import(ObjString *path) {
    return path->raw_length > 9 && memcmp(path->chars, "foreign:", 8) == 0;
}

#define CALL_OK             0
#define CALL_INVALID_OBJ    1
#define CALL_STACK_OVERFLOW 2
#define CALL_INVALID_ARGC   3
#define CALL_UNKNOWN_MEMBER 4
#define CALL_NATIVE_ERR     5

static int call(ObjClosure *closure, int argc) {
    if (argc != closure->function->arity) {
        runtime_error("Expected %d arguments but got %d.", closure->function->arity, argc);
        return CALL_INVALID_ARGC;
    }

    if (vm.frame_count == FRAMES_MAX) {
        runtime_error("Stack overflow.");
        return CALL_STACK_OVERFLOW;
    }

    CallFrame *frame = &vm.frames[vm.frame_count++];
    frame->closure = closure;
    frame->ip = closure->function->chunk.code;
    frame->slots = vm.stack_top - argc - 1;

    return CALL_OK;
}

static int call_object(Obj *callee, int argc, Obj *caller) {
    switch (callee->type) {
        case OBJ_BOUND_METHOD:
            {
                ObjBoundMethod *bound = AS_BOUND_METHOD(callee);
                vm.stack_top[-argc - 1] = bound->receiver;
                return call_object(bound->method, argc, bound->receiver);
            }
        case OBJ_CLASS:
            {
                ObjClass *klass = AS_CLASS(callee);

                Obj *obj;
                Obj *new;
                if (table_get(&klass->statics, AS_OBJ(_new_string), &new)) {
                    push(AS_OBJ(new_nil()));
                    call_object(new, 0, AS_OBJ(klass));
                    obj = pop();
                } else {
                    ObjInstance *instance = new_instance(klass);
                    table_add_all(&klass->fields, &instance->fields);

                    obj = AS_OBJ(instance);
                }

                vm.stack_top[-argc - 1] = obj;

                Obj *initializer;
                if (table_get(&klass->methods, AS_OBJ(_init_string), &initializer)) {
                    call_object(initializer, argc, obj);
                    pop();
                } else if (argc != 0) {
                    runtime_error("Expected 0 arguments but got %d.", argc);
                    return CALL_INVALID_ARGC;
                }

                return CALL_OK;
            }
        case OBJ_CLOSURE:
            return call(AS_CLOSURE(callee), argc);
        case OBJ_NATIVE_FUNCTION:
            {
                NativeFn native = AS_NATIVE_FUNCTION(callee)->method;
                NativeResult result = native(argc, vm.stack_top - argc, caller);

                if (result.error != NULL) {
                    runtime_error(result.error);
                    free(result.error);
                    return CALL_NATIVE_ERR;
                }

                vm.stack_top -= argc + 1;
                push(result.value);

                return CALL_OK;
            }
        default:
            runtime_error("Cannot call '%s'", get_obj_kind(callee));
            return CALL_INVALID_OBJ;
    }
}

static int invoke_from_class(ObjClass *klass, ObjString *name, int argc, Obj *caller) {
    Obj *method;

    if (!table_get(&klass->methods, AS_OBJ(name), &method) &&
        !table_get(&klass->statics, AS_OBJ(name), &method) &&
        !table_get(&klass->fields, AS_OBJ(name), &method)) {
        runtime_error(
            "Undefined property '%.*s' for class '%.*s'.",
            name->raw_length,
            name->chars,
            klass->name->raw_length,
            klass->name->chars);
        return CALL_INVALID_OBJ;
    }

    int ret;
    if ((ret = call_object(method, argc, caller)) != CALL_OK) {
        if (ret < CALL_NATIVE_ERR)
            runtime_error(
                "Property '%.*s' of class '%.*s' is not callable.",
                name->raw_length,
                name->chars,
                klass->name->raw_length,
                klass->name->chars);
        return ret;
    }

    return CALL_OK;
}

static int invoke_scoped_member(ObjModule *module, ObjString *name, int argc) {
    Obj *member;
    if (table_get(&module->globals, AS_OBJ(name), &member)) {
        return call_object(member, argc, AS_OBJ(module));
    }

    runtime_error(
        "No member named '%.*s' in module '%s'.", name->raw_length, name->chars, module->name);
    return CALL_INVALID_OBJ;
}

static int invoke(ObjString *name, int argc) {
    Obj *receiver = peek(argc);

    switch (receiver->type) {
        case OBJ_INSTANCE:
            {
                ObjInstance *instance = AS_INSTANCE(receiver);

                Obj *value;
                if (table_get(&instance->fields, AS_OBJ(name), &value)) {
                    vm.stack_top[-argc - 1] = value;
                    return call_object(value, argc, AS_OBJ(instance));
                }

                return invoke_from_class(instance->obj.klass, name, argc, AS_OBJ(instance));
            }
        case OBJ_CLASS:
            {
                ObjClass *klass = AS_CLASS(receiver);

                Obj *method;
                if (!table_get(&klass->statics, AS_OBJ(name), &method) &&
                    !table_get(&klass->fields, AS_OBJ(name), &method)) {
                    runtime_error(
                        "Undefined property '%.*s' for class '%.*s'.",
                        name->raw_length,
                        name->chars,
                        klass->name->raw_length,
                        klass->name->chars);
                    return CALL_UNKNOWN_MEMBER;
                }

                int ret;
                if ((ret = call_object(method, argc, AS_OBJ(klass))) != CALL_OK) {
                    if (ret < CALL_NATIVE_ERR)
                        runtime_error(
                            "Property '%.*s' for class '%.*s' is not callable.",
                            name->raw_length,
                            name->chars,
                            klass->name->raw_length,
                            klass->name->chars);
                    return ret;
                }

                return CALL_OK;
            }
        default:
            {
                if (receiver->klass != NULL) {
                    return invoke_from_class(receiver->klass, name, argc, receiver);
                }

                runtime_error(
                    "Undefined property '%.*s' for '%s'.",
                    name->raw_length,
                    name->chars,
                    get_obj_kind(receiver));
                return CALL_UNKNOWN_MEMBER;
            }
    }
}

static int bind_method(ObjClass *klass, ObjString *name) {
    Obj *method;
    if (!table_get(&klass->methods, AS_OBJ(name), &method)) {
        runtime_error(
            "Undefined property '%.*s' of class '%.*s'.",
            name->raw_length,
            name->chars,
            klass->name->raw_length,
            klass->name->chars);
        return CALL_UNKNOWN_MEMBER;
    }

    ObjBoundMethod *bound = new_bound_method(peek(0), method);
    pop();
    push(AS_OBJ(bound));

    return CALL_OK;
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

static bool is_false(Obj *value) { return (IS_BOOL(value) && !(AS_BOOL(value)->value)); }

static InterpretResult run() {
    CallFrame *frame = &vm.frames[vm.frame_count - 1];

#define READ_BYTE()     (*frame->ip++)
#define READ_SHORT()    (frame->ip += 2, (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))
#define READ_CONSTANT() (frame->closure->function->chunk.constants.values[READ_BYTE()])
#define READ_STRING()   AS_STRING(READ_CONSTANT())
#define BINARY_OP(op_str)                                                                          \
    {                                                                                              \
        Obj *b = pop();                                                                            \
        Obj *a = pop();                                                                            \
        push(AS_OBJ(a->klass));                                                                    \
        push(a);                                                                                   \
        push(b);                                                                                   \
        Obj *op;                                                                                   \
        if (table_get(&a->klass->statics, AS_OBJ(op_str), &op)) {                                  \
            if (call_object(op, 2, a) != CALL_OK) {                                                \
                runtime_error("Invalid method signature.");                                        \
                return INTERPRET_RUNTIME_ERROR;                                                    \
            }                                                                                      \
        } else {                                                                                   \
            runtime_error("Invalid operation.");                                                   \
            return INTERPRET_RUNTIME_ERROR;                                                        \
        }                                                                                          \
        break;                                                                                     \
    }
#define UNARY_OP(op_str)                                                                           \
    {                                                                                              \
        Obj *a = pop();                                                                            \
        push(AS_OBJ(a->klass));                                                                    \
        push(a);                                                                                   \
        Obj *op;                                                                                   \
        if (table_get(&a->klass->statics, AS_OBJ(op_str), &op)) {                                  \
            if (call_object(op, 1, a) != CALL_OK) {                                                \
                runtime_error("Invalid method signature.");                                        \
                return INTERPRET_RUNTIME_ERROR;                                                    \
            }                                                                                      \
        } else {                                                                                   \
            runtime_error("Invalid operation.");                                                   \
            return INTERPRET_RUNTIME_ERROR;                                                        \
        }                                                                                          \
        break;                                                                                     \
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

                    for (int i = 0; i < pair_count; i++) {
                        Obj *key = (vm.stack_top - pair_count * 2)[i * 2];
                        if (!is_hashable(key)) {
                            runtime_error(
                                "Invalid key of type '%s' in MAP literal.", get_obj_kind(key));
                            return INTERPRET_RUNTIME_ERROR;
                        }
                    }

                    ObjMap *map = new_map(vm.stack_top - pair_count * 2, pair_count);
                    vm.stack_top -= pair_count * 2;
                    push(AS_OBJ(map));

                    break;
                }
            case OP_GET_INDEX:
                {
                    Obj *index_obj = pop();
                    Obj *value = pop();

                    Obj *indexed;
                    switch (value->type) {
                        case OBJ_LIST:
                            {
                                if (!IS_INT(index_obj)) {
                                    runtime_error("Lists can only be indexed "
                                                  "using integers.");
                                    return INTERPRET_RUNTIME_ERROR;
                                }

                                int64_t index = mpz_get_si(AS_INT(index_obj)->value);
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
                                if (!is_hashable(index_obj)) {
                                    runtime_error(
                                        "Maps cannot be indexed by '%s'.", get_obj_kind(index_obj));
                                    return INTERPRET_RUNTIME_ERROR;
                                }

                                ObjString *index = AS_STRING(index_obj);
                                ObjMap *map = AS_MAP(value);

                                if (!table_get(&map->table, AS_OBJ(index), &indexed)) {
                                    runtime_error("Key not found.");
                                    return INTERPRET_RUNTIME_ERROR;
                                }
                                break;
                            }
                        case OBJ_STRING:
                            {
                                if (!IS_INT(index_obj)) {
                                    runtime_error("Strings can only be indexed "
                                                  "using integers.");
                                    return INTERPRET_RUNTIME_ERROR;
                                }

                                int64_t index = mpz_get_si(AS_INT(index_obj)->value);
                                ObjString *string = AS_STRING(value);

                                if (index < 0)
                                    index = string->length + index;

                                if (index >= string->length) {
                                    runtime_error("Index out of bounds.");
                                    return INTERPRET_RUNTIME_ERROR;
                                }

                                int cur = 0;
                                ucs4_t at_index;
                                for (uint8_t *s = (uint8_t *)string->chars;
                                     (s = (uint8_t *)u8_next(&at_index, s)) != NULL;
                                     cur++) {
                                    if (index == cur) {
                                        indexed = AS_OBJ(new_char(at_index));
                                        break;
                                    }
                                }
                                break;
                            }
                        default:
                            {
                                runtime_error("'%s' cannot be indexed.", get_obj_kind(value));
                                return INTERPRET_RUNTIME_ERROR;
                            }
                    }

                    push(indexed);
                    break;
                }
            case OP_SET_INDEX:
                {
                    Obj *to_be_assigned = peek(0);
                    Obj *index_obj = peek(1);
                    Obj *value = peek(2);

                    if (IS_LIST(value)) {
                        if (!IS_INT(index_obj)) {
                            runtime_error("Lists can only be indexed using numbers.");
                            return INTERPRET_RUNTIME_ERROR;
                        }

                        int64_t index = mpz_get_si(AS_INT(index_obj)->value);
                        ObjList *list = AS_LIST(value);

                        if (index >= list->elems.count) {
                            runtime_error("Index out of bounds.");
                            return INTERPRET_RUNTIME_ERROR;
                        }

                        list->elems.values[index] = to_be_assigned;
                    } else if (IS_MAP(value)) {
                        if (!is_hashable(index_obj)) {
                            runtime_error(
                                "Maps cannot be indexed by '%s'.", get_obj_kind(index_obj));
                            return INTERPRET_RUNTIME_ERROR;
                        }

                        ObjString *index = AS_STRING(index_obj);
                        ObjMap *map = AS_MAP(value);

                        table_set(&map->table, AS_OBJ(index), to_be_assigned);
                    } else {
                        runtime_error("'%s' cannot be indexed.", get_obj_kind(value));
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
                    Table *globals = get_current_global();

                    if (globals == NULL) {
                        return INTERPRET_RUNTIME_ERROR;
                    }

                    if (table_get(globals, AS_OBJ(name), &value)) {
                        push(value);
                        break;
                    }

                    runtime_error("Undefined variable '%.*s'.", name->raw_length, name->chars);
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
                        runtime_error("Undefined variable '%.*s'.", name->raw_length, name->chars);
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

                                if (bind_method(instance->obj.klass, name) != CALL_OK) {
                                    runtime_error(
                                        "Could not bind '%s' from class '%s'.",
                                        name->chars,
                                        instance->obj.klass->name->chars);

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
                            if (obj->klass != NULL) {
                                Obj *method;
                                if (table_get(&obj->klass->methods, AS_OBJ(name), &method)) {
                                    pop();
                                    push(AS_OBJ(new_bound_method(obj, method)));
                                } else {
                                    runtime_error(
                                        "No method named '%s' on '%s'",
                                        name->chars,
                                        get_obj_kind(obj));
                                    return INTERPRET_RUNTIME_ERROR;
                                }
                            } else {
                                runtime_error(
                                    "Properties and methods do not exist for "
                                    "'%s'.",
                                    get_obj_kind(obj));
                                return INTERPRET_RUNTIME_ERROR;
                            }
                    }
                    break;
                }
            case OP_SET_PROPERTY:
                {
                    if (IS_INSTANCE(peek(1))) {
                        ObjInstance *instance = AS_INSTANCE(peek(1));

                        if (instance->obj.klass->is_native) {
                            runtime_error(
                                "Cannot set property of instance of native class '%s'.",
                                instance->obj.klass->name->chars);
                            return INTERPRET_RUNTIME_ERROR;
                        }

                        table_set(&instance->fields, AS_OBJ(READ_STRING()), peek(0));

                        Obj *value = pop();
                        pop();

                        push(value);
                        break;
                    } else if (IS_CLASS(peek(1))) {
                        ObjClass *klass = AS_CLASS(peek(1));

                        if (klass->is_native) {
                            runtime_error(
                                "Cannot set property of native class '%.*s'.",
                                klass->name->raw_length,
                                klass->name->chars);
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
                        runtime_error("Cannot scope %s.", get_obj_kind(peek(0)));
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
                        "No method named '%.*s' found in module '%s'.",
                        name->raw_length,
                        name->chars,
                        module->name);
                    return INTERPRET_RUNTIME_ERROR;
                }
            case OP_EQUAL:
                BINARY_OP(_eq_string)
            case OP_GREATER:
                BINARY_OP(_gt_string)
            case OP_LESS:
                BINARY_OP(_lt_string)
            case OP_ADD:
                // {
                //     if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
                //         ObjString *b = AS_STRING(peek(0));
                //         ObjString *a = AS_STRING(peek(1));

                //         int length = a->raw_length + b->raw_length;
                //         char *chars = ALLOCATE(char, length + 1);
                //         memcpy(chars, a->chars, a->raw_length);
                //         memcpy(chars + a->raw_length, b->chars, b->raw_length);
                //         chars[length] = '\0';

                //         ObjString *result = take_string(chars, length);
                //         pop();
                //         pop();
                //         push(AS_OBJ(result));
                //     } else if (IS_INT(peek(0)) && IS_INT(peek(1))) {
                //         mpz_t _int;
                //         mpz_init(_int);
                //         mpz_add(_int, AS_INT(pop())->value, AS_INT(pop())->value);
                //         push(AS_OBJ(new_int(_int)));
                //     } else if (IS_FLOAT(peek(0)) && IS_FLOAT(peek(1))) {
                //         push(AS_OBJ(new_float(AS_FLOAT(pop())->value + AS_FLOAT(pop())->value)));
                //     } else {
                //         runtime_error(
                //             "Unsupported operand types for '+', '%s' and '%s'.",
                //             get_obj_kind(peek(0)),
                //             get_obj_kind(peek(1)));
                //         return INTERPRET_RUNTIME_ERROR;
                //     }
                //     break;
                // }
                BINARY_OP(_add_string)
            case OP_SUBTRACT:
                BINARY_OP(_sub_string)
            case OP_BITWISE_AND:
                BINARY_OP(_bitand_string)
            case OP_BITWISE_OR:
                BINARY_OP(_bitor_string)
            case OP_BITWISE_XOR:
                BINARY_OP(_bitxor_string)
            case OP_BITWISE_NOT:
                UNARY_OP(_bitcomp_string)
            case OP_SHIFT_RIGHT:
                BINARY_OP(_bitshr_string)
            case OP_SHIFT_LEFT:
                BINARY_OP(_bitshl_string)
            case OP_MULTIPLY:
                BINARY_OP(_mul_string)
            case OP_DIVIDE:
                BINARY_OP(_div_string)
            case OP_NOT:
                UNARY_OP(_not_string)
            case OP_NEGATE:
                UNARY_OP(_neg_string)
            case OP_JUMP:
                {
                    uint16_t offset = READ_SHORT();
                    frame->ip += offset;
                    break;
                }
            case OP_JUMP_IF_FALSE:
                {
                    uint16_t offset = READ_SHORT();
                    if (is_false(peek(0)))
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
                    if (call_object(peek(argc), argc, NULL) != CALL_OK) {
                        return INTERPRET_RUNTIME_ERROR;
                    }
                    frame = &vm.frames[vm.frame_count - 1];
                    break;
                }
            case OP_METHOD_INVOKE:
                {
                    ObjString *method = READ_STRING();
                    int argc = READ_BYTE();
                    if (invoke(method, argc) != CALL_OK) {
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
                    if (invoke_from_class(superclass, method, argc, NULL) != CALL_OK) {
                        return INTERPRET_RUNTIME_ERROR;
                    }
                    frame = &vm.frames[vm.frame_count - 1];
                    break;
                }
            case OP_SCOPE_INVOKE:
                {
                    ObjString *method = READ_STRING();
                    int argc = READ_BYTE();

                    if (invoke_scoped_member(AS_MODULE(peek(argc)), method, argc) != CALL_OK)
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
                        int part_count = 1;
                        for (int i = 4; i < import_path->raw_length; i++)
                            if (import_path->chars[i] == '/')
                                part_count++;

                        char **parts = malloc(part_count * sizeof(char *));
                        char *temp = malloc(import_path->raw_length - 3);

                        strcpy(temp, import_path->chars + 4);

                        parts[0] = strtok(temp, "/");
                        for (int i = 1; i < part_count; i++) {
                            parts[i] = strtok(NULL, "/");
                        }

                        ObjModule *std_module = get_module(part_count, parts);

                        free(parts);
                        free(temp);

                        if (std_module == NULL) {
                            runtime_error(
                                "No standard module named '%.*s'.",
                                import_path->raw_length,
                                import_path->chars);
                            return INTERPRET_RUNTIME_ERROR;
                        } else
                            push(AS_OBJ(std_module));
                    } else if (is_foreign_import(import_path)) {
                        void *handle = dlopen(import_path->chars + 8, RTLD_LAZY);
                        if (!handle) {
                            runtime_error("No foreign import library found.");
                            return INTERPRET_RUNTIME_ERROR;
                        }

                        struct {
                            ObjModule *(*init)(void);
                        } *foreign_module = dlsym(handle, "get_module");

                        push(AS_OBJ(foreign_module->init()));
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
                        module->current = new_module(import_path->chars);

                        vm.current_module = module;
                        vm.module_count++;

                        free(source);
                    }
                    break;
                }
        }
    }

#undef READ_BYTE
#undef READ_SHORT
#undef READ_CONSTANT
#undef READ_STRING
#undef BINARY_OP
#undef UNARY_OP
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
