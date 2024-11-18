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

.PHONY: test

TESTCASE = ./$(OUT) test/$1.zig | diff test/$1.stdout - && echo "$1: OK" >> test_results.out || echo "$1: FAIL" >> test_results.out;

test: $(OUT)
	echo "" > test_results.out
	$(call TESTCASE,hello)
	$(call TESTCASE,funcs_and_nums)
	$(call TESTCASE,ziggzagg)
	cat test_results.out
	grep -q "FAIL" test_results.out && exit 1 || exit 0
