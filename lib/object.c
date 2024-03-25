#include <unistr.h>

#include "memory.h"
#include "native.h"
#include "object.h"
#include "vm.h"

ObjNil *_NIL = NULL;
ObjBool *_TRUE = NULL;
ObjBool *_FALSE = NULL;

extern VM vm;

#define ALLOCATE_OBJ(type, object_type) (type *)allocate_object(sizeof(type), object_type)

static Obj *allocate_object(size_t size, ObjType type) {
    Obj *object = AS_OBJ(reallocate(NULL, 0, size));
    memset(object, 0, sizeof(Obj));

    object->type = type;
    object->next = vm.objects;
    vm.objects = object;

    return object;
}

int utf_strlen(char *chars) {
    ucs4_t puc;
    int len = 0;
    for (uint8_t *tmp = (uint8_t *)chars; tmp != NULL; tmp = (uint8_t *)u8_next(&puc, tmp))
        len++;

    return len - !puc;
}

static ObjString *allocate_string(char *chars, int length, uint32_t hash) {
    ObjString *string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    // string->obj.klass = get_string_class();

    string->raw_length = length;
    string->length = utf_strlen(chars);
    string->chars = chars;
    string->obj.hash = (uint64_t)hash;

    push(AS_OBJ(string));
    table_set(&vm.strings, AS_OBJ(string), AS_OBJ(new_nil()));
    pop();

    return string;
}

ObjNil *new_nil() { return _NIL; }

ObjInt *new_int(mpz_t value) {
    ObjInt *integer = ALLOCATE_OBJ(ObjInt, OBJ_INT);
    integer->obj.klass = get_int_class();

    mpz_init_set(integer->value, value);

    return integer;
}

ObjInt *new_int_i(int64_t si) {
    ObjInt *integer = ALLOCATE_OBJ(ObjInt, OBJ_INT);
    integer->obj.klass = get_int_class();

    mpz_init_set_si(integer->value, si);

    return integer;
}

ObjInt *new_int_s(char *str, int base) {
    ObjInt *integer = ALLOCATE_OBJ(ObjInt, OBJ_INT);
    integer->obj.klass = get_int_class();

    mpz_init_set_str(integer->value, str, base);

    return integer;
}

ObjMap *new_map(Obj **elems, int pair_count) {
    ObjMap *map = ALLOCATE_OBJ(ObjMap, OBJ_MAP);
    map->obj.klass = get_map_class();

    init_table(&map->table);
    map->table.count = pair_count;

    for (int i = 0; i < pair_count; i++) {
        table_set(&map->table, elems[i * 2], elems[i * 2 + 1]);
    }

    return map;
}

ObjChar *new_char(ucs4_t value) {
    ObjChar *_char = ALLOCATE_OBJ(ObjChar, OBJ_CHAR);
    _char->obj.klass = get_char_class();

    _char->value = value;
    _char->obj.hash = (uint64_t)value;

    return _char;
}

ObjList *new_list(Obj **elems, int elem_count) {
    ObjList *list = ALLOCATE_OBJ(ObjList, OBJ_LIST);
    list->obj.klass = get_list_class();

    init_array(&list->elems);

    for (int i = 0; i < elem_count; i++) {
        write_array(&list->elems, elems[i]);
    }

    return list;
}

ObjBool *new_bool(bool value) {
    if (value)
        return _TRUE;
    return _FALSE;
}

ObjBytes *new_bytes(const uint8_t *inp, int length) {
    ObjBytes *bytes = ALLOCATE_OBJ(ObjBytes, OBJ_BYTES);
    bytes->obj.klass = get_bytes_class();

    uint8_t *_bytes = malloc(length);
    memcpy(_bytes, inp, length);

    bytes->length = length;
    bytes->bytes = _bytes;

    return bytes;
}

ObjFloat *new_float(mpfr_t value) {
    ObjFloat *float_ = ALLOCATE_OBJ(ObjFloat, OBJ_FLOAT);
    float_->obj.klass = get_float_class();

    mpfr_init_set(float_->value, value, MPFR_RNDN);

    float_->obj.hash = (uint64_t)value;

    return float_;
}

ObjFloat *new_float_d(double value) {
    ObjFloat *float_ = ALLOCATE_OBJ(ObjFloat, OBJ_FLOAT);
    float_->obj.klass = get_float_class();

    mpfr_init_set_d(float_->value, value, MPFR_RNDN);

    float_->obj.hash = (uint64_t)value;

    return float_;
}

ObjFloat *new_float_s(char *value, int base) {
    ObjFloat *float_ = ALLOCATE_OBJ(ObjFloat, OBJ_FLOAT);
    float_->obj.klass = get_float_class();

    mpfr_init_set_str(float_->value, value, base, MPFR_RNDN);

    float_->obj.hash = (uint64_t)value;

    return float_;
}

ObjString *new_string(const char *chars, int length) {
    uint32_t hash = hash_string(chars, length);

    Obj *interned = table_find_string(&vm.strings, hash);
    if (interned != NULL && interned->type == OBJ_STRING)
        return AS_STRING(interned);

    char *heap_chars = ALLOCATE(char, length + 1);
    memcpy(heap_chars, chars, length);
    heap_chars[length] = '\0';

    return allocate_string(heap_chars, length, hash);
}

ObjClosure *new_closure(ObjFunction *function) {
    ObjUpvalue **upvalues = ALLOCATE(ObjUpvalue *, function->upvalue_count);
    for (int i = 0; i < function->upvalue_count; i++) {
        upvalues[i] = NULL;
    }

    ObjClosure *closure = ALLOCATE_OBJ(ObjClosure, OBJ_CLOSURE);
    closure->function = function;

    closure->upvalues = upvalues;
    closure->upvalue_count = function->upvalue_count;

    return closure;
}

ObjFunction *new_function() {
    ObjFunction *function = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION);
    function->arity = 0;
    function->name = NULL;
    function->upvalue_count = 0;

    init_chunk(&function->chunk);
    return function;
}

ObjUpvalue *new_upvalue(Obj **slot) {
    ObjUpvalue *upvalue = ALLOCATE_OBJ(ObjUpvalue, OBJ_UPVALUE);
    upvalue->closed = AS_OBJ(new_nil());
    upvalue->location = slot;

    upvalue->next = NULL;
    return upvalue;
}

ObjClass *new_class(ObjString *name) {
    ObjClass *klass = ALLOCATE_OBJ(ObjClass, OBJ_CLASS);
    klass->name = name;
    klass->is_native = false;
    init_table(&klass->methods);
    init_table(&klass->statics);
    init_table(&klass->fields);

    return klass;
}

ObjInstance *new_instance(ObjClass *klass) {
    ObjInstance *instance = ALLOCATE_OBJ(ObjInstance, OBJ_INSTANCE);
    instance->obj.klass = klass;
    init_table(&instance->fields);

    return instance;
}

ObjBoundMethod *new_bound_method(Obj *receiver, Obj *method) {
    ObjBoundMethod *bound = ALLOCATE_OBJ(ObjBoundMethod, OBJ_BOUND_METHOD);
    bound->receiver = receiver;
    bound->method = method;

    return bound;
}

ObjModule *new_module(const char *name) {
    ObjModule *module = ALLOCATE_OBJ(ObjModule, OBJ_MODULE);

    init_table(&module->globals);
    module->name = name;

    return module;
}

ObjNativeFunction *new_native_function(NativeFn fn, char *name) {
    ObjNativeFunction *native = ALLOCATE_OBJ(ObjNativeFunction, OBJ_NATIVE_FUNCTION);

    native->method = fn;
    native->name = name;

    return native;
}

ObjNativeStruct *new_native_struct(void *ptr, FreeFn free_fn) {
    ObjNativeStruct *native = ALLOCATE_OBJ(ObjNativeStruct, OBJ_NATIVE_STRUCT);

    native->ptr = ptr;
    native->free_fn = free_fn;

    return native;
}

ObjList *argv_list(int argc, const char **argv) {
    ObjList *list = ALLOCATE_OBJ(ObjList, OBJ_LIST);
    list->obj.klass = get_list_class();

    Array arr;
    init_array(&arr);

    for (int i = 0; i < argc; i++) {
        write_array(&arr, AS_OBJ(new_string(argv[i], strlen(argv[i]))));
    }

    list->elems = arr;

    return list;
}

ObjBytes *take_bytes(uint8_t *inp, int length) {
    ObjBytes *bytes = ALLOCATE_OBJ(ObjBytes, OBJ_BYTES);
    bytes->obj.klass = get_bytes_class();

    bytes->length = length;
    bytes->bytes = inp;

    return bytes;
}

ObjString *take_string(char *chars, int length) {
    uint32_t hash = hash_string(chars, length);

    Obj *interned = table_find_string(&vm.strings, hash);
    if (interned != NULL && interned->type == OBJ_STRING) {
        free(chars);
        return AS_STRING(interned);
    }

    return allocate_string(chars, length, hash);
}

ObjClass *new_native_class(char *name) {
    ObjClass *klass = new_class(new_string(name, strlen(name)));
    klass->is_native = true;

    return klass;
}

static void print_function(ObjFunction *function) {
    if (function->name == NULL) {
        printf("<script>");
        return;
    }
    printf("<fn %.*s>", function->name->raw_length, function->name->chars);
}

static void print_list(Array *elems) {
    Obj **values = elems->values;

    printf("[");
    if (elems->count > 0) {
        repr_object(values[0]);
        for (int i = 1; i < elems->count; i++) {
            printf(", ");
            repr_object(values[i]);
        }
    }
    printf("]");
}

static void print_map(ObjMap *map) {
    int count = map->table.count;
    Obj *value;

    printf("{");
    for (int i = 0; i < map->table.capacity && count > 0; i++) {
        Entry *entry = &map->table.entries[i];

        if (entry->key == NULL)
            continue;

        repr_object(entry->key);
        printf(": ");
        table_get(&map->table, entry->key, &value);
        repr_object(value);
        printf(",");

        count--;
    }

    printf("}");
}

static void print_char(ucs4_t ch, bool repr) {
    uint8_t s[5] = {0};
    u8_uctomb(s, ch, 4);
    s[4] = '\0';

    if (repr)
        printf("'%s'", s);
    else
        printf("%s", s);
}

static void print_bytes(ObjBytes *bytes) {
    printf("[");
    for (int i = 0; i < bytes->length; i++) {
        printf("%d, ", bytes->bytes[i]);
    }
    printf("]");
}

void print_object(Obj *obj) {
    switch (obj->type) {
        case OBJ_NIL:
            printf("(nil)");
            break;
        case OBJ_FLOAT:
            mpfr_printf("%.5Rg", AS_FLOAT(obj)->value);
            break;
        case OBJ_INT:
            printf("%s", mpz_get_str(NULL, 10, AS_INT(obj)->value));
            break;
        case OBJ_CHAR:
            print_char(AS_CHAR(obj)->value, false);
            break;
        case OBJ_BOOL:
            printf(AS_BOOL(obj)->value ? "true" : "false");
            break;
        case OBJ_MAP:
            print_map(AS_MAP(obj));
            break;
        case OBJ_LIST:
            print_list(&AS_LIST(obj)->elems);
            break;
        case OBJ_BOUND_METHOD:
            print_object(AS_BOUND_METHOD(obj)->method);
            break;
        case OBJ_CLASS:
            printf("<class '%.*s'>", AS_CLASS(obj)->name->raw_length, AS_CLASS(obj)->name->chars);
            break;
        case OBJ_CLOSURE:
            print_function(AS_CLOSURE(obj)->function);
            break;
        case OBJ_STRING:
            printf("%.*s", AS_STRING(obj)->raw_length, AS_STRING(obj)->chars);
            break;
        case OBJ_BYTES:
            print_bytes(AS_BYTES(obj));
            break;
        case OBJ_INSTANCE:
            printf("<'%.*s' instance>", obj->klass->name->raw_length, obj->klass->name->chars);
            break;
        case OBJ_NATIVE_FUNCTION:
            printf("<native fn '%s'>", AS_NATIVE_FUNCTION(obj)->name);
            break;
        case OBJ_FUNCTION:
            print_function(AS_FUNCTION(obj));
            break;
        case OBJ_UPVALUE:
            printf("upvalue");
            break;
        case OBJ_MODULE:
            printf("<module '%s'>", AS_MODULE(obj)->name);
            break;
        case OBJ_NATIVE_STRUCT:
            printf("<native struct at %p>", AS_NATIVE_STRUCT(obj)->ptr);
            break;
    }
}

void repr_object(Obj *obj) {
    switch (obj->type) {
        case OBJ_STRING:
            printf("\"%.*s\"", AS_STRING(obj)->raw_length, AS_STRING(obj)->chars);
            break;
        case OBJ_CHAR:
            print_char(AS_CHAR(obj)->value, true);
            break;
        default:
            print_object(obj);
            break;
    }
}

uint32_t hash_string(const char *key, int length) {
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++) {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;
}

bool obj_equal(Obj *a, Obj *b) {
    if (a->type != b->type)
        return false;

    switch (a->type) {
        case OBJ_NIL:
            return true;
        case OBJ_INT:
            return mpz_cmp(AS_INT(a)->value, AS_INT(b)->value) == 0;
        case OBJ_CHAR:
            return AS_CHAR(a)->value == AS_CHAR(b)->value;
        case OBJ_FLOAT:
            return mpfr_cmp(AS_FLOAT(a)->value, AS_FLOAT(b)->value);
        case OBJ_STRING:
            {
                ObjString *str_a = AS_STRING(a);
                ObjString *str_b = AS_STRING(b);

                return a->hash == b->hash && str_a->raw_length == str_b->raw_length &&
                       memcmp(str_a->chars, str_b->chars, str_a->raw_length) == 0;
            }
        case OBJ_BYTES:
            {
                ObjBytes *bytes_a = AS_BYTES(a);
                ObjBytes *bytes_b = AS_BYTES(b);

                return bytes_a->length == bytes_b->length &&
                       memcmp(bytes_a->bytes, bytes_b->bytes, bytes_a->length) == 0;
            }
        default:
            return a == b;
    }
}

#define HASHABLE 427 // 000000000110101011

inline bool is_hashable(Obj *obj) { return (HASHABLE & obj->type) == obj->type; }

void init_literals() {
    _NIL = malloc(sizeof(ObjNil));
    _TRUE = malloc(sizeof(ObjBool));
    _FALSE = malloc(sizeof(ObjBool));

    _NIL->obj.type = OBJ_NIL;
    _NIL->obj.is_marked = false;

    _TRUE->obj.type = OBJ_BOOL;
    _TRUE->obj.is_marked = false;
    _TRUE->obj.hash = 1;

    _TRUE->value = true;

    _FALSE->obj.type = OBJ_BOOL;
    _FALSE->obj.is_marked = false;
    _FALSE->value = false;
}

void free_literals() {
    free(_NIL);
    free(_TRUE);
    free(_FALSE);
}

char *get_obj_kind(Obj *obj) { return obj->klass->name->chars; }
