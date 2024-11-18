CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -Wpedantic -std=gnu99 -g

SRC = src/main.c
UTILS = src/utils.h src/utils/array.h src/utils/input_file.h src/utils/str.h src/utils/debug.h
HEADERS = src/lexer.h src/parser.h src/runner.h src/runtime.h src/foreign_fn.h $(UTILS)
OUT = izig

all: $(OUT)

$(OUT): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

clean:
	rm -f $(OUT)
