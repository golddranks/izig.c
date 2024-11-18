#ifndef IZIGC_PARSER_H
#define IZIGC_PARSER_H

#include "utils.h"

DefArray(LexArgs, struct LexArg)
DefArray(Exprs, struct Expr)
DefArray(Consts, struct Const)
DefArray(Stmts, struct Stmt)
DefArray(Params, struct Param)
DefArray(Types, struct Type)
DefArray(NS, struct LexBinding)
DefArray(Blocks, struct Block)
DefArray(Fns, struct Fn)

typedef enum ExKind {
    EX_REF,
    EX_FIELD,
    EX_CALL,
    EX_STR,
    EX_TUPLE,
} ExKind;

Str ExKind_str(ExKind kind) {
    char* cstr = (char*[]){
        "ref",
        "field",
        "call",
        "str",
        "tuple"
    }[kind];
    return Str_new(cstr);
}

typedef struct LexArg {
    struct Expr* expr;
    struct LexArg* next;
} LexArg;

DefArrayMethods(LexArgs, LexArg)

LexArg LexArg_new(struct Expr* expr) {
    return (LexArg) {
        .expr = expr,
        .next = NULL,
    };
}

typedef struct ExCallData {
    size_t n_args;
    LexArg* args;
} ExCallData;

typedef union ExData {
    void* empty;
    Str ident;
    Str str;
    ExCallData call;
} ExData;

typedef struct Expr {
    ExKind kind;
    ExData data;
    struct Expr* chain;
} Expr;

DefArrayMethods(Exprs, Expr)

Expr Expr_new(ExKind kind) {
    return (Expr) {
        .kind = kind,
        .data = { .empty = NULL },
    };
}

Expr Expr_new_ref(Str ident) {
    return (Expr) {
        .kind = EX_REF,
        .data = { .ident = ident },
    };
}

Expr Expr_new_field(Str ident) {
    return (Expr) {
        .kind = EX_FIELD,
        .data = { .ident = ident },
    };
}

Expr Expr_new_call(LexArg* args, size_t n_args) {
    return (Expr) {
        .kind = EX_CALL,
        .data = { .call = { .args = args, .n_args = n_args } },
    };
}

Expr Expr_new_str(Str str) {
    return (Expr) {
        .kind = EX_STR,
        .data = { .str = str },
    };
}

Expr Expr_new_tuple(LexArg* args, size_t n_args) {
    return (Expr) {
        .kind = EX_TUPLE,
        .data = { .call = { .args = args, .n_args = n_args } },
    };
}

typedef struct Const {
    Str ident;
    Expr* expr;
} Const;

DefArrayMethods(Consts, Const)

Const Const_new(Str ident, Expr* expr) {
    assert(expr != NULL);
    return (Const) {
        .ident = ident,
        .expr = expr,
    };
}

typedef enum SKind {
    ST_CONST,
    ST_TRY,
    ST_EXPR,
} SKind;

Str SKind_str(SKind kind) {
    char* c_str = (char*[]){
        "const",
        "try",
        "expr",
    }[kind];
    return Str_new(c_str);
}

typedef union SData {
    Expr* try;
    Const* constt;
    Expr* expr;
} SData;

typedef struct Stmt {
    SKind kind;
    struct Stmt* next;
    SData data;
} Stmt;

DefArrayMethods(Stmts, Stmt)

typedef struct Param {
    Str ident;
    struct Param* next;
} Param;

DefArrayMethods(Params, Param)

typedef struct Type {
    Str ident; // TODO
    bool error_union;
} Type;

DefArrayMethods(Types, Type)

Type Type_new(Str ident, bool error_union) {
    return (Type) {
        .ident = ident,
        .error_union = error_union,
    };
}

typedef struct LexBinding {
    Str ident;
    Expr* expr;
} LexBinding;

DefArrayMethods(NS, LexBinding)

LexBinding LexBinding_new(Str ident, Expr* expr) {
    return (LexBinding) {
        .ident = ident,
        .expr = expr,
    };
}

typedef struct Block {
    Stmt* stmt_head;
    NS* ns;
    size_t ns_start;
} Block;

DefArrayMethods(Blocks, Block)

Block Block_new(Stmt* stmt_head, NS* ns, size_t ns_start) {
    return (Block) {
        .stmt_head = stmt_head,
        .ns = ns,
        .ns_start = ns_start,
    };
}

typedef struct Fn {
    Str ident;
    size_t n_params;
    Param* params;
    Type* return_type;
    Block* body;
} Fn;

DefArrayMethods(Fns, Fn)

Fn Fn_new(Str ident, size_t n_params, Param* param_head, Type* return_type, Block* body) {
    assert(return_type != NULL);
    assert(body != NULL);
    return (Fn) {
        .ident = ident,
        .n_params = n_params,
        .params = param_head,
        .return_type = return_type,
        .body = body,
    };
}


typedef struct Parser {
    Consts consts;
    Fns fns;
    Blocks blocks;
    Params params;
    Stmts stmts;
    Types types;
    Exprs exprs;
    LexArgs lex_args;
    NS global_ns;
    NS local_ns;
} Parser;

Parser Parser_new(void) {
    return (Parser) {
        .consts = Consts_new(1000),
        .fns = Fns_new(1000),
        .blocks = Blocks_new(1000),
        .params = Params_new(1000),
        .stmts = Stmts_new(1000),
        .types = Types_new(1000),
        .exprs = Exprs_new(1000),
        .lex_args = LexArgs_new(1000),
        .global_ns = NS_new(1000),
        .local_ns = NS_new(1000),
    };
}

void Parser_print(Parser* parser) {
    fprintf(stderr, "Parser status:\n");
    fprintf(stderr, "Consts: ");
    ConstsIter consts = Consts_iter(&parser->consts);
    while (ConstsIter_next(&consts)) {
        fprintf(stderr, "%.*s ", (int) consts.current->ident.len, consts.current->ident.start);
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "Fns: ");
    FnsIter fns = Fns_iter(&parser->fns);
    while (FnsIter_next(&fns)) {
        fprintf(stderr, "%.*s ", (int) fns.current->ident.len, fns.current->ident.start);
    }
    fprintf(stderr, "\n");
}

Expr* Parser_expr(Parser* parser, Lexer* lexer);
Expr* Parser_suffix_expr(Parser* parser, Lexer* lexer);

Fn* Parser_get_fn(Parser* parser, const char* name) {
    FnsIter i = Fns_iter(&parser->fns);
    while (FnsIter_next(&i)) {
        if (Str_eq_cstr(i.current->ident, name)) {
            return i.current;
        }
    }
    return NULL;
}

Type* Parser_type(Parser* parser, Lexer* lexer) {
    bool error_union = Lexer_maybe_token(lexer, TK_EXCLAM);
    if (Lexer_maybe_token(lexer, TK_LBRACKET)) {
        Lexer_token(lexer, TK_RBRACKET);
        Lexer_maybe_token(lexer, TK_CONST);
    }
    Str ident = Lexer_ident(lexer);
    assert(Str_eq_cstr(ident, "void") || Str_eq_cstr(ident, "u8"));
    return Types_push(&parser->types, Type_new(ident, error_union));
}

Expr* Parser_field_access(Parser* parser, Lexer* lexer) {
    Lexer_token(lexer, TK_DOT);
    Str ident = Lexer_ident(lexer);
    Expr* expr = Exprs_push(&parser->exprs, Expr_new_field(ident));
    expr->chain = Parser_suffix_expr(parser, lexer);
    return expr;
}

size_t Parser_args(Parser* parser, Lexer* lexer, LexArg** head_arg) {
    size_t n_args = 0;
    Expr* expr = Parser_expr(parser, lexer);
    *head_arg = LexArgs_push(&parser->lex_args, LexArg_new(expr));
    LexArg* arg = *head_arg;
    n_args += 1;
    while (Lexer_maybe_token(lexer, TK_COMMA)) {
        expr = Parser_expr(parser, lexer);
        arg->next = LexArgs_push(&parser->lex_args, LexArg_new(expr));
        arg = arg->next;
        n_args += 1;
    };
    return n_args;
}

Expr* Parser_fn_call(Parser* parser, Lexer* lexer) {
    Lexer_token(lexer, TK_LPAREN);
    LexArg* head_arg = NULL;
    size_t n_args = 0;
    if (!Lexer_peek_token(lexer, TK_RPAREN)) {
        n_args = Parser_args(parser, lexer, &head_arg);
    }
    Expr* expr = Exprs_push(&parser->exprs, Expr_new_call(head_arg, n_args));
    Lexer_token(lexer, TK_RPAREN);
    expr->chain = Parser_suffix_expr(parser, lexer);
    return expr;
}

Expr* Parser_suffix_expr(Parser* parser, Lexer* lexer) {
    if (Lexer_peek_token(lexer, TK_DOT)) {
        return Parser_field_access(parser, lexer);
    } else if (Lexer_peek_token(lexer, TK_LPAREN)) {
        return Parser_fn_call(parser, lexer);
    }
    return NULL;
}

Expr* Parser_tuple(Parser* parser, Lexer* lexer) {
    Lexer_token(lexer, TK_DOT);
    Lexer_token(lexer, TK_LBRACE);

    LexArg* head_arg = NULL;
    size_t n_args = 0;
    if (!Lexer_peek_token(lexer, TK_RBRACE)) {
        n_args = Parser_args(parser, lexer, &head_arg);
    }
    Lexer_token(lexer, TK_RBRACE);
    return Exprs_push(&parser->exprs, Expr_new_tuple(head_arg, n_args));
}

Expr* Parser_expr(Parser* parser, Lexer* lexer) {
    Expr* expr;
    if (Lexer_peek_token(lexer, TK_DOT)) {
        expr = Parser_tuple(parser, lexer);
    } else if (Lexer_peek_token(lexer, TK_DQUOTE)) {
        Str str = Lexer_str(lexer);
        expr = Exprs_push(&parser->exprs, Expr_new_str(str));
    } else {
        Str ident = Lexer_ident(lexer);
        expr = Exprs_push(&parser->exprs, Expr_new_ref(ident));
    }
    expr->chain = Parser_suffix_expr(parser, lexer);
    return expr;
}

Const* Parser_const(Parser* parser, Lexer* lexer, NS* ns) {
    Lexer_token(lexer, TK_CONST);
    Str ident = Lexer_ident(lexer);
    Lexer_token(lexer, TK_ASSIGN);
    Expr* expr = Parser_expr(parser, lexer);
    Lexer_token(lexer, TK_EOS);
    NS_push(ns, LexBinding_new(ident, expr));
    return Consts_push(&parser->consts, Const_new(ident, expr));
}

Expr* Parser_try(Parser* parser, Lexer* lexer) {
    Lexer_token(lexer, TK_TRY);
    Expr* expr = Parser_expr(parser, lexer);
    Lexer_token(lexer, TK_EOS);
    return expr;
}

Stmt* Parser_stmt(Parser* parser, Lexer* lexer, NS* ns) {
    Stmt stmt;
    stmt.next = NULL;
    if (Lexer_peek_token(lexer, TK_CONST)) {
        stmt.kind = ST_CONST;
        stmt.data = (SData) { .constt = Parser_const(parser, lexer, ns) };
    } else if (Lexer_peek_token(lexer, TK_TRY)) {
        stmt.kind = ST_TRY;
        stmt.data = (SData) { .try = Parser_try(parser, lexer) };
    } else {
        stmt.kind = ST_EXPR;
        stmt.data = (SData) { .expr = Parser_expr(parser, lexer) };
        Lexer_token(lexer, TK_EOS);
    }
    return Stmts_push(&parser->stmts, stmt);
}

Block* Parser_block(Parser* parser, Lexer* lexer) {
    Lexer_token(lexer, TK_LBRACE);
    Stmt* stmt_head = NULL;
    NS* ns = &parser->local_ns;
    size_t ns_start = ns->slice.len;

    if (!Lexer_peek_token(lexer, TK_RBRACE)) {
        stmt_head = Parser_stmt(parser, lexer, ns);
        Stmt* stmt = stmt_head;
        while (!Lexer_peek_token(lexer, TK_RBRACE)) {
            Stmt* next_stmt = Parser_stmt(parser, lexer, ns);
            stmt->next = next_stmt;
            stmt = next_stmt;
        }
    }
    Lexer_token(lexer, TK_RBRACE);

    return Blocks_push(&parser->blocks, Block_new(stmt_head, ns, ns_start));
}

Fn* Parser_fn_decl(Parser* parser, Lexer* lexer) {
    Lexer_token(lexer, TK_FN);
    Str ident = Lexer_ident(lexer);
    Lexer_token(lexer, TK_LPAREN);
    size_t n_params = 0;
    Param* param_head = NULL;
    if (!Lexer_peek_token(lexer, TK_RPAREN)) {
        Str ident = Lexer_ident(lexer);
        Lexer_token(lexer, TK_COLON);
        Parser_type(parser, lexer);
        param_head = Params_push(&parser->params, (Param) { .ident = ident });
        Param* param = param_head;
        n_params += 1;
        while (Lexer_maybe_token(lexer, TK_COMMA)) {
            ident = Lexer_ident(lexer);
            Lexer_token(lexer, TK_COLON);
            Parser_type(parser, lexer);
            param->next = Params_push(&parser->params, (Param) { .ident = ident });
            param = param->next;
            n_params += 1;
        };
    }
    Lexer_token(lexer, TK_RPAREN);
    Type* return_type = Parser_type(parser, lexer);
    Block* block = Parser_block(parser, lexer);

    return Fns_push(&parser->fns, Fn_new(ident, n_params, param_head, return_type, block));
}

void Parser_item(Parser* parser, Lexer* lexer) {
    Lexer_maybe_token(lexer, TK_PUB);
    if (Lexer_peek_token(lexer, TK_CONST)) {
        Parser_const(parser, lexer, &parser->global_ns);
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

#endif // IZIGC_PARSER_H
