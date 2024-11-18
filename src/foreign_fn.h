#include "utils.h"
#include "runtime.h"

Val print(ValsSlice args) {
    assert(args.len == 2);
    Val* arg_0 = ValsSlice_get(args, 0);
    Val* arg_1 = ValsSlice_get(args, 1);
    assert(arg_0->kind == VA_STR);
    assert(arg_1->kind == VA_TUPLE);
    Str str = arg_0->data.str;
    ValsSlice tuple = arg_1->data.vals;
    size_t fmt_arg_i = 0;
    int i = 0;
    while (i < (int) str.len) {
        if (str.start[i] == '{') {
            fwrite(str.start, sizeof(char), i, stdout);
            assert(str.start[i+1] == 's');
            assert(str.start[i+2] == '}');
            Val* fmt_arg = ValsSlice_get(tuple, fmt_arg_i);
            assert(fmt_arg->kind == VA_STR);
            fwrite(fmt_arg->data.str.start, sizeof(char), fmt_arg->data.str.len, stdout);
            Str_consume_n(&str, i + 3);
            i = 0;
        } else if (str.start[i] == '\\') {
            fwrite(str.start, sizeof(char), i, stdout);
            assert(str.start[i+1] == 'n');
            fwrite("\n", sizeof(char), i, stdout);
            Str_consume_n(&str, i + 2);
            i = 0;
        } else {
            i += 1;
        }
    }
    fwrite(str.start, sizeof(char), (int) str.len, stdout);
    return (Val) { .kind = VA_VOID };
}

const ForeignFn PRINT_FN = (ForeignFn) {
    .fn = print,
};

RunBinding __WRITER_FIELDS[] = {
    {
        .ident = { .len = 5, .start = "print" },
        .val = { .kind = VA_FOREIGN_FN, .data.foreign_fn = &PRINT_FN },
    },
};

Struct WRITER_STRUCT = {
    .fields = {
        .len = 1,
        .buf = __WRITER_FIELDS,
    }
};

Val writer(ValsSlice args) {
    assert(args.len == 0);
    return (Val) { .kind = VA_STRUCT, .data.structt = &WRITER_STRUCT };
}

const ForeignFn WRITER_FN = (ForeignFn) {
    .fn = writer,
};

RunBinding __FILE_FIELDS[] = {
    {
        .ident = { .len = 6, .start = "writer" },
        .val = { .kind = VA_FOREIGN_FN, .data.foreign_fn = &WRITER_FN },
    },
};

Struct FILE_STRUCT = {
    .fields = {
        .len = 1,
        .buf = __FILE_FIELDS,
    }
};

Val get_std_out(ValsSlice args) {
    assert(args.len == 0);
    return (Val) { .kind = VA_STRUCT, .data.structt = &FILE_STRUCT };
}

const ForeignFn GET_STD_OUT_FN = (ForeignFn) {
    .fn = get_std_out,
};

RunBinding __STD_IO_FIELDS[] = {
    {
        .ident = { .len = 9, .start = "getStdOut" },
        .val = { .kind = VA_FOREIGN_FN, .data.foreign_fn = &GET_STD_OUT_FN },
    },
};

Struct STD_IO_STRUCT = {
    .fields = {
        .len = 1,
        .buf = __STD_IO_FIELDS,
    }
};


RunBinding __STD_FIELDS[] = {
    {
        .ident = { .len = 2, .start = "io" },
        .val = { .kind = VA_STRUCT, .data.structt = &STD_IO_STRUCT },
    },
};

Struct STD_STRUCT = {
    .fields = {
        .len = 1,
        .buf = __STD_FIELDS,
    }
};

Val at_import(ValsSlice args) {
    assert(args.len == 1);
    Val* arg = ValsSlice_get(args, 0);
    assert(arg->kind == VA_STR);
    if (Str_eq_cstr(arg->data.str, "std")) {
        return (Val) { .kind = VA_STRUCT, .data.structt = &STD_STRUCT };
    } else {
        todo();
    }
}

const ForeignFn AT_IMPORT = (ForeignFn) {
    .fn = at_import,
};
