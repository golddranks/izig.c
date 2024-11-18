#include "utils.h"

#include "lexer.h"
#include "parser.h"
#include "runner.h"

int main() {
    InputFile hello = InputFile_open("hello.zig");

    Str hello_str = InputFile_Str(hello);

    Str_print(hello_str);

    Lexer lexer = Lexer_new(hello_str);
    Parser parser = parse(&lexer);
    Lexer_print(&lexer);
    Parser_print(&parser);

    run(&lexer, &parser);

    InputFile_close(hello);

    return 0;
}
