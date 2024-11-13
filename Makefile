CC = gcc
CFLAGS = -Iinclude -Wall

SRC = src/main.c
OUT = izig

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

clean:
	rm -f $(OUT)
