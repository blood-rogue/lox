#ifndef clox_chunk_h
#define clox_chunk_h

#include "array.h"
#include "common.h"

typedef enum {
    OP_CONSTANT,
    OP_GET_INDEX,
    OP_SET_INDEX,
    OP_LIST,
    OP_MAP,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_POP,
    OP_GET_LOCAL,
    OP_SET_LOCAL,
    OP_GET_GLOBAL,
    OP_SET_GLOBAL,
    OP_DEFINE_GLOBAL,
    OP_GET_UPVALUE,
    OP_SET_UPVALUE,
    OP_GET_PROPERTY,
    OP_SET_PROPERTY,
    OP_GET_SUPER,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NOT,
    OP_NEGATE,
    OP_JUMP,
    OP_JUMP_IF_FALSE,
    OP_LOOP,
    OP_CALL,
    OP_METHOD_INVOKE,
    OP_SUPER_INVOKE,
    OP_CLOSURE,
    OP_CLOSE_UPVALUE,
    OP_RETURN,
    OP_CLASS,
    OP_INHERIT,
    OP_METHOD,
    OP_STATIC_METHOD,
    OP_IMPORT,
    OP_GET_SCOPED,
    OP_SCOPE_INVOKE,
    OP_END
} OpCode;

typedef struct {
    int count;
    int capacity;
    uint8_t *code;
    int *lines;
    Array constants;
} Chunk;

void init_chunk(Chunk *);
void free_chunk(Chunk *);
void write_chunk(Chunk *, uint8_t, int);

int add_constant(Chunk *, Obj *);

#endif // clox_chunk_h
