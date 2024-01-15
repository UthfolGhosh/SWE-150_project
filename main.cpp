#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int CELL_SIZE = 20;

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
TTF_Font* gFont = nullptr;

int main(int argc, char* args[]) {
    if (!init()) {
        std::cerr << "Failed to initialize!" << std::endl;
        return -1;
    }

    SDL_Event e;

    snake[0].x = 0;
    snake[0].y = 0;

    spawnFood();

    while (currentState != QUIT) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                currentState = QUIT;
            } else if (currentState == START || currentState == GAME) {
                handleInput(e);
            }
        }

        if (currentState == START) {
            renderStartScreen();
        } else if (currentState == GAME && !gameover) {
            moveSnake();
            checkFoodCollision();

            if (checkCollisionWithWalls() || checkCollisionWithItself()) {
                gameover = true;
            }

            renderGameScreen();
        } else if (gameover) {
            renderGameOverScreen();
        }

        SDL_Delay(90);
    }

    close();

    return 0;
}
