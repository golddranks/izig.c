
#include <stdio.h>
#include "utils.h"

typedef enum TKind {
    TK_CONST,
    TK_IDENT,
    TK_ASSIGN,
    TK_DOT,
    TK_LPAREN,
    TK_RPAREN,
    TK_IMPORT, // 6
    TK_STR,
    TK_PUB,
    TK_FN,
    TK_EOS,
    TK_EXCLAM,
    TK_LBRACE,
    TK_RBRACE,
    TK_COMMA,
    TK_TRY,
} TKind;

typedef struct Token {
    TKind kind;
    short id;
} Token;

char* Token_str(Token token) {
    return (char*[]){
        "const",
        "ident",
        "=",
        ".",
        "(",
        ")",
        "@import",
        "str",
        "pub",
        "fn",
        ";",
        "!",
        "{",
        "}",
        ",",
        "try",
    }[token.kind];
}

DefArray(TokenArray, Token, 1000)
DefArray(IdentArray, Str, 1000)
DefArray(StringArray, Str, 1000)

typedef struct ParseCtx {
    TokenArray tokens;
    IdentArray idents;
    StringArray strings;
} ParseCtx;

ParseCtx ParseCtx_new() {
    return (ParseCtx) {
        .tokens = TokenArray_new(),
        .idents = IdentArray_new(),
        .strings = StringArray_new(),
    };
}

void ParseCtx_print(ParseCtx ctx) {
    printf("TokenArray: ");
    for (int i = 0; i < ctx.tokens.len; i++) {
        char* token_str = Token_str(*TokenArray_get(ctx.tokens, i));
        printf("%s ", token_str);
    }
    printf("\n");
}

void ParseCtx__push(ParseCtx* ctx, TKind tkind, short id) {
    TokenArray_push(&ctx->tokens, (Token) {
        .kind = tkind,
        .id = id,
    });
}
void ParseCtx_push(ParseCtx* ctx, TKind tkind) {
    ParseCtx__push(ctx, tkind, -1);
}

void ParseCtx_push_ident(ParseCtx* ctx, Str ident) {
    size_t idx = IdentArray_push(&ctx->idents, ident);
    ParseCtx__push(ctx, TK_IDENT, idx);
}

void ParseCtx_push_str(ParseCtx* ctx, Str str) {
    size_t idx = StringArray_push(&ctx->strings, str);
    ParseCtx__push(ctx, TK_STR, idx);
}

void parse(Str source, ParseCtx* ctx) {
    Str* s = &source;
    while(s->len > 0) {
        ParseCtx_print(*ctx);
        Str_consume_ws(s);
        if (s->len == 0) {
            break;
        }
        printf("%lu\n", s->len);

        if (Str_consume(s, "const") && s->len > 0) {
            ParseCtx_push(ctx, TK_CONST);
            Str_consume_ws(s);
            Str ident = Str_consume_ident(s);
            ParseCtx_push_ident(ctx, ident);
        } else if (Str_consume(s, "=") && s->len > 0) {
            ParseCtx_push(ctx, TK_ASSIGN);
        } else if (Str_consume(s, ".") && s->len > 0) {
            ParseCtx_push(ctx, TK_DOT);
        } else if (Str_consume(s, "(") && s->len > 0) {
            ParseCtx_push(ctx, TK_LPAREN);
        } else if (Str_consume(s, ")") && s->len > 0) {
            ParseCtx_push(ctx, TK_RPAREN);
        } else if (Str_consume(s, "{") && s->len > 0) {
            ParseCtx_push(ctx, TK_LBRACE);
        } else if (Str_consume(s, "}") && s->len > 0) {
            ParseCtx_push(ctx, TK_RBRACE);
        } else if (Str_peek(s, "\"") && s->len > 0) {
            Str str = Str_consume_str(s);
            ParseCtx_push_str(ctx, str);
        } else if (Str_consume(s, ";") && s->len > 0) {
            ParseCtx_push(ctx, TK_EOS);
        } else if (Str_consume(s, "!") && s->len > 0) {
            ParseCtx_push(ctx, TK_EXCLAM);
        } else if (Str_consume(s, ",") && s->len > 0) {
            ParseCtx_push(ctx, TK_COMMA);
        } else if (Str_consume(s, "pub") && s->len > 0) {
            ParseCtx_push(ctx, TK_PUB);
        } else if (Str_consume(s, "fn") && s->len > 0) {
            ParseCtx_push(ctx, TK_FN);
        } else if (Str_consume(s, "@import") && s->len > 0) {
            ParseCtx_push(ctx, TK_IMPORT);
        } else if (Str_consume(s, "try") && s->len > 0) {
            ParseCtx_push(ctx, TK_TRY);
        } else if (!Str_is_null(Str_consume_ident(s))) {
            Str ident = Str_consume_ident(s);
            ParseCtx_push_ident(ctx, ident);
        } else {
            perror("unknown token!");
            exit(5);
        }
    }
}

int main() {
    InputFile hello = InputFile_open("hello.zig");

    Str hello_str = InputFile_Str(hello);

    Str_print(hello_str);

    ParseCtx ctx = ParseCtx_new();
    parse(hello_str, &ctx);

    InputFile_close(hello);

    printf("EOF\n");

    return 0;
}
