#ifndef IZIGC_DEBUG_H
#define IZIGC_DEBUG_H

#include <stdio.h>
#include "str.h"

void __debug_print(const char* msg, Str str, const char* file, int line, const char* func) {
    fprintf(
        stderr,
        "Debug: %s %.*s (at %s:%d:%s)\n",
        msg,
        (int) str.len,
        str.start,
        file,
        line,
        func
    );
}

#define debug(msg) __debug_print(msg, EMPTY_STR, __FILE__, __LINE__, __func__);
#define debug_str(msg, str) __debug_print(msg, str, __FILE__, __LINE__, __func__);
#define todo() fprintf(stderr, "TODO: %s:%d:%s\n", __FILE__, __LINE__, __func__); exit(0);

#endif // IZIGC_DEBUG_H
