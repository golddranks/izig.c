
#include <stdio.h>
#include "utils.h"

enum Tk {
    TK_CONST,
    TK_NAME,
    TK_ASSIGN,
    TK_IMPORT,
    TK_STR,
    TK_PUB,
    TK_FN,
};

typedef struct TokenStream {
    enum Tk kind;
    str value;
} Token;

void parse(str source) {
    str* s = &source;
    while(s->len > 0) {
        str_consume_ws(s);
        if(s->len == 0) {
            break;
        }
        if(str_consume_str(s, "const") && s->len > 0) {
            printf("const\n");
        } else if(str_consume_str(s, "pub") && s->len > 0) {
            printf("pub\n");
        } else if(str_consume_str(s, "fn") && s->len > 0) {
            printf("fn\n");
        } else if(str_consume_str(s, "@import") && s->len > 0) {
            printf("import\n");
        } else {
            str_consume_until(s, ' ');
        }
    }
}

int main() {
    ifile hello = ifile_open("hello.zig");

    str_print(ifile_str(hello));



    ifile_close(hello);

    printf("EOF\n");

    return 0;
}
