SRC = cube.pas
BIN = cube
SDL_PATH = -Fi~/tools/SDL2-for-Pascal-2.3-stable/units -Fu~/tools/SDL2-for-Pascal-2.3-stable/units

cube: $(SRC)
	fpc $(SDL_PATH) -o$(BIN) $(SRC)
clean:
	rm ./*.o
