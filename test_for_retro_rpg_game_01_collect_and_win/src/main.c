
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define FIELD_WIDTH 2000
#define FIELD_HEIGHT 1600
#define WALL_THICKNESS 20
#define SQUARE_SIZE 50
#define SMALL_SQUARE_SIZE 30
#define SPEED 5
#define NUM_SMALL_SQUARES 4

typedef struct {
    SDL_Rect rect;
    bool collected;
} SmallSquare;

bool check_collision(SDL_Rect a, SDL_Rect b) {
    return SDL_HasIntersection(&a, &b);
}

void update_camera(SDL_Rect* camera, SDL_Rect square, int window_width, int window_height) {
    camera->x = square.x + square.w / 2 - window_width / 2;
    camera->y = square.y + square.h / 2 - window_height / 2;

    if (camera->x < 0) camera->x = 0;
    if (camera->y < 0) camera->y = 0;
    if (camera->x > FIELD_WIDTH - window_width) camera->x = FIELD_WIDTH - window_width;
    if (camera->y > FIELD_HEIGHT - window_height) camera->y = FIELD_HEIGHT - window_height;
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL init error: %s\n", SDL_GetError());
        return 1;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        printf("SDL_image init error: %s\n", IMG_GetError());
        return 1;
    }

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    int WINDOW_WIDTH = display_mode.w;
    int WINDOW_HEIGHT = display_mode.h;

    SDL_Window* window = SDL_CreateWindow("Merged Game",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window creation error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture* texture = IMG_LoadTexture(renderer, "boy.png");
    if (!texture) {
        printf("Image load error: %s\n", IMG_GetError());
        return 1;
    }

    SDL_Rect square = {
        WALL_THICKNESS + 100,
        WALL_THICKNESS + 100,
        SQUARE_SIZE, SQUARE_SIZE
    };

    SDL_Rect walls[4] = {
        { 0, 0, FIELD_WIDTH, WALL_THICKNESS },
        { 0, FIELD_HEIGHT - WALL_THICKNESS, FIELD_WIDTH, WALL_THICKNESS },
        { 0, 0, WALL_THICKNESS, FIELD_HEIGHT },
        { FIELD_WIDTH - WALL_THICKNESS, 0, WALL_THICKNESS, FIELD_HEIGHT }
    };

    SDL_Rect camera = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    update_camera(&camera, square, WINDOW_WIDTH, WINDOW_HEIGHT);

    SmallSquare small_squares[NUM_SMALL_SQUARES];
    srand(time(NULL));
    for (int i = 0; i < NUM_SMALL_SQUARES; i++) {
        small_squares[i].rect.x = WALL_THICKNESS + rand() % (FIELD_WIDTH - 2 * WALL_THICKNESS - SMALL_SQUARE_SIZE);
        small_squares[i].rect.y = WALL_THICKNESS + rand() % (FIELD_HEIGHT - 2 * WALL_THICKNESS - SMALL_SQUARE_SIZE);
        small_squares[i].rect.w = SMALL_SQUARE_SIZE;
        small_squares[i].rect.h = SMALL_SQUARE_SIZE;
        small_squares[i].collected = false;
    }

    bool running = true;
    bool win = false;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
                running = false;
        }

        const Uint8* keystates = SDL_GetKeyboardState(NULL);
        SDL_Rect next_pos = square;

        if (keystates[SDL_SCANCODE_UP]) next_pos.y -= SPEED;
        if (keystates[SDL_SCANCODE_DOWN]) next_pos.y += SPEED;
        if (keystates[SDL_SCANCODE_LEFT]) next_pos.x -= SPEED;
        if (keystates[SDL_SCANCODE_RIGHT]) next_pos.x += SPEED;

        if (next_pos.x < 0) next_pos.x = 0;
        if (next_pos.y < 0) next_pos.y = 0;
        if (next_pos.x + SQUARE_SIZE > FIELD_WIDTH) next_pos.x = FIELD_WIDTH - SQUARE_SIZE;
        if (next_pos.y + SQUARE_SIZE > FIELD_HEIGHT) next_pos.y = FIELD_HEIGHT - SQUARE_SIZE;

        bool blocked = false;
        for (int i = 0; i < 4; i++) {
            if (check_collision(next_pos, walls[i])) {
                blocked = true;
                break;
            }
        }

        if (!blocked)
            square = next_pos;

        for (int i = 0; i < NUM_SMALL_SQUARES; i++) {
            if (!small_squares[i].collected && check_collision(square, small_squares[i].rect)) {
                small_squares[i].collected = true;
                bool all_collected = true;
                for (int j = 0; j < NUM_SMALL_SQUARES; j++) {
                    if (!small_squares[j].collected) {
                        all_collected = false;
                        break;
                    }
                }
                if (all_collected) {
                    win = true;
                }
            }
        }

        update_camera(&camera, square, WINDOW_WIDTH, WINDOW_HEIGHT);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Rect field = { -camera.x, -camera.y, FIELD_WIDTH, FIELD_HEIGHT };
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &field);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (int i = 0; i < 4; i++) {
            SDL_Rect view_wall = {
                walls[i].x - camera.x,
                walls[i].y - camera.y,
                walls[i].w,
                walls[i].h
            };
            SDL_RenderFillRect(renderer, &view_wall);
        }

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (int i = 0; i < NUM_SMALL_SQUARES; i++) {
            if (!small_squares[i].collected) {
                SDL_Rect view_square = {
                    small_squares[i].rect.x - camera.x,
                    small_squares[i].rect.y - camera.y,
                    SMALL_SQUARE_SIZE,
                    SMALL_SQUARE_SIZE
                };
                SDL_RenderFillRect(renderer, &view_square);
            }
        }

        SDL_Rect view_square = {
            square.x - camera.x,
            square.y - camera.y,
            square.w,
            square.h
        };
        SDL_RenderCopy(renderer, texture, NULL, &view_square);

        SDL_RenderPresent(renderer);

        if (win) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Congratulations!", "You win!", window);
            running = false;
        }

        SDL_Delay(16);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
