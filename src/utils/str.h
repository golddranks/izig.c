#ifndef IZIGC_STR_H
#define IZIGC_STR_H

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

bool is_ws(char c) {
    return c == ' ' || c == '\t' || c == '\n';
}

bool is_ident_first(char c) {
    return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') || c == '_' || c == '@';
}

bool is_ident_next(char c) {
    return is_ident_first(c) || ('0' <= c && c <= '9');
}

typedef struct Str {
    const char* start;
    size_t len;
} Str;

const Str EMPTY_STR = (Str) { .start = "", .len = 0 };

Str Str_new(const char* cstr) {
    return (Str) { .start = cstr, .len = strlen(cstr) };
}

void Str_consume_n(Str* source, size_t n) {
    assert(source->len >= n);
    source->start += n;
    source->len -= n;
}

Str Str_slice(Str s, size_t start, size_t end) {
    assert(end <= s.len);
    assert(start <= end);
    return (Str) {
        .len = end - start,
        .start = s.start + start,
    };
}

void Str_print(Str s) {
    fwrite(s.start, sizeof(unsigned char), s.len, stderr);
}

bool Str_eq_cstr(Str s, const char* cstr) {
    return strncmp(s.start, cstr, s.len) == 0;
}

bool Str_eq(Str a, Str b) {
    if (a.len == b.len && a.start == b.start) {
        return true;
    }
    if (a.len != b.len) {
        return false;
    }
    return strncmp(a.start, b.start, a.len) == 0;
}

bool Str_prefix_match(Str haystack, Str needle) {
    Str target = Str_slice(haystack, 0, needle.len);
    return Str_eq(target, needle);
}

#endif // IZIGC_STR_H
