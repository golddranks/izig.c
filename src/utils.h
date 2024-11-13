#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdbool.h>


typedef struct str {
    const char* start;
    size_t len;
} str;

const str STR_NULL = { .start = NULL, .len = 0 };

void str_consume(str* source, int n) {
    source->start += n;
    source->len -= n;
}

bool str_consume_str(str* source, char* target) {
    size_t i = 0;
    while(i < source->len && target[i] != '\0' && source->start[i] == target[i]) {
        i++;
    }
    if(target[i] == '\0') {
        str_consume(source, i);
        return true;
    } else {
        return false;
    }
}

void str_consume_ws(str* source) {
    size_t i = 0;
    while(i < source->len && (source->start[i] == ' ' || source->start[i] == '\t' || source->start[i] == '\n')) {
        i++;
    }
    str_consume(source, i);
}

void str_consume_until(str* source, char target) {
    size_t i = 0;
    while(i < source->len && source->start[i] != target) {
        i++;
    }
    str_consume(source, i);
}

void str_print(str s) {
    fwrite(s.start, sizeof(unsigned char), s.len, stdout);
}

typedef struct ifile {
    const char* path;
    size_t size;
    FILE* fptr;
    int fd;
    const char* data;
} ifile;

ifile ifile_open(const char* path) {
    FILE* file = fopen(path, "r");
    if(file == NULL) {
        perror("fopen failed");
        exit(1);
    }

    int fd = fileno(file);

    struct stat filestat;

    if(fstat(fd, &filestat) != 0) {
        perror("stat failed");
        exit(1);
    }

    void* data = mmap(NULL, filestat.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if(data == MAP_FAILED) {
        perror("mmap failed");
        exit(2);
    }

    return (ifile) {
        .path = path,
        .size = filestat.st_size,
        .fptr = file,
        .fd = fd,
        .data = (const char*) data,
    };
}

void ifile_close(ifile file) {
    munmap((void*) file.data, file.size);
    fclose(file.fptr);
}

str ifile_str(ifile file) {
    return (str) {
        .start = file.data,
        .len = file.size,
    };
}

#endif // UTILS_H
