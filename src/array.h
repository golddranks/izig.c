#ifndef IZIGC_ARRAY_H
#define IZIGC_ARRAY_H

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#define DefArray(ArrayName, TypeName, Capacity)                          \
                                                                         \
typedef struct ArrayName {                                               \
    TypeName* buf;                                                       \
    size_t len;                                                          \
} ArrayName;                                                             \
                                                                         \
typedef struct ArrayName##Idx {                                          \
    size_t i;                                                            \
} ArrayName##Idx;                                                        \
                                                                         \
const ArrayName##Idx NULL_##ArrayName##Idx = { -1 };                     \
                                                                         \
ArrayName##Idx ArrayName##_idx(size_t i) {                               \
    return (ArrayName##Idx) { .i = i };                                  \
}                                                                        \
                                                                         \
ArrayName ArrayName##_new() {                                            \
    return (ArrayName) {                                                 \
        .buf = malloc(sizeof(TypeName) * Capacity),                      \
        .len = 0                                                         \
    };                                                                   \
}                                                                        \
                                                                         \
TypeName* ArrayName##_get(ArrayName arr, ArrayName##Idx idx) {           \
    size_t i = idx.i;                                                    \
    assert(i < arr.len);                                                 \
    return arr.buf + i;                                                  \
}                                                                        \
                                                                         \
void ArrayName##_set(ArrayName arr, ArrayName##Idx idx, TypeName data) { \
    size_t i = idx.i;                                                    \
    assert(i < arr.len);                                                 \
    arr.buf[i] = data;                                                   \
}                                                                        \
                                                                         \
ArrayName##Idx ArrayName##_push(ArrayName* arr, TypeName data) {         \
    assert(arr->len < Capacity);                                         \
    ArrayName##Idx idx = ArrayName##_idx(arr->len);                      \
    arr->len += 1;                                                       \
    ArrayName##_set(*arr, idx, data);                                    \
    return idx;                                                          \
}                                                                        \
                                                                         \
TypeName ArrayName##_pop(ArrayName* arr) {                               \
    assert(arr->len > 0);                                                \
    arr->len -= 1;                                                       \
    ArrayName##Idx idx = ArrayName##_idx(arr->len);                      \
    return *ArrayName##_get(*arr, idx);                                  \
}

DefArray(IntArray, int, 1000)

#endif // IZIGC_ARRAY_H
