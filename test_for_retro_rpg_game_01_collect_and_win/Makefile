# Alapértelmezett szabály
all: hello_sdl.exe

# Windows build
hello_sdl.exe: src/main.c
	gcc src/main.c -o hello_sdl.exe -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

# Linux build
linux: hello_sdl
hello_sdl: src/main.c
	gcc src/main.c -o hello_sdl -lSDL2main -lSDL2 -lSDL2_image

# Takarítás
clean:
	rm -f hello_sdl.exe hello_sdl
