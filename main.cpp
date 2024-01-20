#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int CELL_SIZE = 20;

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
TTF_Font* gFont = nullptr;
SDL_Surface* surface = nullptr;
SDL_Texture* Texture = nullptr;

enum GameState {
    START,
    GAME,
    GAME_OVER,
    QUIT
};

GameState currentState = START;

struct Snake {
    int x, y;
};

struct Button {
    SDL_Rect rect;
    bool isClicked;
};

Button startButton;

Snake snake[100];
int snakeLength = 1;
int direction = 1;

SDL_Point food;
int score = 0;
int count = 0;

bool gameover = false;

//Initialization window,sdl_TTF
bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    gWindow = SDL_CreateWindow("SnakeGame_Project", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (gRenderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    gFont = TTF_OpenFont("uthfol.ttf", 24);
    if (gFont == nullptr) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    startButton.rect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, 200, 100};
    startButton.isClicked = false;

    return true;
}

//Colse program function
void close() {
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    TTF_CloseFont(gFont);
    TTF_Quit();
    SDL_Quit();
}

//Food position
void spawnFood() {
    food.x = rand() % (SCREEN_WIDTH / CELL_SIZE) * CELL_SIZE;
    food.y = rand() % (SCREEN_HEIGHT / CELL_SIZE) * CELL_SIZE;
}

void BigspawnFood() {
    food.x = (rand() % (SCREEN_WIDTH / CELL_SIZE) * CELL_SIZE);
    food.y = (rand() % (SCREEN_HEIGHT / CELL_SIZE) * CELL_SIZE);
}

void drawSnake() {
    for (int i = 0; i < snakeLength; ++i) {

        if(i==0){
         SDL_Rect snakeRect = {snake[i].x, snake[i].y, CELL_SIZE, CELL_SIZE};
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 255, 0);
        SDL_RenderFillRect(gRenderer, &snakeRect);

        surface = SDL_LoadBMP("Head.bmp");
	    Texture =SDL_CreateTextureFromSurface(gRenderer,surface);
	    SDL_FreeSurface(surface);
	    SDL_RenderCopy(gRenderer,Texture,NULL,&snakeRect);
        }
        else{
            SDL_Rect snakeRect = {snake[i].x, snake[i].y, CELL_SIZE, CELL_SIZE};
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 255, 0);
        SDL_RenderFillRect(gRenderer, &snakeRect);

        surface = SDL_LoadBMP("Body.bmp");
	    Texture =SDL_CreateTextureFromSurface(gRenderer,surface);
	    SDL_FreeSurface(surface);
	    SDL_RenderCopy(gRenderer,Texture,NULL,&snakeRect);
        }
        
    }
}

void drawFood() {
    SDL_Rect foodRect = {food.x, food.y, CELL_SIZE, CELL_SIZE};
    SDL_SetRenderDrawColor(gRenderer, 255, 0, 255, 0); 
    SDL_RenderFillRect(gRenderer, &foodRect);
}

void BigdrawFood()
{    
    
     SDL_Rect foodRect = {food.x, food.y, CELL_SIZE, CELL_SIZE};
    SDL_SetRenderDrawColor(gRenderer, 0, 255, 255 , 0); 
    SDL_RenderFillRect(gRenderer, &foodRect);
   
}

//display score
void drawScore() {
    SDL_Color textColor = {0, 255, 0, 0};
    std::string scoreText = "Score: " + std::to_string(score);

    SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, scoreText.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);

    SDL_Rect textRect = {5, 2, textSurface->w, textSurface->h};
    SDL_RenderCopy(gRenderer, textTexture, nullptr, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

//Draw the Game start button
void drawButton(const Button& button, const std::string& buttonText) {
    SDL_SetRenderDrawColor(gRenderer, 255, 110, 0, 220);
    SDL_RenderFillRect(gRenderer, &button.rect);

    SDL_Color textColor = {0, 0,255, 0};
    SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, buttonText.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);

    SDL_Rect textRect = {button.rect.x + button.rect.w / 2 - textSurface->w / 2,
                         button.rect.y + button.rect.h / 2 - textSurface->h / 2,
                         textSurface->w, textSurface->h};
    SDL_RenderCopy(gRenderer, textTexture, nullptr, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

bool checkCollisionWithButton(const Button& button, int mouseX, int mouseY) {
    return mouseX >= button.rect.x && mouseX < button.rect.x + button.rect.w &&
           mouseY >= button.rect.y && mouseY < button.rect.y + button.rect.h;
}

//Handle input
void handleInput(SDL_Event& e) {
    switch (e.type) {
        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button == SDL_BUTTON_LEFT) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                switch (currentState) {
                    case START:
                        if (checkCollisionWithButton(startButton, mouseX, mouseY)) {
                            startButton.isClicked = true;
                        }
                        break;
                }
            }
            break;

        case SDL_KEYDOWN:
            switch (e.key.keysym.sym) {
                case SDLK_UP:
                    if (currentState == GAME && direction != 2) {
                        direction = 0;
                    }
                    break;
                case SDLK_DOWN:
                    if (currentState == GAME && direction != 0) {
                        direction = 2;
                    }
                    break;
                case SDLK_LEFT:
                    if (currentState == GAME && direction != 1) {
                        direction = 3;
                    }
                    break;
                case SDLK_RIGHT:
                    if (currentState == GAME && direction != 3) {
                        direction = 1;
                    }
                    break;
                
            }
            break;
    }
}

//Snake update part
void moveSnake() {
    for (int i = snakeLength - 1; i > 0; --i) {
        snake[i] = snake[i - 1];
    }

    switch (direction) {
        case 0:
            snake[0].y -= CELL_SIZE;
            break;
        case 1:
            snake[0].x += CELL_SIZE;
            break;
        case 2:
            snake[0].y += CELL_SIZE;
            break;
        case 3:
            snake[0].x -= CELL_SIZE;
            break;
    }
}


void checkFoodCollision() {
    if (snake[0].x == food.x && snake[0].y == food.y) {
        snakeLength++;

        if(count==3 )
        {
          BigspawnFood();
        score += 10;
        count = 0;
        }
        else{
        spawnFood();
        score += 3;
        count ++;
        }
    }
}

bool checkCollisionWithWalls() {
    return snake[0].x < 0 || snake[0].x >= SCREEN_WIDTH || snake[0].y < 0 || snake[0].y >= SCREEN_HEIGHT;
}

bool checkCollisionWithItself() {
    for (int i = 1; i < snakeLength; ++i) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            return true;
        }
    }
    return false;
}

void renderStartScreen() {
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gRenderer);

    surface = SDL_LoadBMP("./Backgroud.bmp");
	Texture =SDL_CreateTextureFromSurface(gRenderer,surface);
	SDL_FreeSurface(surface);
	SDL_RenderCopy(gRenderer,Texture,NULL,NULL);

    drawButton(startButton, " Game Start");

    if (startButton.isClicked) {
        currentState = GAME;
        startButton.isClicked = false;
    }

    SDL_RenderPresent(gRenderer);

    surface = nullptr;
    Texture = nullptr;
}

void renderGameScreen() {
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gRenderer);

    surface = SDL_LoadBMP("sscreenn.bmp");
	Texture =SDL_CreateTextureFromSurface(gRenderer,surface);
	SDL_FreeSurface(surface);
	SDL_RenderCopy(gRenderer,Texture,NULL,NULL);

    drawSnake();

    if(count==3)
    {
      BigdrawFood();
    }
    else
    drawFood();
    drawScore();

    SDL_RenderPresent(gRenderer);

    surface = nullptr;
    Texture = nullptr;
}

void renderGameOverScreen() {
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255); 
    SDL_RenderClear(gRenderer);

	surface = SDL_LoadBMP("Backgroud.bmp");
	Texture =SDL_CreateTextureFromSurface(gRenderer,surface);
	SDL_FreeSurface(surface);
	SDL_RenderCopy(gRenderer,Texture,NULL,NULL);

    std::string gameOverText = "Game Over!!!";
    std::string scoreText = "YOUR FINAL SCORE: " + std::to_string(score);

    SDL_Color textColor = {100,150,200,250};

    SDL_Surface* gameOverSurface = TTF_RenderText_Solid(gFont, gameOverText.c_str(), textColor);
    SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(gRenderer, gameOverSurface);

    SDL_Rect gameOverRect = {SCREEN_WIDTH / 2 - gameOverSurface->w / 2, SCREEN_HEIGHT / 2 - 70, gameOverSurface->w, gameOverSurface->h};
    SDL_RenderCopy(gRenderer, gameOverTexture, nullptr, &gameOverRect);

    SDL_Surface* scoreSurface = TTF_RenderText_Solid(gFont, scoreText.c_str(), textColor);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(gRenderer, scoreSurface);

    SDL_Rect scoreRect = {SCREEN_WIDTH / 2 - scoreSurface->w / 2, SCREEN_HEIGHT / 2 -20, scoreSurface->w, scoreSurface->h};
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

        SDL_Delay(200);
    }
   close();

    return 0;
}
