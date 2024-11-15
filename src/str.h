#ifndef IZIGC_STR_H
#define IZIGC_STR_H

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

bool is_ws(char c) {
    return c == ' ' || c == '\t' || c == '\n';
}

bool is_alpha(char c) {
    return (65 <= c && c <= 90) || (97 <= c && c <= 122);
}

bool is_alphanum(char c) {
    return is_alpha(c) || (48 <= c && c <= 57);
}


typedef struct Str {
    const char* start;
    size_t len;
} Str;

const Str EMPTY_STR = (Str) { .start = "", .len = 0 };

Str Str_new(const char* charp) {
    return (Str) { .start = charp, .len = strlen(charp) };
}

void Str_consume_n(Str* source, int n) {
    source->start += n;
    source->len -= n;
}

bool Str_is_null(Str s) {
    return s.len == 0 && s.start == NULL;
}

size_t Str_prefix_match(Str haystack, Str needle) {
    size_t i = 0;
    if (haystack.len < needle.len) {
        return -1;
    }
    while (i < needle.len && haystack.start[i] == needle.start[i]) {
        i++;
    }
    if (i == needle.len) {
        return i;
    } else {
        return -1;
    }
}

void Str_print(Str s) {
    fwrite(s.start, sizeof(unsigned char), s.len, stdout);
}

bool Str_eq_c_str(Str s, const char* charp) {
    return strncmp(s.start, charp, s.len) == 0;
}

#endif // IZIGC_STR_H
