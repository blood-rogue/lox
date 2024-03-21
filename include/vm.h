#ifndef lox_vm_h
#define lox_vm_h

#include "object.h"
#include "table.h"

#define FRAMES_MAX 64
#define STACK_MAX  (FRAMES_MAX * UINT8_COUNT)

typedef struct {
    ObjClosure *closure;
    uint8_t *ip;
    Obj **slots;
} CallFrame;

typedef struct Module {
    struct Module *prev;
    ObjModule *current;
} Module;

typedef struct {
    CallFrame frames[FRAMES_MAX];
    int frame_count;

    Obj *stack[STACK_MAX];
    Obj **stack_top;

    Table globals;
    Table strings;
    Table modules;

    Module *current_module;
    uint8_t module_count;

    NativeTable **builtin_methods;
    NativeTable builtin_functions;

    ObjString *init_string;
    ObjUpvalue *open_upvalues;

    size_t bytes_allocated;
    size_t next_gc;

    Obj *objects;

    int gray_count;
    int gray_capacity;
    Obj **gray_stack;
} VM;

typedef enum { INTERPRET_OK, INTERPRET_COMPILE_ERROR, INTERPRET_RUNTIME_ERROR } InterpretResult;

void init_vm();
void free_vm();

InterpretResult interpret(char *);

void push(Obj *);
Obj *pop();

#endif // lox_vm_h
