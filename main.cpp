#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 600;
const int CELL_SIZE = 20;

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;

enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

struct SnakeSegment {
    int x, y;
};

std::vector<SnakeSegment> snake;
Direction currentDirection = Direction::RIGHT;

SDL_Point food;

bool Initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    gWindow = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (gRenderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void Close() {
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    SDL_Quit();
}

void SpawnFood() {
    food.x = rand() % (SCREEN_WIDTH / CELL_SIZE) * CELL_SIZE;
    food.y = rand() % (SCREEN_HEIGHT / CELL_SIZE) * CELL_SIZE;
}

void DrawSnake() {
    for (const auto& segment : snake) {
        SDL_Rect rect = { segment.x, segment.y, CELL_SIZE, CELL_SIZE };
        SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
        SDL_RenderFillRect(gRenderer, &rect);
    }
}

void DrawFood() {
    SDL_Rect rect = { food.x, food.y, CELL_SIZE, CELL_SIZE };
    SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
    SDL_RenderFillRect(gRenderer, &rect);
}

void MoveSnake() {
    SnakeSegment head = snake.front();

    switch (currentDirection) {
    case Direction::UP:
        head.y -= CELL_SIZE;
        break;
    case Direction::DOWN:
        head.y += CELL_SIZE;
        break;
    case Direction::LEFT:
        head.x -= CELL_SIZE;
        break;
    case Direction::RIGHT:
        head.x += CELL_SIZE;
        break;
    }

    snake.insert(snake.begin(), head);

    if (head.x == food.x && head.y == food.y) {
        SpawnFood();
    } else {
        snake.pop_back();
    }
}

bool CheckCollision() {
    SnakeSegment head = snake.front();

    if (head.x < 0 || head.y < 0 || head.x >= SCREEN_WIDTH || head.y >= SCREEN_HEIGHT) {
        return true; // Collision with screen boundary
    }

    for (auto it = snake.begin() + 1; it != snake.end(); ++it) {
        if (it->x == head.x && it->y == head.y) {
            return true; // Collision with own body
        }
    }

    return false;
}

int main(int argc, char* args[]) {
    if (!Initialize()) {
        return -1;
    }

    snake.push_back({ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 }); // Initial position of the snake
    SpawnFood();

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_UP:
                    if (currentDirection != Direction::DOWN) currentDirection = Direction::UP;
                    break;
                case SDLK_DOWN:
                    if (currentDirection != Direction::UP) currentDirection = Direction::DOWN;
                    break;
                case SDLK_LEFT:
                    if (currentDirection != Direction::RIGHT) currentDirection = Direction::LEFT;
                    break;
                case SDLK_RIGHT:
                    if (currentDirection != Direction::LEFT) currentDirection = Direction::RIGHT;
                    break;
                }
            }
        }

        MoveSnake();

        if (CheckCollision()) {
            quit = true;
        }

        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
        SDL_RenderClear(gRenderer);

        DrawSnake();
        DrawFood();

        SDL_RenderPresent(gRenderer);

        SDL_Delay(100); // Adjust the delay to control snake speed
    }

    Close();

    return 0;
}
