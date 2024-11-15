#ifndef IZIGC_LEXER_H
#define IZIGC_LEXER_H

#include "str.h"
#include "array.h"

typedef enum TKind {
    TK_CONST,
    TK_IDENT,
    TK_ASSIGN,
    TK_DOT,
    TK_LPAREN,
    TK_RPAREN,
    TK_AT,
    TK_STR,
    TK_PUB,
    TK_FN,
    TK_EOS,
    TK_EXCLAM,
    TK_LBRACE,
    TK_RBRACE,
    TK_COMMA,
    TK_TRY,
    TK_DQUOTE,
} TKind;

Str TKind_str(TKind kind) {
    char* c_str = (char*[]){
        "const",
        "ident",
        "=",
        ".",
        "(",
        ")",
        "@",
        "str",
        "pub",
        "fn",
        ";",
        "!",
        "{",
        "}",
        ",",
        "try",
        "\"",
    }[kind];
    return Str_new(c_str);
}

typedef union TokenData {
    struct Empty {} empty;
    Str ident;
    Str str;
} TokenData;

const TokenData EMPTY_TK_DATA = (TokenData) { {} };

typedef struct Token {
    TKind kind;
    TokenData data;
} Token;

Token Token_new(TKind kind, TokenData data) {
    return (Token) {
        .kind = kind,
        .data = data,
    };
}

DefArray(TokenArray, Token, 1000)

typedef struct Lexer {
    int consumed;
    Str source;
    TokenArray tokens;
    int nesting;
} Lexer;

Lexer Lexer_new(Str source) {
    return (Lexer) {
        .consumed = 0,
        .source = source,
        .tokens = TokenArray_new(),
        .nesting = 0,
    };
}

Str Token_str(Token token, Lexer* lexer) {
    if (token.kind == TK_IDENT) {
        return token.data.ident;
    } else if (token.kind == TK_STR) {
        return token.data.str;
    }
    return TKind_str(token.kind);
}

void Lexer_print(Lexer* lexer) {
    printf("Currently consumed: %d\n", lexer->consumed);
    printf("TokenArray: ");
    for (int i = 0; i < lexer->tokens.len; i++) {
        Str token = Token_str(*TokenArray_get(lexer->tokens, TokenArray_idx(i)), lexer);
        Str_print(token);
        printf(" ");
    }
    printf("\n");
}

void report_error(Lexer* lexer, const char* msg, Str expected, const char* file, int line) {
    fprintf(
        stderr,
        "Error: %s; expected: %.*s, got: %c (at %s:%d)\n",
        msg,
        (int) expected.len,
        expected.start,
        lexer->source.start[0],
        file,
        line
    );
    Lexer_print(lexer);
}

#define lex_error(lexer, msg, expected) report_error(lexer, msg, expected, __FILE__, __LINE__); exit(EXIT_FAILURE);

void Lexer_consume(Lexer* lexer, size_t n) {
    lexer->consumed += n;
    Str_consume_n(&lexer->source, n);
}

void Lexer_ws(Lexer* lexer) {
    size_t i = 0;
    while (i < lexer->source.len && is_ws(lexer->source.start[i])) {
        i++;
    }
    Lexer_consume(lexer, i);
}

Str Lexer_consume_until(Lexer* lexer, char c) {
    size_t i = 0;
    while (i < lexer->source.len && lexer->source.start[i] != c) {
        i++;
    }
    Str consumed = lexer->source;
    consumed.len = i;
    Lexer_consume(lexer, i);
    return consumed;
}

void Lexer_token(Lexer* lexer, TKind tkind) {
    Lexer_ws(lexer);
    size_t i = Str_prefix_match(lexer->source, TKind_str(tkind));
    if (i != -1) {
        TokenArray_push(&lexer->tokens, Token_new(tkind, EMPTY_TK_DATA));
        Lexer_consume(lexer, i);
    } else {
        lex_error(lexer, "expected token", TKind_str(tkind));
    }
}

bool Lexer_peek_token(Lexer* lexer, TKind tkind) {
    Lexer_ws(lexer);
    return Str_prefix_match(lexer->source, TKind_str(tkind)) != -1;
}

bool Lexer_maybe_token(Lexer* lexer, TKind tkind) {
    Lexer_ws(lexer);
    size_t i = Str_prefix_match(lexer->source, TKind_str(tkind));
    if (i != -1) {
        TokenArray_push(&lexer->tokens, Token_new(tkind, EMPTY_TK_DATA));
        Lexer_consume(lexer, i);
        return true;
    } else {
        return false;
    }
}

Str Lexer_ident(Lexer* lexer) {
    Lexer_ws(lexer);
    size_t i = 0;
    while (i < lexer->source.len) {
        char c = lexer->source.start[i];
        if ((i > 0 && !is_alphanum(c)) || (i == 0 && !is_alpha(c) && c != '@')) {
            break;
        }
        i++;
    }
    if (i > 0) {
        Str ident = lexer->source;
        ident.len = i;
        TokenArray_push(&lexer->tokens, Token_new(TK_IDENT, (TokenData) ident));
        Lexer_consume(lexer, i);
        return ident;
    } else {
        lex_error(lexer, "expected ident", EMPTY_STR);
    }
}

Str Lexer_str(Lexer* lexer) {
    Lexer_ws(lexer);
    Lexer_token(lexer, TK_DQUOTE);
    Str str = Lexer_consume_until(lexer, '"');
    TokenArray_push(&lexer->tokens, Token_new(TK_STR, (TokenData) str));
    Lexer_token(lexer, TK_DQUOTE);
    return str;
}

#endif // IZIGC_PARSER_H
