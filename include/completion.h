#ifndef lox_completion_h
#define lox_completion_h

#include <readline/readline.h>
#include <string.h>

#include "common.h"

char *completions[] = {
    "and",    "class",    "else",       "false",       "for",      "fun",       "if",    "nil",
    "or",     "return",   "super",      "this",        "true",     "var",       "while", "map",
    "static", "import",   "as",         "break",       "continue", "exit",      "print", "input",
    "argv",   "run_gc",   "parse_int",  "parse_float", "sleep",    "type",      "repr",  "std/math",
    "std/fs", "std/time", "std/random", "std/process", "std/sys",  "std/regex", NULL};

static char *dupstr(char *s) {
    char *r = malloc(strlen(s) + 1);
    strcpy(r, s);
    return (r);
}

static char *completer_callback(const char *text, int state) {
    static int list_index, len;
    char *option;

    if (state == 0) {
        list_index = 0;
        len = strlen(text);
    }

    while ((option = completions[list_index++])) {
        if (strncmp(option, text, len) == 0) {
            return dupstr(option);
        }
    }

    return NULL;
}

static char **completer(const char *text, UNUSED(int start), UNUSED(int end)) {
    char **matches = NULL;
    rl_completion_append_character = '\0';
    matches = rl_completion_matches(text, completer_callback);

    if (matches == NULL) {
        matches = rl_completion_matches(text, rl_filename_completion_function);
    }

    return matches;
}

#endif // lox_completion_h
