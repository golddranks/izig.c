#ifndef IZIGC_LEXER_H
#define IZIGC_LEXER_H

#include "utils.h"

DefArray(Tokens, struct Token)

typedef enum TkKind {
    TK_CONST,
    TK_IDENT,
    TK_ASSIGN,
    TK_DOT,
    TK_LPAREN,
    TK_RPAREN,
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
    TK_COLON,
    TK_LBRACKET,
    TK_RBRACKET
} TkKind;

Str TkKind_str(TkKind kind) {
    char* cstr = (char*[]){
        "const",
        "ident",
        "=",
        ".",
        "(",
        ")",
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
        ":",
        "[",
        "]",
    }[kind];
    return Str_new(cstr);
}

typedef union TkData {
    void* empty;
    Str ident;
    Str str;
} TkData;

typedef struct Token {
    TkKind kind;
    TkData data;
} Token;

DefArrayMethods(Tokens, Token)

Token Token_new(TkKind kind, TkData data) {
    return (Token) {
        .kind = kind,
        .data = data,
    };
}

typedef struct Lexer {
    int consumed;
    Str source;
    Tokens tokens;
} Lexer;

Lexer Lexer_new(Str source) {
    return (Lexer) {
        .consumed = 0,
        .source = source,
        .tokens = Tokens_new(1000),
    };
}

Str Token_str(Token token) {
    if (token.kind == TK_IDENT) {
        return token.data.ident;
    } else if (token.kind == TK_STR) {
        return token.data.str;
    }
    return TkKind_str(token.kind);
}

void Lexer_print(Lexer* lexer) {
    fprintf(stderr, "Lexer status:\n");
    fprintf(stderr, "Currently consumed: %d\n", lexer->consumed);
    fprintf(stderr, "Tokens: ");
    TokensIter i = Tokens_iter(&lexer->tokens);
    while (TokensIter_next(&i)) {
        Str token = Token_str(*i.current);
        Str_print(token);
        fprintf(stderr, " ");
    }
    fprintf(stderr, "\n");
}

void __lex_error(Lexer* lexer, const char* msg, Str expected, const char* file, const char* func, int line) {
    fprintf(
        stderr,
        "Error: %s; expected: %.*s, got: %c (at %s:%s:%d)\n",
        msg,
        (int) expected.len,
        expected.start,
        lexer->source.start[0],
        file,
        func,
        line
    );
    Lexer_print(lexer);
}

#define lex_error(lexer, msg, expected) __lex_error(lexer, msg, expected, __FILE__, __func__, __LINE__); exit(EXIT_FAILURE);

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

bool Lexer_peek_token(Lexer* lexer, TkKind tkind) {
    Lexer_ws(lexer);
    return Str_prefix_match(lexer->source, TkKind_str(tkind));
}

bool Lexer_maybe_token(Lexer* lexer, TkKind tkind) {
    Lexer_ws(lexer);
    Str tkind_str = TkKind_str(tkind);
    if (Str_prefix_match(lexer->source, tkind_str)) {
        Tokens_push(&lexer->tokens, Token_new(tkind, (TkData) { .empty = NULL }));
        Lexer_consume(lexer, tkind_str.len);
        return true;
    } else {
        return false;
    }
}

void Lexer_token(Lexer* lexer, TkKind tkind) {
    if (!Lexer_maybe_token(lexer, tkind)) {
        lex_error(lexer, "expected token", TkKind_str(tkind));
    }
}

Str Lexer_ident(Lexer* lexer) {
    Lexer_ws(lexer);
    char c = lexer->source.start[0];
    if (!is_ident_first(c)) {
        lex_error(lexer, "expected ident", EMPTY_STR);
    }
    size_t i = 1;
    while (i < lexer->source.len) {
        c = lexer->source.start[i];
        if (!is_ident_next(c)) {
            break;
        }
        i++;
    }
    Str ident = lexer->source;
    ident.len = i;
    Tokens_push(&lexer->tokens, Token_new(TK_IDENT, (TkData) { .ident = ident }));
    Lexer_consume(lexer, i);
    return ident;
}

Str Lexer_str(Lexer* lexer) {
    Lexer_ws(lexer);
    Lexer_token(lexer, TK_DQUOTE);
    Str str = Lexer_consume_until(lexer, '"');
    Tokens_push(&lexer->tokens, Token_new(TK_STR, (TkData) { .str = str }));
    Lexer_token(lexer, TK_DQUOTE);
    return str;
}

#endif // IZIGC_LEXER_H
