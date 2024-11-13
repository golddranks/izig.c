#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <assert.h>


typedef struct Str {
    const char* start;
    size_t len;
} Str;

const Str NULL_STR = (Str) { .start = NULL, .len = 0 };

bool is_ws(char c) {
    return c == ' ' || c == '\t' || c == '\n';
}

bool is_alpha(char c) {
    return (65 <= c && c <= 90) || (97 <= c && c <= 122);
}

bool is_alphanum(char c) {
    return is_alpha(c) || (48 <= c && c <= 57);
}

void Str_consume_n(Str* source, int n) {
    source->start += n;
    source->len -= n;
}

bool Str_is_null(Str s) {
    return s.len == 0 && s.start == NULL;
}

size_t Str__match(const Str* source, const char* target) {
    size_t i = 0;
    while (i < source->len && target[i] != '\0' && source->start[i] == target[i]) {
        i++;
    }
    if (target[i] == '\0') {
        return i;
    } else {
        return -1;
    }
}

Str Str_consume_until(Str* source, char target) {
    size_t i = 0;
    while (i < source->len && source->start[i] != target) {
        i++;
    }
    Str consumed = *source;
    consumed.len = i;
    Str_consume_n(source, i);
    return consumed;
}

bool Str_peek(Str* source, const char* target) {
    size_t i = Str__match(source, target);
    return i != -1;
}

bool Str_consume(Str* source, const char* target) {
    size_t i = Str__match(source, target);
    if (i != -1) {
        Str_consume_n(source, i);
        return true;
    } else {
        return false;
    }
}

void Str_consume_ws(Str* source) {
    size_t i = 0;
    while (i < source->len && is_ws(source->start[i])) {
        i++;
    }
    Str_consume_n(source, i);
}

Str Str_consume_ident(Str* source) {
    size_t i = 0;
    while (i < source->len) {
        char c = source->start[i];
        if (!is_alphanum(c) || (i == 0 && !is_alpha(c))) {
            break;
        }
        i++;
    }
    if (i > 0) {
        Str ident = *source;
        ident.len = i;
        Str_consume_n(source, i);
        return ident;
    } else {
        return NULL_STR;
    }
}

Str Str_consume_str(Str* source) {
    Str_consume(source, "\"");
    Str str = Str_consume_until(source, '"');
    Str_consume(source, "\"");
    return str;
}

void Str_print(Str s) {
    fwrite(s.start, sizeof(unsigned char), s.len, stdout);
}

typedef struct InputFile {
    const char* path;
    size_t size;
    FILE* fptr;
    int fd;
    const char* data;
} InputFile;

InputFile InputFile_open(const char* path) {
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        perror("fopen failed");
        exit(1);
    }

    int fd = fileno(file);

    struct stat filestat;

    if (fstat(fd, &filestat) != 0) {
        perror("stat failed");
        exit(2);
    }

    void* data = mmap(NULL, filestat.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap failed");
        exit(3);
    }

    return (InputFile) {
        .path = path,
        .size = filestat.st_size,
        .fptr = file,
        .fd = fd,
        .data = (const char*) data,
    };
}

void InputFile_close(InputFile file) {
    munmap((void*) file.data, file.size);
    fclose(file.fptr);
}

Str InputFile_Str(InputFile file) {
    return (Str) {
        .start = file.data,
        .len = file.size,
    };
}

#endif // UTILS_H


#define DefArray(ArrayName, TypeName, Capacity)                     \
                                                                    \
typedef struct ArrayName {                                          \
    TypeName* buf;                                                  \
    size_t len;                                                    \
} ArrayName;                                                        \
                                                                    \
ArrayName ArrayName##_new() {                                       \
    return (ArrayName) {                                            \
        .buf = malloc(sizeof(TypeName) * Capacity),                 \
        .len = 0                                                   \
    };                                                              \
}                                                                   \
TypeName* ArrayName##_get(ArrayName arr, size_t idx) {              \
    assert(idx < arr.len);                                         \
    return arr.buf + idx;                                           \
}                                                                   \
void ArrayName##_set(ArrayName arr, size_t idx, TypeName data) {    \
    assert(idx < arr.len);                                         \
    arr.buf[idx] = data;                                            \
}                                                                   \
size_t ArrayName##_push(ArrayName* arr, TypeName data) {            \
    assert(arr->len < Capacity);                                   \
    size_t idx = arr->len;                                         \
    arr->len += 1;                                                 \
    ArrayName##_set(*arr, idx, data);                               \
    return idx;                                                     \
}
