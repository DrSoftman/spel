# Makefile for SDL2-gamae with SDL2_image

CC = gcc
CFLAGS = -Wall
LDFLAGS = -lSDL2 -lSDL2_image

SRC = src/spel.c
OUT = spel-build

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

run: $(OUT)
	./$(OUT)

clean:
	rm -f $(OUT)

