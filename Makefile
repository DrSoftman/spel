# Makefile för SDL2-spel med SDL2_image

CC = gcc
CFLAGS = -Wall
LDFLAGS = -lSDL2 -lSDL2_image

SRC = src/spel.c
OUT = spel-build

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

clean:
	rm -f $(OUT)

