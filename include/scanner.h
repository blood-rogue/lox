#ifndef lox_scanner_h
#define lox_scanner_h

typedef enum {
    TOKEN_LEFT_PAREN,      // (
    TOKEN_RIGHT_PAREN,     // )
    TOKEN_LEFT_BRACE,      // {
    TOKEN_RIGHT_BRACE,     // }
    TOKEN_LEFT_SQUARE,     // [
    TOKEN_RIGHT_SQUARE,    // ]
    TOKEN_COMMA,           // ,
    TOKEN_DOT,             // .
    TOKEN_MINUS,           // -
    TOKEN_PLUS,            // +
    TOKEN_COLON,           // :
    TOKEN_SEMICOLON,       // ;
    TOKEN_SLASH,           // /
    TOKEN_STAR,            // *
    TOKEN_COLON_COLON,     // ::
    TOKEN_PIPE,            // |
    TOKEN_AMPERSAND,       // &
    TOKEN_CARET,           // ^
    TOKEN_GREATER_GREATER, // >>
    TOKEN_LESS_LESS,       // <<

    TOKEN_BANG,          // !
    TOKEN_BANG_EQUAL,    // !=
    TOKEN_EQUAL,         // =
    TOKEN_EQUAL_EQUAL,   // ==
    TOKEN_GREATER,       // >
    TOKEN_GREATER_EQUAL, // >=
    TOKEN_LESS,          // <
    TOKEN_LESS_EQUAL,    // <=

    TOKEN_AND,      // 'and'
    TOKEN_CLASS,    // 'class'
    TOKEN_ELSE,     // 'else'
    TOKEN_FALSE,    // 'false'
    TOKEN_FOR,      // 'for'
    TOKEN_FUN,      // 'fun'
    TOKEN_IF,       // 'if'
    TOKEN_NIL,      // 'nil'
    TOKEN_OR,       // 'or'
    TOKEN_RETURN,   // 'return'
    TOKEN_SUPER,    // 'super'
    TOKEN_THIS,     // 'this'
    TOKEN_TRUE,     // 'true'
    TOKEN_VAR,      // 'var'
    TOKEN_WHILE,    // 'while'
    TOKEN_MAP,      // 'map'
    TOKEN_STATIC,   // 'static'
    TOKEN_IMPORT,   // 'import'
    TOKEN_AS,       // 'as'
    TOKEN_BREAK,    // 'break'
    TOKEN_CONTINUE, // 'continue'

    TOKEN_IDENTIFIER, // [_a-zA-Z][\w\d_]*
    TOKEN_STRING,     // ".*"
    TOKEN_INT,        // \d+
    TOKEN_FLOAT,      // \d+\.\d+
    TOKEN_CHAR,       // '.*'

    TOKEN_ERROR,
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    const char *start;
    int length;
    int line;
} Token;

void init_scanner(const char *);
Token scan_token();

#endif // lox_scanner_h
