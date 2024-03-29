#ifndef lox_compiler_h
#define lox_compiler_h

#include "object.h"
#include "offset_array.h"
#include "scanner.h"
#include "vm.h"

typedef struct {
    Token current;
    Token previous;
    bool had_error;
    bool panic_mode;
} Parser;

typedef struct {
    Token name;
    int depth;
    bool is_captured;
} Local;

typedef struct {
    uint8_t index;
    bool is_local;
    struct ObjUpvalue *next;
} Upvalue;

typedef enum {
    TYPE_FUNCTION,
    TYPE_INITIALIZER,
    TYPE_METHOD,
    TYPE_METHOD_STATIC,
    TYPE_SCRIPT,
} FunctionType;

typedef struct Compiler {
    struct Compiler *enclosing;
    ObjFunction *function;
    FunctionType type;

    Local locals[UINT8_COUNT];
    int local_count;

    Upvalue upvalues[UINT8_COUNT];
    int scope_depth;

    bool in_loop;
    OffsetArray breaks;
    OffsetArray continues;
} Compiler;

typedef struct ClassCompiler {
    struct ClassCompiler *enclosing;
    bool has_super_class;
} ClassCompiler;

typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,
    PREC_OR,
    PREC_AND,
    PREC_EQUALITY,
    PREC_COMPARISON,
    PREC_BITWISE_OR,
    PREC_BITWISE_XOR,
    PREC_BITWISE_AND,
    PREC_SHIFT,
    PREC_TERM,
    PREC_FACTOR,
    PREC_UNARY,
    PREC_CALL,
    PREC_PRIMARY
} Precedence;

typedef void (*ParseFn)(bool);

typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

ObjFunction *compile(const char *);
void mark_compiler_roots();

#endif // lox_compiler_h
