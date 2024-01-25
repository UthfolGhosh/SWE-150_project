#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
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
Button restartButton;
Button exitButton; 

Snake snake[100];
int snakeLength = 1;
int direction = 1; // 0: up, 1: right, 2: down, 3: left

SDL_Point food;

int score = 0;
int level = 0;
int temp = 15;
int High=0;

bool gameover = false;

struct Obstacle {
    SDL_Rect rect;
};

Obstacle obstacles[4]; 

bool init() {
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

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    gFont = TTF_OpenFont("bold.ttf", 24);
    if (gFont == nullptr) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    startButton.rect = {SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 -150, 400, 200};
    startButton.isClicked = false;

    exitButton.rect = {SCREEN_WIDTH / 2-10 , SCREEN_HEIGHT / 2 + 70,90, 30};
    exitButton.isClicked = false;

    restartButton.rect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 70,90, 30};
    restartButton.isClicked = false;


    return true;
}

void close() {
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    TTF_CloseFont(gFont);
    TTF_Quit();
    SDL_Quit();
}

void drawObstacles() {
    for (int i = 0; i < 4; ++i) {
        SDL_SetRenderDrawColor(gRenderer,10,180,140,100);
        SDL_RenderFillRect(gRenderer, &obstacles[i].rect);
    }
}

bool initObstacles() {
    // Initialize obstacle positions
    obstacles[0].rect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, 200, 30};
    obstacles[1].rect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 150, 30, 300};
    obstacles[2].rect = {SCREEN_WIDTH / 2 + 100, SCREEN_HEIGHT / 2 - 150, 30, 300};
    obstacles[3].rect = {0, 0, SCREEN_WIDTH, 40};

    return true;
}

//SDL_Rect obstagolrect1 = {SCREEN_WIDTH/2-100,SCREEN_HEIGHT/2,200,30};
// SDL_Rect obstagolrect2 = {SCREEN_WIDTH/2-100,SCREEN_HEIGHT/2-150,30,300};
//SDL_Rect obstagolrect3 = {SCREEN_WIDTH/2+100,SCREEN_HEIGHT/2-150,30,300};

bool checkCollisionWithobstragol() {

    if(snake[0].x >SCREEN_WIDTH/2-100 && snake[0].x <= SCREEN_WIDTH/2+100 && snake[0].y <SCREEN_HEIGHT/2+30 && snake[0].y >= SCREEN_HEIGHT/2)
    {
      return true;
    }

    if(snake[0].x >=SCREEN_WIDTH/2-105 && snake[0].x <= SCREEN_WIDTH/2-70 && snake[0].y <=SCREEN_HEIGHT/2+150 && snake[0].y >= SCREEN_HEIGHT/2-155)
    {
      return true;
    }

    if(snake[0].x >=SCREEN_WIDTH/2+95 && snake[0].x <= SCREEN_WIDTH/2+130 && snake[0].y <=SCREEN_HEIGHT/2+150 && snake[0].y >= SCREEN_HEIGHT/2-155)
    {
      return true;
    }

    return false;
    
}

bool checkCollisionWithFoodobstragol()
{
    if(food.x >SCREEN_WIDTH/2-100 && food.x <= SCREEN_WIDTH/2+100 && food.y <SCREEN_HEIGHT/2+30 && food.y >= SCREEN_HEIGHT/2)
    {
      return true;
    }

    if(food.x >=SCREEN_WIDTH/2-105 && food.x <= SCREEN_WIDTH/2-70 && food.y <=SCREEN_HEIGHT/2+150 && food.y >= SCREEN_HEIGHT/2-155)
    {
      return true;
    }

    if(food.x >=SCREEN_WIDTH/2+95 && food.x <= SCREEN_WIDTH/2+130 && food.y <=SCREEN_HEIGHT/2+150 && food.y >= SCREEN_HEIGHT/2-155)
    {
      return true;
    }

    if(food.x >=0 && food.x <=SCREEN_WIDTH && food.y <=40 && food.y >=0)
    {
      return true;
    }

    return false;
}

void spawnFood() {
    food.x = rand() % (SCREEN_WIDTH / CELL_SIZE) * CELL_SIZE;
    food.y = rand() % (SCREEN_HEIGHT / CELL_SIZE) * CELL_SIZE;
    
    while(checkCollisionWithFoodobstragol()){
    food.x = rand() % (SCREEN_WIDTH / CELL_SIZE) * CELL_SIZE;
    food.y = rand() % (SCREEN_HEIGHT / CELL_SIZE) * CELL_SIZE;
    }

}

void drawSnake() {
    for (int i = 0; i < snakeLength; ++i) {

       if(i==0){
         SDL_Rect snakeRect = {snake[i].x, snake[i].y, CELL_SIZE, CELL_SIZE};
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 255, 0);
        SDL_RenderFillRect(gRenderer, &snakeRect);

        surface = SDL_LoadBMP("Head.bmp");
	    Texture =SDL_CreateTextureFromSurface(gRenderer,surface);
	    SDL_RenderCopy(gRenderer,Texture,NULL,&snakeRect);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(Texture);
        }
        else{
        SDL_Rect snakeRect = {snake[i].x, snake[i].y, CELL_SIZE, CELL_SIZE};
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 255, 0);
        SDL_RenderFillRect(gRenderer, &snakeRect);

        surface = SDL_LoadBMP("Body.bmp");
	    Texture =SDL_CreateTextureFromSurface(gRenderer,surface);
	    SDL_RenderCopy(gRenderer,Texture,NULL,&snakeRect);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(Texture);
        }
        
    }
}

void drawFood() {
    SDL_Rect foodRect = {food.x, food.y, CELL_SIZE, CELL_SIZE};
    SDL_SetRenderDrawColor(gRenderer, 200,200,0, 0);
    SDL_RenderFillRect(gRenderer, &foodRect);
}

void drawScore() {
    SDL_Color textColor = {255, 250, 0, 0};
    
    // Score text
    std::string scoreText = "Score: " + std::to_string(score);
    SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, scoreText.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);

    SDL_Rect textRect = {10, 7, textSurface->w, textSurface->h};
    SDL_RenderCopy(gRenderer, textTexture, nullptr, &textRect);

    // Level text
    std::string levelText = "Your Level: " + std::to_string(level);
    SDL_Surface* levelTextSurface = TTF_RenderText_Solid(gFont, levelText.c_str(), textColor);
    SDL_Texture* levelTextTexture = SDL_CreateTextureFromSurface(gRenderer, levelTextSurface);

    SDL_Rect levelTextRect = {SCREEN_WIDTH - 190, 7, levelTextSurface->w, levelTextSurface->h};
    SDL_RenderCopy(gRenderer, levelTextTexture, nullptr, &levelTextRect);

    // Highscore text
    std::string highscoreText = "HIGH Score: " + std::to_string(High);
    SDL_Surface* highscoreTextSurface = TTF_RenderText_Solid(gFont, highscoreText.c_str(), textColor);
    SDL_Texture* highscoreTextTexture = SDL_CreateTextureFromSurface(gRenderer, highscoreTextSurface);

    SDL_Rect highscoreTextRect = {SCREEN_WIDTH / 2 - 100, 7, highscoreTextSurface->w, highscoreTextSurface->h};
    SDL_RenderCopy(gRenderer, highscoreTextTexture, nullptr, &highscoreTextRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(levelTextSurface);
    SDL_DestroyTexture(levelTextTexture);
    SDL_FreeSurface(highscoreTextSurface);
    SDL_DestroyTexture(highscoreTextTexture);
}


void drawButton(const Button& button, const std::string& buttonText) {

    SDL_Color textColor = {0,0, 0, 255};//{255,100, 160, 220}
    SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, buttonText.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);

    SDL_Rect textRect = {button.rect.x + button.rect.w / 2 - textSurface->w / 2,
                         button.rect.y + button.rect.h / 2 - textSurface->h / 2,
                         textSurface->w+20, textSurface->h+30};
    SDL_RenderCopy(gRenderer, textTexture, nullptr, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

bool checkCollisionWithButton(const Button& button, int mouseX, int mouseY) {
    return mouseX >= button.rect.x && mouseX < button.rect.x + button.rect.w &&
           mouseY >= button.rect.y && mouseY < button.rect.y + button.rect.h;
}

void resetGame() {
    // Reset the game state and variables
    currentState = GAME;
    snake[0].x = 0;
    snake[0].y = 0;
    snakeLength = 1;
    direction = 1;
    level = 0;
    score = 0;
    gameover = false;
    spawnFood();
}


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
                    case GAME_OVER:
                        if (checkCollisionWithButton(exitButton, mouseX, mouseY)) {
                            exitButton.isClicked = true;
                               currentState = QUIT;
                        }
                        else if(checkCollisionWithButton(restartButton, mouseX, mouseY)) 
                        {
                            currentState = GAME;
                            gameover = false;
                            restartButton.isClicked = false;
                            SDL_DestroyRenderer(gRenderer);
                            SDL_DestroyWindow(gWindow);
                            TTF_CloseFont(gFont);
                            init();  // Initialize the game again
                            resetGame();
                            spawnFood();
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

void moveSnake() {
    for (int i = snakeLength - 1; i > 0; --i) {
        snake[i] = snake[i - 1];
    }

    switch (direction) {
        case 0:
            snake[0].y -= CELL_SIZE;
            if (snake[0].y < 0) {
                snake[0].y = SCREEN_HEIGHT - CELL_SIZE; // Wrap around to the bottom
                  score -=3;
            }
            break;
        case 1:
            snake[0].x += CELL_SIZE;
            if (snake[0].x >= SCREEN_WIDTH) {
                snake[0].x = 0; // Wrap around to the left
                  score -=3;
            }
            break;
        case 2:
            snake[0].y += CELL_SIZE;
            if (snake[0].y >= SCREEN_HEIGHT) {
                snake[0].y = 0; // Wrap around to the top
                  score -=3;
            }
            break;
        case 3:
            snake[0].x -= CELL_SIZE;
            if (snake[0].x < 0) {
                snake[0].x = SCREEN_WIDTH - CELL_SIZE; // Wrap around to the right
                  score -=3;
            }
            break;
    }
}

void checkFoodCollision() {
    if (snake[0].x == food.x && snake[0].y == food.y) {
        snakeLength++;
        score += 5;

    
        if(temp<=score)
        {
          level +=1;
          temp +=15;
        }

        if(High < score)
        {
            High = score;
        }

        spawnFood();
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

     surface = SDL_LoadBMP("startboard.bmp");
	 Texture =SDL_CreateTextureFromSurface(gRenderer,surface);
	 SDL_FreeSurface(surface);
	 SDL_RenderCopy(gRenderer,Texture,NULL,NULL);

    std::string Welcometext = "Welcome to Snake Game!";
    SDL_Color textcolor = {110,0,0,255};

    SDL_Surface* WelcomeSurface = TTF_RenderText_Solid(gFont, Welcometext.c_str(), textcolor);
    SDL_Texture* WelcomeTexture = SDL_CreateTextureFromSurface(gRenderer, WelcomeSurface);

    SDL_Rect WelcomeRect = {SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 -180, 300, 100};
    SDL_RenderCopy(gRenderer, WelcomeTexture, nullptr, &WelcomeRect);

    SDL_FreeSurface(WelcomeSurface);
    SDL_DestroyTexture(WelcomeTexture);

    drawButton(startButton, "Play GAME");

    if (startButton.isClicked) {
        resetGame();
        currentState = GAME;
        startButton.isClicked = false;
    }

    SDL_RenderPresent(gRenderer);
}

void renderGameScreen() {
    SDL_SetRenderDrawColor(gRenderer, 60,130, 110, 30);
    SDL_RenderClear(gRenderer);

    drawObstacles();
    drawScore();
    drawSnake();
    drawFood();

    SDL_RenderPresent(gRenderer);
}

void renderGameOverScreen() {
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255); // Black color
    SDL_RenderClear(gRenderer);

    surface = SDL_LoadBMP("overboard.bmp");
	 Texture =SDL_CreateTextureFromSurface(gRenderer,surface);
	 SDL_FreeSurface(surface);
	 SDL_RenderCopy(gRenderer,Texture,NULL,NULL);

    std::string gameOverText = "GAME OVER!!!";
    
    std::string OverText2 = "Do you Play Again?";

    std::string scoreText = "FINAL Score: " + std::to_string(score);

    SDL_Color textColor = {255,110,110,255};
    SDL_Color OptiontextColor = {0,0,0,255};

    SDL_Surface* gameOverSurface = TTF_RenderText_Solid(gFont, gameOverText.c_str(), textColor);
    SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(gRenderer, gameOverSurface);

    SDL_Rect gameOverRect = {SCREEN_WIDTH / 2 - gameOverSurface->w / 2-10, SCREEN_HEIGHT / 2 - 170, gameOverSurface->w+50, gameOverSurface->h+50};
    SDL_RenderCopy(gRenderer, gameOverTexture, nullptr, &gameOverRect);

    SDL_Surface* scoreSurface = TTF_RenderText_Solid(gFont, scoreText.c_str(), textColor);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(gRenderer, scoreSurface);

    SDL_Rect scoreRect = {SCREEN_WIDTH / 2 - scoreSurface->w / 2, SCREEN_HEIGHT / 2 - 80, scoreSurface->w+20, scoreSurface->h+20};
    SDL_RenderCopy(gRenderer, scoreTexture, nullptr, &scoreRect);

    SDL_Surface* OverSurface2 = TTF_RenderText_Solid(gFont, OverText2.c_str(), OptiontextColor);
    SDL_Texture* OverTexture2 = SDL_CreateTextureFromSurface(gRenderer, OverSurface2);

    SDL_Rect OverRect2 = {SCREEN_WIDTH / 2 - gameOverSurface->w / 2-10, SCREEN_HEIGHT / 2, gameOverSurface->w+20, gameOverSurface->h+30};
    SDL_RenderCopy(gRenderer, OverTexture2, nullptr, &OverRect2);

    SDL_FreeSurface(gameOverSurface);
    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(gameOverTexture);
    SDL_DestroyTexture(scoreTexture);

    exitButton.isClicked = false;
    drawButton(exitButton, "No");

    restartButton.isClicked = false;
    drawButton(restartButton, "Yes");

    SDL_RenderPresent(gRenderer);
}

int main(int argc, char* args[]) {
    if (!init()) {
        std::cerr << "Failed to initialize!" << std::endl;
        return -1;
    }

     if (!initObstacles()) {
        std::cerr << "Failed to initialize obstacles!" << std::endl;
        return -1;
    }

    SDL_Event e;

    spawnFood();

    while (currentState != QUIT) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                currentState = QUIT;
            } else if (currentState == START || currentState == GAME || currentState == GAME_OVER) {
                handleInput(e);
            }
        }

        if (currentState == START) {
            renderStartScreen();
        } else if (currentState == GAME && !gameover) {
            moveSnake();
            checkFoodCollision();
           

            if ( checkCollisionWithItself()|| checkCollisionWithobstragol()) {
                gameover = true;
                currentState = GAME_OVER;
            }

            renderGameScreen();
        } else if (currentState == GAME_OVER) {
            renderGameOverScreen();

            if (exitButton.isClicked) {
                currentState = QUIT;
            }
        }

        SDL_Delay(120);
    }

    close();

    return 0;
}
