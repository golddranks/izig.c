CC = gcc
CFLAGS = -Iinclude -Wall -g

SRC = src/main.c
HEADERS = src/array.h src/input_file.h src/str.h src/lexer.h src/parser.h
OUT = izig

all: $(OUT)

$(OUT): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

clean:
	rm -f $(OUT)
