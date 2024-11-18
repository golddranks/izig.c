#ifndef IZIGC_INPUT_FILE_H
#define IZIGC_INPUT_FILE_H

#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <assert.h>
#include "str.h"

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

#endif // IZIGC_INPUT_FILE_H
