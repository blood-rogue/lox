#include <stdio.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

typedef struct {
    const char *start;
    const char *current;
    int line;
} Scanner;

Scanner scanner;

void init_scanner(const char *source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

static bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool is_digit(char c) { return c >= '0' && c <= '9'; }

static bool is_eof() { return *scanner.current == '\0'; }

static char advance() {
    scanner.current++;
    return scanner.current[-1];
}

static char peek() { return *scanner.current; }

static char peek_next() {
    if (is_eof())
        return '\0';
    return scanner.current[1];
}

static bool match(char expected) {
    if (is_eof())
        return false;

    if (*scanner.current != expected)
        return false;

    scanner.current++;
    return true;
}

static Token make_token(TokenType type) {
    Token token;

    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;

    return token;
}

static Token error_token(const char *message) {
    Token token;

    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = scanner.line;

    return token;
}

static void skip_whitespace() {
    for (;;) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                scanner.line++;
                advance();
                break;
            case '#':
                while (peek() != '\n' && !is_eof())
                    advance();
                break;
            default:
                return;
        }
    }
}

static TokenType check_keyword(int start, int length, const char *rest, TokenType type) {
    if (scanner.current - scanner.start == start + length &&
        memcmp(scanner.start + start, rest, length) == 0) {
        return type;
    }

    return TOKEN_IDENTIFIER;
}

static TokenType identifier_type() {
    switch (scanner.start[0]) {
        case 'a':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'n':
                        return check_keyword(2, 1, "d", TOKEN_AND);
                    case 's':
                        return check_keyword(2, 0, "", TOKEN_AS);
                }
            }
            break;
        case 'b':
            return check_keyword(1, 4, "reak", TOKEN_BREAK);
        case 'c':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'l':
                        return check_keyword(2, 3, "ass", TOKEN_CLASS);
                    case 'o':
                        return check_keyword(2, 6, "ntinue", TOKEN_CONTINUE);
                }
            }
            break;
        case 'e':
            return check_keyword(1, 3, "lse", TOKEN_ELSE);
        case 'f':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'a':
                        return check_keyword(2, 3, "lse", TOKEN_FALSE);
                    case 'o':
                        return check_keyword(2, 1, "r", TOKEN_FOR);
                    case 'u':
                        return check_keyword(2, 1, "n", TOKEN_FUN);
                }
            }
            break;
        case 'i':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'f':
                        return check_keyword(2, 0, "", TOKEN_IF);
                    case 'm':
                        return check_keyword(2, 4, "port", TOKEN_IMPORT);
                }
            }
            break;
        case 'n':
            return check_keyword(1, 2, "il", TOKEN_NIL);
        case 'o':
            return check_keyword(1, 1, "r", TOKEN_OR);
        case 'r':
            return check_keyword(1, 5, "eturn", TOKEN_RETURN);
        case 's':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 't':
                        return check_keyword(2, 4, "atic", TOKEN_STATIC);
                    case 'u':
                        return check_keyword(2, 3, "per", TOKEN_SUPER);
                }
            }
            break;
        case 't':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'h':
                        return check_keyword(2, 2, "is", TOKEN_THIS);
                    case 'r':
                        return check_keyword(2, 2, "ue", TOKEN_TRUE);
                }
            }
            break;
        case 'v':
            return check_keyword(1, 2, "ar", TOKEN_VAR);
        case 'w':
            return check_keyword(1, 4, "hile", TOKEN_WHILE);
    }

    return TOKEN_IDENTIFIER;
}

static Token identifier() {
    while (is_alpha(peek()) || is_digit(peek()))
        advance();
    return make_token(identifier_type());
}

static Token number() {
    while (is_digit(peek()))
        advance();

    if (peek() == '.' && is_digit(peek_next())) {
        advance();

        while (is_digit(peek()))
            advance();
    } else {
        return make_token(TOKEN_INT);
    }

    return make_token(TOKEN_FLOAT);
}

static Token string() {
    while (peek() != '"' && !is_eof()) {
        if (peek() == '\n')
            scanner.line++;
        advance();
    }

    if (is_eof())
        return error_token("Unterminated string.");

    advance();
    switch (peek()) {
        case 'b':
            advance();
            return make_token(TOKEN_BYTES);
        case 'r':
            advance();
            return make_token(TOKEN_RAW);
        default:
            return make_token(TOKEN_STRING);
    }
}

static Token character() {
    while (peek() != '\'' && !is_eof()) {
        if (peek() == '\n')
            return error_token("Invalid character literal.");
        advance();
    }

    if (is_eof())
        return error_token("Unterminated char literal.");

    advance();
    return make_token(TOKEN_CHAR);
}

Token scan_token() {
    skip_whitespace();
    scanner.start = scanner.current;

    if (is_eof())
        return make_token(TOKEN_EOF);

    char c = advance();

    if (is_digit(c))
        return number();

    if (is_alpha(c))
        return identifier();

    switch (c) {
        case '(':
            return make_token(TOKEN_LEFT_PAREN);
        case ')':
            return make_token(TOKEN_RIGHT_PAREN);
        case '{':
            return make_token(TOKEN_LEFT_BRACE);
        case '}':
            return make_token(TOKEN_RIGHT_BRACE);
        case '[':
            return make_token(TOKEN_LEFT_SQUARE);
        case ']':
            return make_token(TOKEN_RIGHT_SQUARE);
        case ':':
            return make_token(match(':') ? TOKEN_COLON_COLON : TOKEN_COLON);
        case ';':
            return make_token(TOKEN_SEMICOLON);
        case ',':
            return make_token(TOKEN_COMMA);
        case '.':
            return make_token(TOKEN_DOT);
        case '-':
            return make_token(TOKEN_MINUS);
        case '+':
            return make_token(TOKEN_PLUS);
        case '/':
            return make_token(TOKEN_SLASH);
        case '*':
            return make_token(TOKEN_STAR);
        case '!':
            return make_token(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=':
            return make_token(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '<':
            if (match('='))
                return make_token(TOKEN_LESS_EQUAL);
            else if (match('<'))
                return make_token(TOKEN_LESS_LESS);
            else
                return make_token(TOKEN_LESS);
        case '>':
            if (match('='))
                return make_token(TOKEN_GREATER_EQUAL);
            else if (match('>'))
                return make_token(TOKEN_GREATER_GREATER);
            else
                return make_token(TOKEN_GREATER);
        case '|':
            return make_token(TOKEN_PIPE);
        case '&':
            return make_token(TOKEN_AMPERSAND);
        case '^':
            return make_token(TOKEN_CARET);
        case '~':
            return make_token(TOKEN_TILDE);
        case '"':
            return string();
        case '\'':
            return character();
    }

    return error_token("Unexpected character.");
}
