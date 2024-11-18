#include "utils.h"

#include "lexer.h"
#include "parser.h"
#include "runner.h"

int main(int argc, char** argv) {
    bool verbose = false;
    char* filename = NULL;

    if (argc == 3 && argv[1][0] == '-' && argv[1][1] == 'v' && argv[1][2] == '\0') {
        verbose = true;
        filename = argv[2];
    } else if (argc == 2 && argv[1][0] != '-') {
        filename = argv[1];
    } else {
        fprintf(stderr, "Usage: izig [-v] <filename>\n");
        exit(EXIT_FAILURE);
    }

    InputFile input = InputFile_open(filename);

    Str input_str = InputFile_Str(input);

    if (verbose) {
        fprintf(stderr, "Input:\n");
        Str_print(input_str);
    }

    Lexer lexer = Lexer_new(input_str);
    Parser parser = parse(&lexer);

    if (verbose) {
        Lexer_print(&lexer);
        Parser_print(&parser);
    }

    run(&lexer, &parser, verbose);

    InputFile_close(input);

    return 0;
}
