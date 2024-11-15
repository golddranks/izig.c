
#include <stdio.h>
#include "array.h"
#include "str.h"
#include "lexer.h"

typedef struct Const {
    Str ident;
} Const;

DefArray(ConstArray, Const, 1000)

Const Const_new(Str ident) {
    return (Const) { .ident = ident };
}

typedef struct Stmt {
    TokenArrayIdx token; // TODO
} Stmt;

DefArray(StmtArray, Stmt, 1000)

typedef struct Arg {
    Str ident; // TODO
} Arg;

DefArray(ArgArray, Arg, 1000)

typedef struct Type {
    Str ident; // TODO
    bool error_union;
} Type;

Type Type_new(Str ident, bool error_union) {
    return (Type) {
        .ident = ident,
        .error_union = error_union,
    };
}

DefArray(TypeArray, Type, 1000)

typedef struct Fn {
    Str ident;
    ArgArrayIdx args;
    TypeArrayIdx return_type;
    StmtArrayIdx body;
} Fn;

DefArray(FnArray, Fn, 1000)

const Fn NULL_FN = (Fn) { .ident = {}, .args = { -1 }, .body = { -1 } };

Fn Fn_new(Str ident, ArgArrayIdx arg_head, TypeArrayIdx return_type) {
    return (Fn) {
        .ident = ident,
        .args = arg_head,
        .return_type = return_type,
        .body = { -1 },
    };
}

typedef struct Parser {
    ConstArray consts;
    FnArray fns;
    ArgArray args;
    StmtArray stmts;
    TypeArray types;
} Parser;

Parser Parser_new() {
    return (Parser) {
        .consts = ConstArray_new(),
        .fns = FnArray_new(),
        .args = ArgArray_new(),
        .stmts = StmtArray_new(),
        .types = TypeArray_new(),
    };
}


void Parser_expr(Parser* parser, Lexer* lexer);
void Parser_suffix_expr(Parser* parser, Lexer* lexer);

ConstArrayIdx Parser_push_const(Parser* parser, Const c) {
    return ConstArray_push(&parser->consts, c);
}

FnArrayIdx Parser_push_fn(
    Parser* parser,
    Str ident,
    ArgArrayIdx args_head,
    TypeArrayIdx return_type
) {
    Fn fn = Fn_new(ident, args_head, return_type);
    FnArrayIdx fn_idx = FnArray_push(&parser->fns, fn);
    return fn_idx;
}

Fn Parser_get_fn(Parser* parser, const char* name) {
    for (int i = 0; i < parser->fns.len; i++) {
        Fn fn = *FnArray_get(parser->fns, FnArray_idx(i));
        if (Str_eq_c_str(fn.ident, name)) {
            return fn;
        }
    }
    return NULL_FN;
}

TypeArrayIdx Parser_type(Parser* parser, Lexer* lexer) {
    bool error_union = Lexer_maybe_token(lexer, TK_EXCLAM);
    Str ident = Lexer_ident(lexer);
    return TypeArray_push(&parser->types, Type_new(ident, error_union));
}

void Parser_field_access(Parser* parser, Lexer* lexer) {
    Lexer_token(lexer, TK_DOT);
    Lexer_ident(lexer);
    Parser_suffix_expr(parser, lexer);
}

void Parser_fn_call(Parser* parser, Lexer* lexer) {
    Lexer_token(lexer, TK_LPAREN);
    if (!Lexer_peek_token(lexer, TK_RPAREN)) {
        do {
            Parser_expr(parser, lexer);
        } while (Lexer_maybe_token(lexer, TK_COMMA));
    }
    Lexer_token(lexer, TK_RPAREN);
    Parser_suffix_expr(parser, lexer);
}

void Parser_suffix_expr(Parser* parser, Lexer* lexer) {
    if (Lexer_peek_token(lexer, TK_DOT)) {
        Parser_field_access(parser, lexer);
    } else if (Lexer_peek_token(lexer, TK_LPAREN)) {
        Parser_fn_call(parser, lexer);
    }
}

void Parser_tuple(Parser* parser, Lexer* lexer) {
    Lexer_token(lexer, TK_DOT);
    Lexer_token(lexer, TK_LBRACE);
    if (!Lexer_peek_token(lexer, TK_RBRACE)) {
        do {
            Parser_expr(parser, lexer);
        } while (Lexer_maybe_token(lexer, TK_COMMA));
    }
    Lexer_token(lexer, TK_RBRACE);
}

void Parser_expr(Parser* parser, Lexer* lexer) {
    if (Lexer_peek_token(lexer, TK_DOT)) {
        Parser_tuple(parser, lexer);
    } else if (Lexer_peek_token(lexer, TK_DQUOTE)) {
        Lexer_str(lexer);
    } else {
        Lexer_ident(lexer);
    }
    Parser_suffix_expr(parser, lexer);
}

void Parser_const(Parser* parser, Lexer* lexer) {
    Lexer_token(lexer, TK_CONST);
    Str ident = Lexer_ident(lexer);
    Lexer_token(lexer, TK_ASSIGN);
    Parser_expr(parser, lexer);
    Lexer_token(lexer, TK_EOS);
    Parser_push_const(parser, Const_new(ident));
}

void Parser_try(Parser* parser, Lexer* lexer) {
    Lexer_token(lexer, TK_TRY);
    Parser_expr(parser, lexer);
    Lexer_token(lexer, TK_EOS);
}

void Parser_stmt(Parser* parser, Lexer* lexer) {
    if (Lexer_peek_token(lexer, TK_CONST)) {
        Parser_const(parser, lexer);
    } else if (Lexer_peek_token(lexer, TK_TRY)) {
        Parser_try(parser, lexer);
    } else {
        lex_error(lexer, "expected stmt", EMPTY_STR);
    }
}

void Parser_body(Parser* parser, Lexer* lexer) {
    while (!Lexer_peek_token(lexer, TK_RBRACE)) {
        Parser_stmt(parser, lexer);
    }
}

void Parser_fn_decl(Parser* parser, Lexer* lexer) {
    Lexer_token(lexer, TK_FN);
    Str ident = Lexer_ident(lexer);
    Lexer_token(lexer, TK_LPAREN);
    // TODO: params
    Lexer_token(lexer, TK_RPAREN);
    TypeArrayIdx return_type = Parser_type(parser, lexer);
    Lexer_token(lexer, TK_LBRACE);
    Parser_body(parser, lexer);
    Lexer_token(lexer, TK_RBRACE);
    Parser_push_fn(parser, ident, NULL_ArgArrayIdx, return_type);
}

void Parser_item(Parser* parser, Lexer* lexer) {
    Lexer_maybe_token(lexer, TK_PUB);
    if (Lexer_peek_token(lexer, TK_CONST)) {
        Parser_const(parser, lexer);
    } else if (Lexer_peek_token(lexer, TK_FN)) {
        Parser_fn_decl(parser, lexer);
    } else {
        lex_error(lexer, "expected item", EMPTY_STR);
    }
}

Parser parse(Lexer* lexer) {
    Parser parse_parser = Parser_new();
    while(lexer->source.len > 0) {
        Parser_item(&parse_parser, lexer);
        Lexer_ws(lexer);
    }
    return parse_parser;
}
