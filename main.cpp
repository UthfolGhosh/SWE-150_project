#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int CELL_SIZE = 20;

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
TTF_Font* gFont = nullptr;

void renderStartScreen() {
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gRenderer);

    drawButton(startButton, " Game Start");

    if (startButton.isClicked) {
        currentState = GAME;
        startButton.isClicked = false;
    }

    SDL_RenderPresent(gRenderer);
}

void renderGameScreen() {
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gRenderer);

    drawSnake();
    drawFood();
    drawScore();

    SDL_RenderPresent(gRenderer);
}

void renderGameOverScreen() {
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255); 
    SDL_RenderClear(gRenderer);

    std::string gameOverText = "Game Over";
    std::string scoreText = "Final Score: " + std::to_string(score);

    SDL_Color textColor = {255, 255, 255, 255};

    SDL_Surface* gameOverSurface = TTF_RenderText_Solid(gFont, gameOverText.c_str(), textColor);
    SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(gRenderer, gameOverSurface);

    SDL_Rect gameOverRect = {SCREEN_WIDTH / 2 - gameOverSurface->w / 2, SCREEN_HEIGHT / 2 - 50, gameOverSurface->w, gameOverSurface->h};
    SDL_RenderCopy(gRenderer, gameOverTexture, nullptr, &gameOverRect);

    SDL_Surface* scoreSurface = TTF_RenderText_Solid(gFont, scoreText.c_str(), textColor);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(gRenderer, scoreSurface);

    SDL_Rect scoreRect = {SCREEN_WIDTH / 2 - scoreSurface->w / 2, SCREEN_HEIGHT / 2 + 20, scoreSurface->w, scoreSurface->h};
    SDL_RenderCopy(gRenderer, scoreTexture, nullptr, &scoreRect);

    SDL_FreeSurface(gameOverSurface);
    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(gameOverTexture);
    SDL_DestroyTexture(scoreTexture);

    SDL_RenderPresent(gRenderer);
}

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
