#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

SDL_Window* window;
SDL_GLContext context;

void initOpenGL() {
    glEnable(GL_DEPTH_TEST);  // Engedélyezzük a mélységi tesztet
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Háttér szín
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0f, 1.0f, 0.1f, 100.0f);  // Perspektívikus nézet
    glMatrixMode(GL_MODELVIEW);
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Töröljük a képernyőt
    glLoadIdentity();  // Töröljük az előző transzformációkat

    glTranslatef(0.0f, 0.0f, -5.0f);  // Mozgatjuk a kamerát

    glBegin(GL_QUADS);  // Kocka rajzolása
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(-1.0f, -1.0f, -1.0f);
        glVertex3f( 1.0f, -1.0f, -1.0f);
        glVertex3f( 1.0f,  1.0f, -1.0f);
        glVertex3f(-1.0f,  1.0f, -1.0f);
    glEnd();

    SDL_GL_SwapWindow(window);  // Frissítjük az ablakot
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);  // SDL inicializálása

    window = SDL_CreateWindow("OpenGL 3D Scene", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
    context = SDL_GL_CreateContext(window);  // OpenGL kontextus létrehozása

    initOpenGL();

    SDL_Event e;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }

        renderScene();  // Rajzolás
        SDL_Delay(16);  // Képkocka frissítés (60 FPS)
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
