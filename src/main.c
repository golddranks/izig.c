#include <stdio.h>
#include <errno.h>
#include "array.h"
#include "input_file.h"
#include "str.h"
#include "lexer.h"
#include "parser.h"

typedef struct Ipreter {
    Lexer* lexer;
    Parser* parser;
} Ipreter;

Ipreter Ipreter_new(Lexer* lexer, Parser* parser) {
    return (Ipreter) {
        .lexer = lexer,
        .parser = parser,
    };
}

void IPreter_call_fn(Ipreter* ipreter, Fn fn) {
    Stmt* stmt = StmtArray_get(ipreter->parser->stmts, fn.body)
}

void interpret(Lexer* lexer, Parser* parser) {
    Ipreter ipreter = Ipreter_new(lexer, parser);
    Fn fn = Parser_get_fn(parser, "main");
    Ipreter_call_fn(&ipreter, fn);
}

int main() {
    InputFile hello = InputFile_open("hello.zig");

    Str hello_str = InputFile_Str(hello);

    Str_print(hello_str);

    Lexer lexer = Lexer_new(hello_str);
    Parser parser = parse(&lexer);
    Lexer_print(&lexer);

    interpret(&lexer, &parser);

    InputFile_close(hello);

    return 0;
}
