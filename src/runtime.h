#ifndef IZIGC_RUNTIME_H
#define IZIGC_RUNTIME_H

#include "utils.h"

DefArray(Vals, struct Val)
DefArray(NSs, struct RunBinding)
DefArray(Structs, struct Struct)


typedef enum VaKind {
    VA_VOID,
    VA_STR,
    VA_STRUCT,
    VA_TUPLE,
    VA_FN,
    VA_FOREIGN_FN,
} VaKind;

Str VaKind_str(VaKind kind) {
    char* cstr = (char*[]) {
        "void",
        "str",
        "struct",
        "tuple",
        "fn",
        "foreign_fn"
    }[kind];
    return Str_new(cstr);
}


typedef union VaData {
    const struct ForeignFn* foreign_fn;
    struct Fn* fn;
    Str str;
    ValsSlice vals;
    struct Struct* structt;
} VaData;

typedef struct Val {
    VaKind kind;
    VaData data;
} Val;

DefArrayMethods(Vals, Val)

typedef Val (* ForeignFnPtr)(ValsSlice args);

typedef struct ForeignFn {
    ForeignFnPtr fn;
} ForeignFn;

typedef struct RunBinding {
    Str ident;
    Val val;
} RunBinding;

DefArrayMethods(NSs, RunBinding)

typedef struct Struct {
    NSsSlice fields;
} Struct;

DefArrayMethods(Structs, Struct)

const Val VOID = { .kind = VA_VOID };


#endif // IZIGC_RUNTIME_H
