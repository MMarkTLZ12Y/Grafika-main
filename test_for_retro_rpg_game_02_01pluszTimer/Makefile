all: hello_sdl.exe

hello_sdl.exe: src/main.c
	gcc src/main.c -o hello_sdl.exe -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

linux: hello_sdl
hello_sdl: src/main.c
	gcc src/main.c -o hello_sdl -lSDL2main -lSDL2 -lSDL2_image

clean:
	rm -f hello_sdl.exe hello_sdl
