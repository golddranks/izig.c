
#include <stdio.h>
#include <errno.h>
#include "array.h"
#include "input_file.h"
#include "str.h"
#include "lexer.h"
#include "parser.h"

typedef struct InterpretCtx {
    int dummy;
} InterpretCtx;

InterpretCtx InterpretCtx_new() {
    return (InterpretCtx) { .dummy = 0 };
}

void interpret(Parser* parser, InterpretCtx* interpret_ctx) {
    Fn fn = Parser_get_fn(parser, "main");
}

int main() {
    InputFile hello = InputFile_open("hello.zig");

    Str hello_str = InputFile_Str(hello);

    Str_print(hello_str);

    Lexer lexer = Lexer_new(hello_str);
    Parser parse_ctx = parse(&lexer);
    Lexer_print(&lexer);

    InterpretCtx interpret_ctx = InterpretCtx_new();
    interpret(&parse_ctx, &interpret_ctx);

    InputFile_close(hello);

    return 0;
}
