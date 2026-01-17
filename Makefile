SRC = cube.pas
BIN = cube

SDL_SRC = sdl.pas
SDL_PATH = -Fi~/tools/SDL2-for-Pascal-2.3-stable/units -Fu~/tools/SDL2-for-Pascal-2.3-stable/units
SDL_BIN = test

cube: $(SRC)
	fpc $(SDL_PATH) -o$(BIN) $(SRC)
clean:
	rm ./*.o
