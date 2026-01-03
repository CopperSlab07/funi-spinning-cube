SRC = main.c

CFLAGS = -std=c11 -Wall -Wextra -pedantic -DPRELOAD_CONFIG
SDL2-CFLAGS = -I/usr/include/SDL2 -D_GNU_SOURCE=1 -D_REENTRANT
LINKERS = -lm -lSDL2 -llua

build: $(SRC)
	cc $(CFLAGS) $(SDL2-CFLAGS) -o run $(SRC) $(LINKERS)
