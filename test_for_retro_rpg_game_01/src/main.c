#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 1200
#define FIELD_WIDTH 400
#define FIELD_HEIGHT 600
#define SQUARE_SIZE 50
#define SMALL_SQUARE_SIZE 30
#define SPEED 5
#define NUM_SMALL_SQUARES 4

typedef struct {
    SDL_Rect rect;
    bool collected;
} SmallSquare;

// Function to check collision between two rectangles
bool check_collision(SDL_Rect a, SDL_Rect b) {
    return SDL_HasIntersection(&a, &b);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL init error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_image (for loading .png textures with transparency)
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        printf("SDL_image init error: %s\n", IMG_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Textured Square with Small Red Squares",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    if (!window) {
        printf("Window creation error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load PNG image with transparency support
    SDL_Texture* texture = IMG_LoadTexture(renderer, "boy.png");
    if (!texture) {
        printf("Image load error: %s\n", IMG_GetError());
        return 1;
    }

    // Set up the field and square
    int field_offset_x = (WINDOW_WIDTH - FIELD_WIDTH) / 2;
    int field_offset_y = (WINDOW_HEIGHT - FIELD_HEIGHT) / 2;

    SDL_Rect square = {
        field_offset_x + FIELD_WIDTH / 2 - SQUARE_SIZE / 2,
        field_offset_y + FIELD_HEIGHT / 2 - SQUARE_SIZE / 2,
        SQUARE_SIZE, SQUARE_SIZE
    };

    // Create small red squares
    SmallSquare small_squares[NUM_SMALL_SQUARES];
    srand(time(NULL));  // Seed for random position generation

    for (int i = 0; i < NUM_SMALL_SQUARES; i++) {
        small_squares[i].rect.x = field_offset_x + rand() % (FIELD_WIDTH - SMALL_SQUARE_SIZE);
        small_squares[i].rect.y = field_offset_y + rand() % (FIELD_HEIGHT - SMALL_SQUARE_SIZE);
        small_squares[i].rect.w = SMALL_SQUARE_SIZE;
        small_squares[i].rect.h = SMALL_SQUARE_SIZE;
        small_squares[i].collected = false;
    }

    bool running = true;
    bool win = false;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        if (square.y - SPEED >= field_offset_y)
                            square.y -= SPEED;
                        break;
                    case SDLK_DOWN:
                        if (square.y + SQUARE_SIZE + SPEED <= field_offset_y + FIELD_HEIGHT)
                            square.y += SPEED;
                        break;
                    case SDLK_LEFT:
                        if (square.x - SPEED >= field_offset_x)
                            square.x -= SPEED;
                        break;
                    case SDLK_RIGHT:
                        if (square.x + SQUARE_SIZE + SPEED <= field_offset_x + FIELD_WIDTH)
                            square.x += SPEED;
                        break;
                }
            }
        }

        // Check for collisions with small squares
        for (int i = 0; i < NUM_SMALL_SQUARES; i++) {
            if (!small_squares[i].collected && check_collision(square, small_squares[i].rect)) {
                small_squares[i].collected = true;
                // If all small squares are collected, the player wins
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

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Clear screen with black
        SDL_RenderClear(renderer);

        // Draw the playfield
        SDL_Rect playfield = { field_offset_x, field_offset_y, FIELD_WIDTH, FIELD_HEIGHT };
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // Green
        SDL_RenderFillRect(renderer, &playfield);

        // Draw the small red squares
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Red
        for (int i = 0; i < NUM_SMALL_SQUARES; i++) {
            if (!small_squares[i].collected) {
                SDL_RenderFillRect(renderer, &small_squares[i].rect);
            }
        }

        // Draw the textured square (character)
        SDL_RenderCopy(renderer, texture, NULL, &square);

        SDL_RenderPresent(renderer);  // Present the rendered image

        // Display "You win!" if the player collected all small squares
        if (win) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Congratulations!", "You win!", window);
            running = false;  // End the game
        }

        SDL_Delay(16);  // Delay for ~60 FPS
    }

    // Clean up
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
