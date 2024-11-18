#include "utils.h"

#include "lexer.h"
#include "parser.h"
#include "runtime.h"
#include "foreign_fn.h"

typedef struct Runner {
    Lexer* lexer;
    Parser* parser;
    NSs ns_stack;
    Vals val_stack;
    Structs struct_stack;
} Runner;

void Runner_print(Runner* runner) {
    fprintf(stderr, "NS stack: ");
    NSsIter i = NSs_iter(&runner->ns_stack);
    while (NSsIter_next(&i)) {
        Str_print(i.current->ident);
        fprintf(stderr, " ");
    }
    fprintf(stderr, "\n");
}

void __run_error(Runner* runner, const char* msg, Str str, const char* file, const char* func, int line) {
    fprintf(
        stderr,
        "Error: %s %.*s (at %s:%s:%d)\n",
        msg,
        (int) str.len,
        str.start,
        file,
        func,
        line
    );
    Runner_print(runner);
}

#define run_error(runner, msg, str) __run_error(runner, msg, str, __FILE__, __func__, __LINE__); exit(EXIT_FAILURE);

Runner Runner_new(Lexer* lexer, Parser* parser) {
    Runner runner = (Runner) {
        .lexer = lexer,
        .parser = parser,
        .ns_stack = NSs_new(1000),
        .val_stack = Vals_new(1000),
        .struct_stack = Structs_new(1000),
    };
    return runner;
}


Val Runner_get_ref(Runner* runner, Str ident) {
    NSsIter i = NSs_iter(&runner->ns_stack);
    while (NSsIter_next(&i)) {
        if (Str_eq(i.current->ident, ident)) {
            return i.current->val;
        }
    }
    run_error(runner, "ref not found", ident);
}

Val Runner_get_field(Runner* runner, Str ident, Val* in_val) {
    assert(in_val->kind == VA_STRUCT);
    assert(in_val->data.structt != NULL);
    NSsIter i = NSsSlice_iter(&in_val->data.structt->fields);
    while (NSsIter_next(&i)) {
        if (Str_eq(i.current->ident, ident)) {
            return i.current->val;
        }
    }
    run_error(runner, "field not found", ident);
}

Val Runner_expr(Runner* runner, Expr* expr, Val* in_val);

ValsSlice Runner_args(Runner* runner, ExCallData ex_arg_data) {
    size_t n_args = ex_arg_data.n_args;
    ValsSlice slice = Vals_reserve(&runner->val_stack, n_args);
    LexArg* args = ex_arg_data.args;
    for (size_t i = 0; i < n_args; i++) {
        assert(args != NULL);
        Val val = Runner_expr(runner, args->expr, NULL);
        ValsSlice_set(slice, i, val);
        args = args->next;
    }
    return slice;
}

Val Runner_tuple(Runner* runner, Expr* expr) {
    assert(expr->kind == EX_TUPLE);
    ValsSlice slice = Runner_args(runner, expr->data.call);
    return (Val) { .kind = VA_TUPLE, .data.vals = slice };
}

void Runner_const_stmt(Runner* runner, Stmt* stmt) {
    assert(stmt->kind == ST_CONST);
    Str ident = stmt->data.constt->ident;
    Val val = Runner_expr(runner, stmt->data.constt->expr, NULL);
    NSs_push(&runner->ns_stack, (RunBinding) { .ident = ident, .val = val });
}

void Runner_stmt(Runner* runner, Stmt* stmt) {
    switch (stmt->kind) {
    case ST_CONST:
        Runner_const_stmt(runner, stmt);
        return;
    case ST_TRY:
        Runner_expr(runner, stmt->data.try, NULL);
        return;
    case ST_EXPR:
        Runner_expr(runner, stmt->data.try, NULL);
        return;
    default: assert(false);
    }
}

Val Runner_call_fn(Runner* runner, Fn* fn, ValsSlice args) {
    ValsIter a = ValsSlice_iter(&args);
    Param* p = fn->params;
    while (ValsIter_next(&a)) {
        assert(p != NULL);
        NSs_push(&runner->ns_stack, (RunBinding) { .ident = p->ident, .val = *a.current });
        p = p->next;
    }
    assert(p == NULL);
    Stmt* stmt = fn->body->stmt_head;
    do {
        Runner_stmt(runner, stmt);
        stmt = stmt->next;
    } while (stmt != NULL);
    return VOID;
}

Val Runner_expr(Runner* runner, Expr* expr, Val* in_val) {
    Val out_val;
    Fn* fn = NULL;
    switch (expr->kind)
    {
    case EX_REF:
        out_val = Runner_get_ref(runner, expr->data.ident);
        if (expr->chain) {
            return Runner_expr(runner, expr->chain, &out_val);
        }
        return out_val;
    case EX_FIELD:
        out_val = Runner_get_field(runner, expr->data.ident, in_val);
        if (expr->chain) {
            return Runner_expr(runner, expr->chain, &out_val);
        }
        return out_val;
    case EX_CALL:
        if (in_val->kind == VA_FN) {
            ValsSlice args = Runner_args(runner, expr->data.call);
            fn = in_val->data.fn;
            assert(fn != NULL);
            out_val = Runner_call_fn(runner, fn, args);
        } else if (in_val->kind == VA_FOREIGN_FN) {
            ValsSlice slice = Runner_args(runner, expr->data.call);
            assert(in_val->data.foreign_fn != NULL);
            out_val = in_val->data.foreign_fn->fn(slice);
        }
        if (expr->chain) {
            return Runner_expr(runner, expr->chain, &out_val);
        }
        return out_val;
    case EX_STR:
        out_val = (Val) { .kind = VA_STR, .data.str = expr->data.str  };
        if (expr->chain) {
            return Runner_expr(runner, expr->chain, &out_val);
        }
        return out_val;
    case EX_TUPLE:
        out_val = Runner_tuple(runner, expr);
        if (expr->chain) {
            return Runner_expr(runner, expr->chain, &out_val);
        }
        return out_val;
    default:
        run_error(runner, "invalid kind", ExKind_str(expr->kind));
    }
}

void Runner_init_globals(Runner* runner) {
    NSIter i = NS_iter(&runner->parser->global_ns);
    while (NSIter_next(&i)) {
        RunBinding binding = (RunBinding) {
            .ident = i.current->ident,
            .val = Runner_expr(runner, i.current->expr, NULL),
        };
        NSs_push(&runner->ns_stack, binding);
    }
    FnsIter f = Fns_iter(&runner->parser->fns);
    while (FnsIter_next(&f)) {
        RunBinding binding = (RunBinding) {
            .ident = f.current->ident,
            .val = (Val) { .kind = VA_FN, .data.fn = f.current },
        };
        NSs_push(&runner->ns_stack, binding);
    }
}

void Runner_init_builtins(Runner* runner) {
    RunBinding binding = (RunBinding) {
        .ident = Str_new("@import"),
        .val = (Val) {
            .kind = VA_FOREIGN_FN,
            .data = (VaData) { .foreign_fn = &AT_IMPORT },
        },
    };
    NSs_push(&runner->ns_stack, binding);
}

void run(Lexer* lexer, Parser* parser, bool verbose) {
    Runner runner = Runner_new(lexer, parser);
    if (verbose) {
        fprintf(stderr, "Initializing runtime.\n");
    }
    Runner_init_builtins(&runner);
    Runner_init_globals(&runner);
    if (verbose) {
        Runner_print(&runner);
        fprintf(stderr, "Running main!\n\n");
    }
    Fn* fn = Parser_get_fn(parser, "main");
    assert(fn != NULL);
    Runner_call_fn(&runner, fn, EMPTY_ValsSlice);
}
