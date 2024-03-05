#include "compiler.h"
#include "common.h"
#include "memory.h"

#ifdef DEBUG
#include "debug.h"
#endif

Parser parser;
Compiler *current = NULL;
ClassCompiler *current_class = NULL;

static Chunk *current_chunk() { return &current->function->chunk; }

static void error_at(Token *token, const char *message) {
    if (parser.panic_mode)
        return;
    parser.panic_mode = true;

    fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type == TOKEN_ERROR) {
    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    parser.had_error = true;
}

static void error_at_current(const char *message) { error_at(&parser.current, message); }

static void error(const char *message) { error_at(&parser.previous, message); }

static void advance() {
    parser.previous = parser.current;

    for (;;) {
        parser.current = scan_token();
        if (parser.current.type != TOKEN_ERROR)
            break;

        error_at_current(parser.current.start);
    }
}

static void consume(TokenType type, const char *message) {
    if (parser.current.type == type) {
        advance();
        return;
    }

    error_at_current(message);
}

static bool check(TokenType type) { return parser.current.type == type; }

static bool match(TokenType type) {
    if (!check(type))
        return false;
    advance();
    return true;
}

static void emit_byte(uint8_t byte) { write_chunk(current_chunk(), byte, parser.previous.line); }

static void emit_bytes(uint8_t byte1, uint8_t byte2) {
    emit_byte(byte1);
    emit_byte(byte2);
}

static void emit_loop(int loop_start) {
    emit_byte(OP_LOOP);

    int offset = current_chunk()->count - loop_start + 2;
    if (offset > UINT16_MAX)
        error("Loop body too large.");

    emit_byte((uint8_t)((offset >> 8) & 0xff));
    emit_byte(offset & 0xff);
}

static int emit_jump(uint8_t instruction) {
    emit_byte(instruction);
    emit_byte(0xff);
    emit_byte(0xff);
    return current_chunk()->count - 2;
}

static void emit_return() {
    if (current->type == TYPE_INITIALIZER) {
        emit_bytes(OP_GET_LOCAL, 0);
    } else {
        emit_byte(OP_NIL);
    }

    emit_byte(OP_RETURN);
}

static uint8_t make_constant(Obj *value) {
    int constant = add_constant(current_chunk(), value);
    if (constant > UINT8_MAX) {
        error("Too many constants in one chunk.");
        return 0;
    }

    return (uint8_t)constant;
}

static uint8_t identifier_constant(Token *name) {
    return make_constant(AS_OBJ(new_string(name->start, name->length)));
}

static bool identifiers_equal(Token *a, Token *b) {
    if (a->length != b->length)
        return false;
    return memcmp(a->start, b->start, a->length) == 0;
}

static int resolve_local(Compiler *compiler, Token *name) {
    for (int i = compiler->local_count - 1; i >= 0; i--) {
        Local *local = &compiler->locals[i];
        if (identifiers_equal(name, &local->name)) {
            if (local->depth == -1) {
                error("Can't read local variable in its own initializer.");
            }
            return i;
        }
    }

    return -1;
}

static int add_upvalue(Compiler *compiler, uint8_t index, bool is_local) {
    int upvalue_count = compiler->function->upvalue_count;

    for (int i = 0; i < upvalue_count; i++) {
        Upvalue *upvalue = &compiler->upvalues[i];
        if (upvalue->index == index && upvalue->is_local == is_local) {
            return i;
        }
    }

    if (upvalue_count == UINT8_COUNT) {
        error("Too many closure variables in function.");
        return 0;
    }

    compiler->upvalues[upvalue_count].is_local = is_local;
    compiler->upvalues[upvalue_count].index = index;
    return compiler->function->upvalue_count++;
}

static int resolve_upvalue(Compiler *compiler, Token *name) {
    if (compiler->enclosing == NULL)
        return -1;

    int local = resolve_local(compiler->enclosing, name);
    if (local != -1) {
        compiler->enclosing->locals[local].is_captured = true;
        return add_upvalue(compiler, (uint8_t)local, true);
    }

    int upvalue = resolve_upvalue(compiler->enclosing, name);
    if (upvalue != -1) {
        return add_upvalue(compiler, (uint8_t)upvalue, false);
    }

    return -1;
}

static void add_local(Token name) {
    if (current->local_count == UINT8_COUNT) {
        error("Too many local variables in function.");
        return;
    }

    Local *local = &current->locals[current->local_count++];
    local->name = name;
    local->depth = -1;
    local->is_captured = false;
}

static void declare_variable() {
    if (current->scope_depth == 0)
        return;

    Token *name = &parser.previous;

    for (int i = current->local_count - 1; i >= 0; i--) {
        Local *local = &current->locals[i];
        if (local->depth != -1 && local->depth < current->scope_depth) {
            break;
        }

        if (identifiers_equal(name, &local->name)) {
            error("Already a variable with this name in this scope.");
        }
    }

    add_local(*name);
}

static void emit_constant(Obj *value) { emit_bytes(OP_CONSTANT, make_constant(value)); }

static inline void patch_jump_to(int offset, int jump) {
    if (jump > UINT16_MAX) {
        error("Too much code to jump over.");
    }

    current_chunk()->code[offset] = (jump >> 8) & 0xff;
    current_chunk()->code[offset + 1] = jump & 0xff;
}

static void patch_jump(int offset) { patch_jump_to(offset, current_chunk()->count - offset - 2); }

static void init_compiler(Compiler *compiler, FunctionType type) {
    compiler->enclosing = current;
    compiler->function = NULL;
    compiler->type = type;

    compiler->local_count = 0;
    compiler->scope_depth = 0;

    compiler->function = new_function();

    compiler->in_loop = false;
    init_offset_array(&compiler->breaks);
    init_offset_array(&compiler->continues);

    current = compiler;

    if (type != TYPE_SCRIPT) {
        current->function->name = new_string(parser.previous.start, parser.previous.length);
    }

    Local *local = &current->locals[current->local_count++];
    local->depth = 0;
    local->is_captured = false;

    if (type != TYPE_FUNCTION) {
        local->name.start = "this";
        local->name.length = 4;
    } else {
        local->name.start = "";
        local->name.length = 0;
    }
}

static ObjFunction *end_compiler(bool ended) {
    if (ended)
        emit_byte(OP_END);
    else
        emit_return();
    ObjFunction *function = current->function;

#ifdef DEBUG
    if (!parser.had_error) {
        disassemble_chunk(
            current_chunk(), function->name != NULL ? function->name->chars : "<script>");
    }
#endif

    current->in_loop = false;
    free_offset_array(&current->breaks);
    free_offset_array(&current->continues);

    current = current->enclosing;
    return function;
}

static void begin_scope() { current->scope_depth++; }

static void end_scope() {
    current->scope_depth--;

    while (current->local_count > 0 &&
           current->locals[current->local_count - 1].depth > current->scope_depth) {
        if (current->locals[current->local_count - 1].is_captured) {
            emit_byte(OP_CLOSE_UPVALUE);
        } else {
            emit_byte(OP_POP);
        }
        current->local_count--;
    }
}

static void expression();
static void statement();
static void declaration();
static void function(FunctionType);
static ParseRule *get_rule(TokenType type);
static void parse_precedence(Precedence precedence);

static void binary(bool) {
    TokenType operator_type = parser.previous.type;
    ParseRule *rule = get_rule(operator_type);
    parse_precedence((Precedence)(rule->precedence + 1));

    switch (operator_type) {
        case TOKEN_BANG_EQUAL:
            emit_bytes(OP_EQUAL, OP_NOT);
            break;
        case TOKEN_EQUAL_EQUAL:
            emit_byte(OP_EQUAL);
            break;
        case TOKEN_GREATER:
            emit_byte(OP_GREATER);
            break;
        case TOKEN_GREATER_EQUAL:
            emit_bytes(OP_LESS, OP_NOT);
            break;
        case TOKEN_LESS:
            emit_byte(OP_LESS);
            break;
        case TOKEN_LESS_EQUAL:
            emit_bytes(OP_GREATER, OP_NOT);
            break;
        case TOKEN_PLUS:
            emit_byte(OP_ADD);
            break;
        case TOKEN_MINUS:
            emit_byte(OP_SUBTRACT);
            break;
        case TOKEN_STAR:
            emit_byte(OP_MULTIPLY);
            break;
        case TOKEN_SLASH:
            emit_byte(OP_DIVIDE);
            break;
        case TOKEN_AMPERSAND:
            emit_byte(OP_BINARY_AND);
            break;
        case TOKEN_PIPE:
            emit_byte(OP_BINARY_OR);
            break;
        case TOKEN_CARET:
            emit_byte(OP_BINARY_XOR);
            break;
        case TOKEN_GREATER_GREATER:
            emit_byte(OP_SHIFT_RIGHT);
            break;
        case TOKEN_LESS_LESS:
            emit_byte(OP_SHIFT_LEFT);
            break;
        default:
            return;
    }
}

static uint8_t expression_list(TokenType terminator) {
    uint8_t expression_count = 0;
    if (!check(terminator)) {
        do {
            expression();

            if (expression_count == 255) {
                error("Can't have more than 255 expressions.");
            }

            expression_count++;
        } while (match(TOKEN_COMMA));
    }

    consume(terminator, "Invalid terminator.");
    return expression_count;
}

static uint8_t map_pair_list() {
    uint8_t expression_count = 0;
    if (!check(TOKEN_RIGHT_BRACE)) {
        do {
            expression();
            consume(TOKEN_COLON, "Expected ':' after key.");
            expression();

            if (expression_count == 255) {
                error("Can't have more than 255 expressions.");
            }

            expression_count++;
        } while (match(TOKEN_COMMA));
    }

    consume(TOKEN_RIGHT_BRACE, "Invalid map terminator.");
    return expression_count;
}

static void call(bool) {
    uint8_t argc = expression_list(TOKEN_RIGHT_PAREN);
    emit_bytes(OP_CALL, argc);
}

static void dot(bool can_assign) {
    consume(TOKEN_IDENTIFIER, "Expect property name after '.'.");
    uint8_t name = identifier_constant(&parser.previous);

    if (can_assign && match(TOKEN_EQUAL)) {
        expression();
        emit_bytes(OP_SET_PROPERTY, name);
    } else if (match(TOKEN_LEFT_PAREN)) {
        uint8_t argc = expression_list(TOKEN_RIGHT_PAREN);
        emit_bytes(OP_METHOD_INVOKE, name);
        emit_byte(argc);
    } else {
        emit_bytes(OP_GET_PROPERTY, name);
    }
}

static void literal(bool) {
    switch (parser.previous.type) {
        case TOKEN_FALSE:
            emit_byte(OP_FALSE);
            break;
        case TOKEN_NIL:
            emit_byte(OP_NIL);
            break;
        case TOKEN_TRUE:
            emit_byte(OP_TRUE);
            break;
        default:
            return;
    }
}

static void grouping(bool) {
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

static void list(bool) {
    uint8_t elem_count = expression_list(TOKEN_RIGHT_SQUARE);
    emit_bytes(OP_LIST, elem_count);
}

static void map(bool) {
    consume(TOKEN_LEFT_BRACE, "Expected '{' after map keyword.");
    uint8_t pair_count = map_pair_list();
    emit_bytes(OP_MAP, pair_count);
}

static void index_(bool can_assign) {
    expression();
    consume(TOKEN_RIGHT_SQUARE, "Expect ']' after index");

    if (can_assign && match(TOKEN_EQUAL)) {
        expression();
        emit_byte(OP_SET_INDEX);
    } else
        emit_byte(OP_GET_INDEX);
}

static void float_(bool) { emit_constant(AS_OBJ(new_float(strtod(parser.previous.start, NULL)))); }
static void char_(bool) { emit_constant(AS_OBJ(new_char(parser.previous.start[1]))); }
static void int_(bool) { emit_constant(AS_OBJ(new_int(strtol(parser.previous.start, NULL, 10)))); }

static void or (bool) {
    int else_jump = emit_jump(OP_JUMP_IF_FALSE);
    int end_jump = emit_jump(OP_JUMP);

    patch_jump(else_jump);
    emit_byte(OP_POP);

    parse_precedence(PREC_OR);
    patch_jump(end_jump);
}

static void and (bool) {
    int end_jump = emit_jump(OP_JUMP_IF_FALSE);

    emit_byte(OP_POP);
    parse_precedence(PREC_AND);

    patch_jump(end_jump);
}

static void string(bool) {
    char *escaped_str = calloc(parser.previous.length - 1, 1);
    int escaped_pos = 0;

    for (int i = 1; i < parser.previous.length - 1; i++) {
        if (parser.previous.start[i] == '\\') {
            switch (parser.previous.start[i + 1]) {
                case '\\':
                    escaped_str[escaped_pos] = '\\';
                    break;
                case '\'':
                    escaped_str[escaped_pos] = '\'';
                    break;
                case '"':
                    escaped_str[escaped_pos] = '"';
                    break;
                case '?':
                    escaped_str[escaped_pos] = '\?';
                    break;
                case 'a':
                    escaped_str[escaped_pos] = '\a';
                    break;
                case 'b':
                    escaped_str[escaped_pos] = '\b';
                    break;
                case 'f':
                    escaped_str[escaped_pos] = '\f';
                    break;
                case 'n':
                    escaped_str[escaped_pos] = '\n';
                    break;
                case 'r':
                    escaped_str[escaped_pos] = '\r';
                    break;
                case 't':
                    escaped_str[escaped_pos] = '\t';
                    break;
                case 'v':
                    escaped_str[escaped_pos] = '\v';
                    break;
                default:
                    free(escaped_str);
                    error("Invalid escape sequence in string");
                    return;
            }
            i++;
        } else {
            escaped_str[escaped_pos] = parser.previous.start[i];
        }

        escaped_pos++;
    }

    emit_constant(AS_OBJ(take_string(escaped_str, escaped_pos)));
}

static void unary(bool) {
    TokenType operator_type = parser.previous.type;

    parse_precedence(PREC_UNARY);

    switch (operator_type) {
        case TOKEN_BANG:
            emit_byte(OP_NOT);
            break;
        case TOKEN_MINUS:
            emit_byte(OP_NEGATE);
            break;
        default:
            return;
    }
}

static void named_variable(Token name, bool can_assign) {
    uint8_t get_op, set_op;
    int arg = resolve_local(current, &name);
    if (arg != -1) {
        get_op = OP_GET_LOCAL;
        set_op = OP_SET_LOCAL;
    } else if ((arg = resolve_upvalue(current, &name)) != -1) {
        get_op = OP_GET_UPVALUE;
        set_op = OP_SET_UPVALUE;
    } else {
        arg = identifier_constant(&name);
        get_op = OP_GET_GLOBAL;
        set_op = OP_SET_GLOBAL;
    }

    if (can_assign && match(TOKEN_EQUAL)) {
        expression();
        emit_bytes(set_op, (uint8_t)arg);
    } else {
        emit_bytes(get_op, (uint8_t)arg);
    }
}

static void variable(bool can_assign) { named_variable(parser.previous, can_assign); }

static Token synthetic_token(const char *text) {
    Token token;
    token.start = text;
    token.length = (int)strlen(text);
    return token;
}

static void super(bool) {
    if (current_class == NULL) {
        error("Can't use 'super' outside of a class.");
    } else if (!current_class->has_super_class) {
        error("Can't use 'super' in a class with no superclass.");
    }

    consume(TOKEN_DOT, "Expect '.' after 'super'.");
    consume(TOKEN_IDENTIFIER, "Expect superclass method name.");
    uint8_t name = identifier_constant(&parser.previous);

    named_variable(synthetic_token("this"), false);

    if (match(TOKEN_LEFT_PAREN)) {
        uint8_t argc = expression_list(TOKEN_RIGHT_PAREN);
        named_variable(synthetic_token("super"), false);
        emit_bytes(OP_SUPER_INVOKE, name);
        emit_byte(argc);
    } else {
        named_variable(synthetic_token("super"), false);
        emit_bytes(OP_GET_SUPER, name);
    }
}

static void this(bool) {
    if (current_class == NULL) {
        error("Can't use 'this' outside of a class.");
        return;
    } else if (current->type == TYPE_METHOD_STATIC) {
        error("Can't use 'this' inside static methods.");
        return;
    }

    variable(false);
}

static void scope(bool) {
    consume(TOKEN_IDENTIFIER, "Expect scoped member after '::'.");
    uint8_t name = identifier_constant(&parser.previous);

    if (match(TOKEN_EQUAL)) {
        error("Cannot set scoped member");
        return;
    } else if (match(TOKEN_LEFT_PAREN)) {
        uint8_t argc = expression_list(TOKEN_RIGHT_PAREN);
        emit_bytes(OP_SCOPE_INVOKE, name);
        emit_byte(argc);
    } else {
        emit_bytes(OP_GET_SCOPED, name);
    }
}

static void lambda(bool) { function(TYPE_FUNCTION); }

ParseRule rules[] = {
    [TOKEN_LEFT_PAREN] = {grouping, call, PREC_CALL},
    [TOKEN_RIGHT_PAREN] = {NULL, NULL, PREC_NONE},
    [TOKEN_LEFT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_RIGHT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_LEFT_SQUARE] = {list, index_, PREC_CALL},
    [TOKEN_RIGHT_SQUARE] = {NULL, NULL, PREC_NONE},
    [TOKEN_COMMA] = {NULL, NULL, PREC_NONE},
    [TOKEN_DOT] = {NULL, dot, PREC_CALL},
    [TOKEN_MINUS] = {unary, binary, PREC_TERM},
    [TOKEN_PLUS] = {NULL, binary, PREC_TERM},
    [TOKEN_SEMICOLON] = {NULL, NULL, PREC_NONE},
    [TOKEN_SLASH] = {NULL, binary, PREC_FACTOR},
    [TOKEN_STAR] = {NULL, binary, PREC_FACTOR},
    [TOKEN_AMPERSAND] = {NULL, binary, PREC_BINARY_AND},
    [TOKEN_PIPE] = {NULL, binary, PREC_BINARY_OR},
    [TOKEN_CARET] = {NULL, binary, PREC_BINARY_XOR},
    [TOKEN_GREATER_GREATER] = {NULL, binary, PREC_SHIFT},
    [TOKEN_LESS_LESS] = {NULL, binary, PREC_SHIFT},
    [TOKEN_COLON_COLON] = {NULL, scope, PREC_CALL},
    [TOKEN_BANG] = {unary, NULL, PREC_NONE},
    [TOKEN_BANG_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TOKEN_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_EQUAL_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TOKEN_GREATER] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_GREATER_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_LESS] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_LESS_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_IDENTIFIER] = {variable, NULL, PREC_NONE},
    [TOKEN_STRING] = {string, NULL, PREC_NONE},
    [TOKEN_INT] = {int_, NULL, PREC_NONE},
    [TOKEN_FLOAT] = {float_, NULL, PREC_NONE},
    [TOKEN_CHAR] = {char_, NULL, PREC_NONE},
    [TOKEN_AND] = {NULL, and, PREC_AND},
    [TOKEN_CLASS] = {NULL, NULL, PREC_NONE},
    [TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
    [TOKEN_FALSE] = {literal, NULL, PREC_NONE},
    [TOKEN_FOR] = {NULL, NULL, PREC_NONE},
    [TOKEN_FUN] = {lambda, NULL, PREC_NONE},
    [TOKEN_IF] = {NULL, NULL, PREC_NONE},
    [TOKEN_NIL] = {literal, NULL, PREC_NONE},
    [TOKEN_OR] = {NULL, or, PREC_OR},
    [TOKEN_RETURN] = {NULL, NULL, PREC_NONE},
    [TOKEN_SUPER] = {super, NULL, PREC_NONE},
    [TOKEN_THIS] = {this, NULL, PREC_NONE},
    [TOKEN_TRUE] = {literal, NULL, PREC_NONE},
    [TOKEN_VAR] = {NULL, NULL, PREC_NONE},
    [TOKEN_WHILE] = {NULL, NULL, PREC_NONE},
    [TOKEN_MAP] = {map, NULL, PREC_CALL},
    [TOKEN_IMPORT] = {NULL, NULL, PREC_NONE},
    [TOKEN_AS] = {NULL, NULL, PREC_NONE},
    [TOKEN_ERROR] = {NULL, NULL, PREC_NONE},
    [TOKEN_EOF] = {NULL, NULL, PREC_NONE},
};

static void parse_precedence(Precedence precedence) {
    advance();
    ParseFn prefix_rule = get_rule(parser.previous.type)->prefix;
    if (prefix_rule == NULL) {
        error("Expect expression.");
        return;
    }

    bool can_assign = precedence <= PREC_ASSIGNMENT;
    prefix_rule(can_assign);

    while (precedence <= get_rule(parser.current.type)->precedence) {
        advance();
        ParseFn infix_rule = get_rule(parser.previous.type)->infix;
        infix_rule(can_assign);
    }

    if (can_assign && match(TOKEN_EQUAL)) {
        error("Invalid assignment target.");
    }
}

static uint8_t parse_variable(const char *error_message) {
    consume(TOKEN_IDENTIFIER, error_message);

    declare_variable();
    if (current->scope_depth > 0)
        return 0;

    return identifier_constant(&parser.previous);
}

static void mark_initialized() {
    if (current->scope_depth == 0)
        return;

    current->locals[current->local_count - 1].depth = current->scope_depth;
}

static void define_variable(uint8_t global) {
    if (current->scope_depth > 0) {
        mark_initialized();
        return;
    }

    emit_bytes(OP_DEFINE_GLOBAL, global);
}

static ParseRule *get_rule(TokenType type) { return &rules[type]; }

static void expression() { parse_precedence(PREC_ASSIGNMENT); }

static void block() {
    while (!check(TOKEN_RIGHT_BRACE) && !check(TOKEN_EOF)) {
        declaration();
    }

    consume(TOKEN_RIGHT_BRACE, "Expect '}' after block.");
}

static void function(FunctionType type) {
    Compiler compiler;
    init_compiler(&compiler, type);
    begin_scope();

    consume(TOKEN_LEFT_PAREN, "Expect '(' after function name.");
    if (!check(TOKEN_RIGHT_PAREN)) {
        do {
            current->function->arity++;
            if (current->function->arity > 255) {
                error_at_current("Can't have more than 255 parameters.");
            }
            uint8_t constant = parse_variable("Expect parameter name.");
            define_variable(constant);
        } while (match(TOKEN_COMMA));
    }
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after parameters.");
    consume(TOKEN_LEFT_BRACE, "Expect '{' before function body.");
    block();

    ObjFunction *function = end_compiler(false);
    emit_bytes(OP_CLOSURE, make_constant(AS_OBJ(function)));

    for (int i = 0; i < function->upvalue_count; i++) {
        emit_bytes(compiler.upvalues[i].is_local ? 1 : 0, compiler.upvalues[i].index);
    }
}

static void method(bool is_static) {
    OpCode op_code;
    FunctionType type;

    if (is_static) {
        op_code = OP_STATIC_METHOD;
        type = TYPE_METHOD_STATIC;
    } else {
        op_code = OP_METHOD;
        type = TYPE_METHOD;
    }

    consume(TOKEN_IDENTIFIER, "Expect method name.");
    uint8_t constant = identifier_constant(&parser.previous);

    if (parser.previous.length == 4 && memcmp(parser.previous.start, "init", 4) == 0) {
        type = TYPE_INITIALIZER;
    }

    function(type);
    emit_bytes(op_code, constant);
}

static void field() {
    consume(TOKEN_IDENTIFIER, "Expect field name.");
    uint8_t constant = identifier_constant(&parser.previous);

    if (match(TOKEN_EQUAL)) {
        expression();
    } else {
        emit_byte(OP_NIL);
    }

    consume(TOKEN_SEMICOLON, "Expect ';' after field declaration.");
    emit_bytes(OP_SET_PROPERTY, constant);
}

static void class_declaration() {
    consume(TOKEN_IDENTIFIER, "Expect class name.");
    Token class_name = parser.previous;
    uint8_t name_constant = identifier_constant(&parser.previous);
    declare_variable();

    emit_bytes(OP_CLASS, name_constant);
    define_variable(name_constant);

    ClassCompiler class_compiler;
    class_compiler.has_super_class = false;

    class_compiler.enclosing = current_class;
    current_class = &class_compiler;

    if (match(TOKEN_LESS)) {
        consume(TOKEN_IDENTIFIER, "Expect superclass name.");
        variable(false);

        if (identifiers_equal(&class_name, &parser.previous)) {
            error("A class can't inherit from itself.");
        }

        begin_scope();
        add_local(synthetic_token("super"));
        define_variable(0);

        named_variable(class_name, false);
        emit_byte(OP_INHERIT);
        class_compiler.has_super_class = true;
    }

    named_variable(class_name, false);
    consume(TOKEN_LEFT_BRACE, "Expect '{' before class body.");

    while (!check(TOKEN_RIGHT_BRACE) && !check(TOKEN_EOF)) {
        if (match(TOKEN_FUN)) {
            method(false);
        } else if (match(TOKEN_STATIC) && match(TOKEN_FUN)) {
            method(true);
        } else if (match(TOKEN_VAR)) {
            field();
        }
    }

    consume(TOKEN_RIGHT_BRACE, "Expect '}' after class body.");
    emit_byte(OP_POP);

    if (class_compiler.has_super_class) {
        end_scope();
    }

    current_class = current_class->enclosing;
}

static void fun_declaration() {
    uint8_t global = parse_variable("Expect function name.");
    mark_initialized();
    function(TYPE_FUNCTION);
    define_variable(global);
}

static void var_declaration() {
    uint8_t global = parse_variable("Expect variable name.");

    if (match(TOKEN_EQUAL)) {
        expression();
    } else {
        emit_byte(OP_NIL);
    }

    consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");
    define_variable(global);
}

static void expression_statement() {
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
    emit_byte(OP_POP);
}

static void for_statement() {
    begin_scope();
    consume(TOKEN_LEFT_PAREN, "Expect '(' after 'for'.");

    if (match(TOKEN_SEMICOLON)) {
    } else if (match(TOKEN_VAR)) {
        var_declaration();
    } else {
        expression_statement();
    }

    int loop_start = current_chunk()->count;

    int exit_jump = -1;
    if (!match(TOKEN_SEMICOLON)) {
        expression();
        consume(TOKEN_SEMICOLON, "Expect ';' after loop condition.");

        exit_jump = emit_jump(OP_JUMP_IF_FALSE);
        emit_byte(OP_POP);
    }

    if (!match(TOKEN_RIGHT_PAREN)) {
        int body_jump = emit_jump(OP_JUMP);
        int increment_start = current_chunk()->count;
        expression();
        emit_byte(OP_POP);
        consume(TOKEN_RIGHT_PAREN, "Expect ')' after for clauses.");

        emit_loop(loop_start);
        loop_start = increment_start;
        patch_jump(body_jump);
    }

    current->in_loop = true;

    statement();
    emit_loop(loop_start);

    if (exit_jump != -1) {
        for (int i = 0; i < current->breaks.count; i++) {
            patch_jump(current->breaks.values[i]);
        }

        for (int i = 0; i < current->continues.count; i++) {
            patch_jump_to(current->continues.values[i], loop_start);
        }

        patch_jump(exit_jump);
        emit_byte(OP_POP);
    }

    end_scope();
}

static void if_statement() {
    consume(TOKEN_LEFT_PAREN, "Expect '(' after 'if'.");
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");

    int then_jump = emit_jump(OP_JUMP_IF_FALSE);
    emit_byte(OP_POP);
    statement();

    int else_jump = emit_jump(OP_JUMP);

    patch_jump(then_jump);
    emit_byte(OP_POP);

    if (match(TOKEN_ELSE))
        statement();

    patch_jump(else_jump);
}

static void return_statement() {
    if (current->type == TYPE_SCRIPT) {
        error("Can't return from top-level code.");
    }

    if (match(TOKEN_SEMICOLON)) {
        emit_return();
    } else {
        if (current->type == TYPE_INITIALIZER) {
            error("Can't return a value from an initializer.");
        }

        expression();
        consume(TOKEN_SEMICOLON, "Expect ';' after return value.");
        emit_byte(OP_RETURN);
    }
}

static void while_statement() {
    int loop_start = current_chunk()->count;

    consume(TOKEN_LEFT_PAREN, "Expect '(' after 'while'.");
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");

    int exit_jump = emit_jump(OP_JUMP_IF_FALSE);

    current->in_loop = true;

    emit_byte(OP_POP);
    statement();

    emit_loop(loop_start);

    patch_jump(exit_jump);

    for (int i = 0; i < current->breaks.count; i++) {
        patch_jump(current->breaks.values[i]);
    }

    for (int i = 0; i < current->continues.count; i++) {
        patch_jump_to(current->continues.values[i], loop_start);
    }

    emit_byte(OP_POP);
}

static void break_statement() {
    if (!current->in_loop)
        error("Cannot use 'break' outside of loop.");

    consume(TOKEN_SEMICOLON, "Expect ';' after 'break'.");

    write_offset_array(&current->breaks, emit_jump(OP_JUMP));
}

static void continue_statement() {
    if (!current->in_loop)
        error("Cannot use 'continue' outside of loop.");

    consume(TOKEN_SEMICOLON, "Expect ';' after 'continue'.");

    write_offset_array(&current->continues, emit_jump(OP_LOOP));
}

static void import_statement() {
    consume(TOKEN_STRING, "Expect import path after 'import'.");

    ObjString *import_path = new_string(parser.previous.start + 1, parser.previous.length - 2);

    emit_bytes(OP_IMPORT, make_constant(AS_OBJ(import_path)));

    consume(TOKEN_AS, "Expect 'as' after import path");

    uint8_t global = parse_variable("Expect alias after 'as'.");
    define_variable(global);

    consume(TOKEN_SEMICOLON, "Expect ';' after import.");
}

static void synchronize() {
    parser.panic_mode = false;
    while (parser.current.type != TOKEN_EOF) {
        if (parser.previous.type == TOKEN_SEMICOLON)
            return;
        switch (parser.current.type) {
            case TOKEN_CLASS:
            case TOKEN_FUN:
            case TOKEN_VAR:
            case TOKEN_FOR:
            case TOKEN_IF:
            case TOKEN_WHILE:
            case TOKEN_RETURN:
            case TOKEN_IMPORT:
                return;
            default:;
        }

        advance();
    }
}

static void declaration() {
    if (match(TOKEN_FUN)) {
        fun_declaration();
    } else if (match(TOKEN_CLASS)) {
        class_declaration();
    } else if (match(TOKEN_VAR)) {
        var_declaration();
    } else {
        statement();
    }

    if (parser.panic_mode)
        synchronize();
}

static void statement() {
    if (match(TOKEN_IF)) {
        if_statement();
    } else if (match(TOKEN_RETURN)) {
        return_statement();
    } else if (match(TOKEN_WHILE)) {
        while_statement();
    } else if (match(TOKEN_FOR)) {
        for_statement();
    } else if (match(TOKEN_IMPORT)) {
        import_statement();
    } else if (match(TOKEN_BREAK)) {
        break_statement();
    } else if (match(TOKEN_CONTINUE)) {
        continue_statement();
    } else if (match(TOKEN_LEFT_BRACE)) {
        begin_scope();
        block();
        end_scope();
    } else {
        expression_statement();
    }
}

ObjFunction *compile(const char *source) {
    init_scanner(source);
    Compiler compiler;
    init_compiler(&compiler, TYPE_SCRIPT);

    parser.had_error = false;
    parser.panic_mode = false;

    advance();

    while (!match(TOKEN_EOF)) {
        declaration();
    }

    ObjFunction *function = end_compiler(true);
    return parser.had_error ? NULL : function;
}

void mark_compiler_roots() {
    Compiler *compiler = current;
    while (compiler != NULL) {
        mark_object(AS_OBJ(compiler->function));
        compiler = compiler->enclosing;
    }
}
