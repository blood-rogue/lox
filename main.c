#include <readline/history.h>
#include <readline/readline.h>

#include "chunk.h"
#include "common.h"
#include "completion.h"
#include "vm.h"

int _argc;
const char **_argv;
char *_source;

static void repl() {
    while ((_source = readline(">> ")) != NULL) {
        if (*_source)
            add_history(_source);

        interpret(_source);
    }

    free_vm();
}

static char *read_file(const char *path) {
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    fseek(file, 0L, SEEK_END);
    size_t file_size = (size_t)ftell(file);
    rewind(file);

    char *buffer = malloc(file_size + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);
    }

    size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
    if (bytes_read < file_size) {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(74);
    }

    buffer[bytes_read] = '\0';

    fclose(file);
    return buffer;
}

static void run_file(const char *path) {
    char *source = read_file(path);
    _source = source;
    InterpretResult result = interpret(source);

    free_vm();

    if (result == INTERPRET_COMPILE_ERROR)
        exit(65);
    if (result == INTERPRET_RUNTIME_ERROR)
        exit(70);
}

int main(int argc, const char **argv) {
    init_vm();

    _argc = argc - 1;
    _argv = argv + 1;

    if (argc == 1) {
        rl_attempted_completion_function = completer;
        repl();
    } else
        run_file(argv[1]);

    return 0;
}
