#ifndef IZIGC_ARRAY_H
#define IZIGC_ARRAY_H

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#define DefArray(ArrayName, T)                                                 \
                                                                               \
typedef struct ArrayName##Slice {                                              \
    T* buf;                                                                    \
    size_t len;                                                                \
} ArrayName##Slice;                                                            \
                                                                               \
const ArrayName##Slice EMPTY_##ArrayName##Slice = { .buf = NULL, .len = 0};    \
                                                                               \
typedef struct ArrayName {                                                     \
    ArrayName##Slice slice;                                                    \
    size_t cap;                                                                \
} ArrayName;                                                                   \
                                                                               \
typedef struct ArrayName##Iter {                                               \
    T* next;                                                                   \
    T* current;                                                                \
    T* end;                                                                    \
} ArrayName##Iter;

#define DefArrayMethods(ArrayName, T)                                          \
                                                                               \
ArrayName ArrayName##_new(size_t capacity) {                                   \
    return (ArrayName) {                                                       \
        .slice = {                                                             \
            .buf = malloc(sizeof(T) * capacity),                               \
            .len = 0                                                           \
        },                                                                     \
        .cap = capacity,                                                       \
    };                                                                         \
}                                                                              \
                                                                               \
ArrayName##Slice ArrayName##_slice(ArrayName arr, size_t start, size_t end) {  \
    assert(arr.slice.buf != NULL);                                             \
    assert(end <= arr.slice.len);                                              \
    assert(start <= end);                                                      \
    return (ArrayName##Slice) {                                                \
        .buf = arr.slice.buf + start,                                          \
        .len = end - start,                                                    \
    };                                                                         \
}                                                                              \
                                                                               \
T* ArrayName##Slice_get(ArrayName##Slice slice, size_t idx) {                  \
    assert(slice.buf != NULL);                                                 \
    assert(idx < slice.len);                                                   \
    return slice.buf + idx;                                                    \
}                                                                              \
                                                                               \
T* ArrayName##Slice_set(ArrayName##Slice slice, size_t idx, T data) {          \
    assert(slice.buf != NULL);                                                 \
    assert(idx < slice.len);                                                   \
    slice.buf[idx] = data;                                                     \
    return &slice.buf[idx];                                                    \
}                                                                              \
T* ArrayName##_get(ArrayName arr, size_t idx) {                                \
    return ArrayName##Slice_get(arr.slice, idx);                               \
}                                                                              \
                                                                               \
T* ArrayName##_set(ArrayName arr, size_t idx, T data) {                        \
    return ArrayName##Slice_set(arr.slice, idx, data);                         \
}                                                                              \
                                                                               \
T* ArrayName##_push(ArrayName* arr, T data) {                                  \
    assert(arr != NULL);                                                       \
    assert(arr->slice.len < arr->cap);                                         \
    size_t idx = arr->slice.len;                                               \
    arr->slice.len += 1;                                                       \
    return ArrayName##_set(*arr, idx, data);                                   \
}                                                                              \
                                                                               \
ArrayName##Slice ArrayName##_reserve(ArrayName* arr, size_t n) {               \
    assert(arr != NULL);                                                       \
    assert(arr->slice.len + n <= arr->cap);                                    \
    size_t orig_len = arr->slice.len;                                          \
    arr->slice.len += n;                                                       \
    return ArrayName##_slice(*arr, orig_len, arr->slice.len);                  \
}                                                                              \
                                                                               \
T ArrayName##_pop(ArrayName* arr) {                                            \
    assert(arr != NULL);                                                       \
    assert(arr->slice.len > 0);                                                \
    arr->slice.len -= 1;                                                       \
    size_t idx = arr->slice.len;                                               \
    return *ArrayName##_get(*arr, idx);                                        \
}                                                                              \
                                                                               \
ArrayName##Iter ArrayName##Slice_iter(ArrayName##Slice* slice) {               \
    assert(slice != NULL);                                                     \
    assert(slice->len == 0 || slice->buf != NULL);                             \
    return (ArrayName##Iter) {                                                 \
        .next = slice->buf,                                                    \
        .current = NULL,                                                       \
        .end = slice->buf + slice->len,                                        \
    };                                                                         \
}                                                                              \
                                                                               \
ArrayName##Iter ArrayName##_iter(ArrayName* arr) {                             \
    return ArrayName##Slice_iter(&arr->slice);                                 \
}                                                                              \
                                                                               \
bool ArrayName##Iter_next(ArrayName##Iter* iter) {                             \
    assert(iter != NULL);                                                      \
    iter->current = iter->next;                                                \
    if (iter->next == iter->end) {                                             \
        return false;                                                          \
    }                                                                          \
    assert(iter->current != NULL);                                             \
    iter->next += 1;                                                           \
    return true;                                                               \
}

DefArray(IntArray, int)

#endif // IZIGC_ARRAY_H
