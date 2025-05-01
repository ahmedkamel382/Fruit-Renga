#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

int screenWidth = 1080;
int screenHeight = 768;

const char* HIGHSCORE_FILE = "highscore.txt";  // File to store the highest score

Mix_Chunk* ButtonClick = nullptr;
Mix_Chunk* PointSound = nullptr;
Mix_Chunk* BombSound = nullptr;
Mix_Chunk* GoldenSound = nullptr;
Mix_Chunk* HealthSound = nullptr;


struct FallingImage {
    SDL_Texture* texture;
    SDL_Rect rect;
    bool isThrown;
    int textureIndex;

    FallingImage(SDL_Texture* tex, const SDL_Rect& r, int index) : texture(tex), rect(r), isThrown(false), textureIndex(index) {}


};

SDL_Texture* loadImage(const char* imagePath, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(imagePath);
    if (!surface) {
        cout << "Error loading image: " << imagePath << " - " << IMG_GetError() << endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        cout << "Error creating Texture from Surface: " << SDL_GetError() << endl;
    }

    return texture;
}

void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Rect rect, SDL_Color textColor) {
    SDL_Surface* fontSurface = TTF_RenderText_Blended(font, text, textColor);
    SDL_Texture* fontTexture = SDL_CreateTextureFromSurface(renderer, fontSurface);

    SDL_RenderCopy(renderer, fontTexture, nullptr, &rect);

    SDL_FreeSurface(fontSurface);
    SDL_DestroyTexture(fontTexture);
}

SDL_Rect gameOverRect = { 240, 150, 600, 150 };
SDL_Rect tryAgainButtonRect = { screenWidth / 2 - 150, screenHeight / 2 + 50, 300, 100 };


bool isMouseOver(const SDL_Rect& rect, int mouseX, int mouseY) {
    return mouseX >= rect.x && mouseX < rect.x + rect.w &&
        mouseY >= rect.y && mouseY < rect.y + rect.h;
}

void saveHighestScore(int highestScore) {
    FILE* file;
    if (fopen_s(&file, HIGHSCORE_FILE, "a") == 0) {
        fprintf(file, "%d\n", highestScore);
        fclose(file);
    }
}

int loadHighestScore() {
    FILE* file;
    int highScore = 0;
    if (fopen_s(&file, HIGHSCORE_FILE, "r") == 0) {
        fscanf_s(file, "%d", &highScore);
        fclose(file);
    }
    return highScore;
}

void showMainMenu(SDL_Renderer* renderer, TTF_Font* font, SDL_Texture* backgroundTexture, bool& inMainMenu, bool& gameStarted, bool& quit, Mix_Music* MenuMusic) {

    SDL_Rect titleRect = { 200, -50, 700, 700 };
    SDL_Rect playButtonRect = { screenWidth / 2 - 100, screenHeight / 2 + 100, 200, 100 };

    SDL_Texture* titleTexture = loadImage("Pictures/Fruit_ringa.png", renderer);
    if (!titleTexture) {
        cout << "Error loading title image!" << endl;
        return;
    }

    SDL_Event event;

    // Play music only in the main menu
    Mix_PlayMusic(MenuMusic, -1);
    while (inMainMenu) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                inMainMenu = false;
                gameStarted = false;
                quit = true;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);

                    if (isMouseOver(playButtonRect, mouseX, mouseY)) {
                        cout << "Play button clicked" << endl;
                        Mix_PlayChannel(-1, ButtonClick, 0);  // Play click sound
                        Mix_HaltMusic();
                        inMainMenu = false;
                        gameStarted = true;
                    }
                }
                break;
            }
        }
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
        SDL_RenderCopy(renderer, titleTexture, nullptr, &titleRect); // Render the title image
        renderText(renderer, font, "Play", playButtonRect, { 255, 255, 255 });
        SDL_RenderPresent(renderer);
    }
}

void RestartGame(SDL_Renderer* renderer, vector<SDL_Texture*>& textures, vector<FallingImage*>& fallingImages,
    int& score, int& health, bool& gameStarted, bool& gameOver, bool& inMainMenu, Mix_Music* GameplayMusic) {
    Mix_PlayMusic(GameplayMusic, -1);
    inMainMenu = false;
    gameStarted = true;
    gameOver = false;
    health = 15;
    score = 0;
    fallingImages.clear();
}

void renderHealthAndScore(SDL_Renderer* renderer, TTF_Font* font, int health, int score) {
    SDL_Rect healthRect = { 10, 10, 150, 50 };
    SDL_Rect scoreRect = { 900, 10, 150, 50 };

    char healthText[16];
    char scoreText[16];

    snprintf(healthText, sizeof(healthText), "Health: %d", health);
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);

    renderText(renderer, font, healthText, healthRect, { 255, 255, 255 });
    renderText(renderer, font, scoreText, scoreRect, { 255, 255, 255 });
}

void renderHighestScore(SDL_Renderer* renderer, TTF_Font* font, int highestScore) {
    SDL_Rect highestScoreRect = { screenWidth / 2 - 150, screenHeight - 50, 300, 40 };
    char highestScoreText[32];
    snprintf(highestScoreText, sizeof(highestScoreText), "Highest Score: %d", highestScore);
    renderText(renderer, font, highestScoreText, highestScoreRect, { 255, 255, 255 });
}

void showGameOverScreen(SDL_Renderer* renderer, TTF_Font* font, int highestScore, SDL_Rect gameOverRect, SDL_Rect tryAgainButtonRect, Mix_Music* GameOverMusic) {
    renderText(renderer, font, "Game Over!", gameOverRect, { 255, 255, 255 });
    renderHighestScore(renderer, font, highestScore);
    renderText(renderer, font, "Try Again", tryAgainButtonRect, { 255, 255, 255 });
}

void cleanupAudio(Mix_Music* MenuMusic, Mix_Music* GameplayMusic, Mix_Music* GameOverMusic, Mix_Chunk* ButtonClick, Mix_Chunk* PointSound, Mix_Chunk* BombSound, Mix_Chunk* GoldenSound, Mix_Chunk* HealthSound) {
    Mix_FreeMusic(MenuMusic);
    Mix_FreeMusic(GameplayMusic);
    Mix_FreeMusic(GameOverMusic);
    Mix_FreeChunk(ButtonClick);
    Mix_FreeChunk(PointSound);
    Mix_FreeChunk(BombSound);
    Mix_FreeChunk(GoldenSound);
    Mix_FreeChunk(HealthSound);
    Mix_CloseAudio();
}



int main(int argc, char* argv[]) {
    // Initialization
    int highestScore = 0;

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << endl;
        return -1;
    }


    Mix_Music* MenuMusic = Mix_LoadMUS("Sounds/Mainmenu.mp3");
    if (!MenuMusic) {
        cout << "Failed to load MenuMusic! SDL_mixer Error: " << Mix_GetError() << endl;
        return -1;
    }
    Mix_Music* GameplayMusic = Mix_LoadMUS("Sounds/Gameplay.mp3");
    if (!GameplayMusic) {
        cout << "Failed to load GameplayMusic! SDL_mixer Error: " << Mix_GetError() << endl;
        return -1;
    }
    Mix_Music* GameOverMusic = Mix_LoadMUS("Sounds/gameover.mp3");
    if (!GameOverMusic) {
        cout << "Failed to load GameOverMusic! SDL_mixer Error: " << Mix_GetError() << endl;
        return -1;
    }
    ButtonClick = Mix_LoadWAV("Sounds/click.wav");
    if (!ButtonClick) {
        cout << "Failed to load ButtonClick! SDL_mixer Error: " << Mix_GetError() << endl;
        return -1;
    }
    PointSound = Mix_LoadWAV("Sounds/point.wav");
    if (!PointSound) {
        cout << "Failed to load PointSound! SDL_mixer Error: " << Mix_GetError() << endl;
        return -1;
    }
    BombSound = Mix_LoadWAV("Sounds/Bomb.wav");
    if (!BombSound) {
        cout << "Failed to load BombSound! SDL_mixer Error: " << Mix_GetError() << endl;
        return -1;
    }
    GoldenSound = Mix_LoadWAV("Sounds/golden.wav");
    if (!GoldenSound) {
        cout << "Failed to load GoldenSound! SDL_mixer Error: " << Mix_GetError() << endl;
        return -1;
    }
    HealthSound = Mix_LoadWAV("Sounds/health.wav");
    if (!HealthSound) {
        cout << "Failed to load HealthSound! SDL_mixer Error: " << Mix_GetError() << endl;
        return -1;
    }


    SDL_Window* window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Load background image
    SDL_Texture* backgroundTexture = loadImage("Pictures/bg.png", renderer);
    if (!backgroundTexture) {
        cout << "Error loading background image!" << endl;
        cleanupAudio(MenuMusic, GameplayMusic, GameOverMusic, ButtonClick, PointSound, BombSound, GoldenSound, HealthSound);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        IMG_Quit();
        return -1;
    }

    vector<FallingImage*> fallingImages;

    int score = 0;
    int health = 15;

    SDL_Event event;
    bool quit = false;
    bool inMainMenu = true;
    bool gameStarted = false;
    bool gameOver = false;
    bool isPaused = false;

    const char* pausedText = "PAUSED";

    const char* imagePaths[] = {
        "Pictures/apple.png", // 0
        "Pictures/banana.png", // 1
        "Pictures/orange.png", // 2
        "Pictures/bomb.png", // 3
        "Pictures/Golden.png", // 4
        "Pictures/health.png" // 5
    };


    vector<SDL_Texture*> textures;
    for (int i = 0; i < sizeof(imagePaths) / sizeof(imagePaths[0]); ++i) {
        const char* imagePath = imagePaths[i];
        SDL_Texture* texture = loadImage(imagePath, renderer);
        if (!texture) {
            for (SDL_Texture* tex : textures) {
                SDL_DestroyTexture(tex);
            }
            textures.clear();
            gameStarted = false;
            SDL_Quit();
            IMG_Quit();
            cleanupAudio(MenuMusic, GameplayMusic, GameOverMusic, ButtonClick, PointSound, BombSound, GoldenSound, HealthSound);
            return -1;
        }
        textures.push_back(texture);
    }

    TTF_Init();
    TTF_Font* font = TTF_OpenFont("Fonts/Game Shark.otf", 36);
    if (!font) {
        cout << "Error loading font: " << TTF_GetError() << endl;
        return -1;
    }

    SDL_RenderPresent(renderer);
    showMainMenu(renderer, font, backgroundTexture, inMainMenu, gameStarted, quit, MenuMusic);

    // Play GameplayMusic when the game starts
    Mix_PlayMusic(GameplayMusic, -1);
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_p) { // P button is clicked for pause
                    isPaused = !isPaused;
                    cout << (isPaused ? "Game Paused" : "Game Resumed") << endl;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);

                    if (!inMainMenu && gameStarted && !gameOver) {
                        for (auto& fallingImage : fallingImages) {
                            if (fallingImage->isThrown && isMouseOver(fallingImage->rect, mouseX, mouseY)) {
                                fallingImage->rect = { rand() % (screenWidth - 100), -70, 100, 70 }; // generates new image in any x-position before being dropped
                 
                                if (fallingImage->textureIndex == 3) {
                                    Mix_PlayChannel(-1, BombSound, 0);  // Bomb sound
                                    health--;
                                }
                                else if (fallingImage->textureIndex == 4) {
                                    Mix_PlayChannel(-1, GoldenSound, 0);  // Golden Fruit sound
                                    score += 5;
                                }
                                else if (fallingImage->textureIndex == 5) {
                                    Mix_PlayChannel(-1, HealthSound, 0);  // Health sound
                                    health += 3;
                                }
                                else {
                                    Mix_PlayChannel(-1, PointSound, 0);  // Point sound
                                    score++;
                                }

                            }
                        }
                    }
                    else if (gameOver) {
                        if (isMouseOver(tryAgainButtonRect, mouseX, mouseY)) {
                            cout << "TryAgain button clicked" << endl;
                            Mix_PlayChannel(-1, ButtonClick, 0);  // Play click sound
                            RestartGame(renderer, textures, fallingImages, score, health, gameStarted, gameOver, inMainMenu, GameplayMusic);
                        }
                    }
                }
                break;
            }
        }
        //-70: y-coordinate on top of screen
        if (!inMainMenu && gameStarted && !gameOver && !isPaused) {
            for (auto& fallingImage : fallingImages) {
                if (fallingImage->isThrown) {
                    fallingImage->rect.y += 3; //speed of falling image (Adjust if too fast or too slow, depending on PC)
                }
                if (fallingImage->textureIndex != 3) {
                    if (fallingImage->rect.y >= screenHeight) { //if image fell down the screen
                        health--;
                    }
                }
                if (fallingImage->rect.y >= screenHeight) {
                    fallingImage->isThrown = false;
                    fallingImage->rect.y = -70; //image goes back to top of screen
                }
            }

            if (health <= 0) {
                gameOver = true;
                highestScore = max(highestScore, score);
                saveHighestScore(highestScore);
                Mix_PlayMusic(GameOverMusic, -1);

            }

            if (rand() % 100 < 2) { //rate of image falling
                int randomIndex;
                // Probability for index 4 and 5
                if (rand() % 15 == 0) {
                    randomIndex = (rand() % 2 == 0) ? 4 : 5;
                }
                // Probability for other indexes
                else {
                    randomIndex = rand() % (textures.size() - 2); // All indexes are randomized equally except last two (goldenfruit & health)
                }
                FallingImage* newImage = new FallingImage(textures[randomIndex], { rand() % (screenWidth - 100), -70, 100, 70 }, randomIndex);
                newImage->isThrown = true;
                fallingImages.push_back(newImage);
            }
        }
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);

        if (!inMainMenu) {
            if (!isPaused) {
                for (const auto& fallingImage : fallingImages) {
                    if (fallingImage->isThrown) {
                        SDL_RenderCopy(renderer, fallingImage->texture, nullptr, &fallingImage->rect);
                    }
                }
                renderHealthAndScore(renderer, font, health, score);

                if (gameOver) {
                    showGameOverScreen(renderer, font, highestScore, gameOverRect, tryAgainButtonRect, GameOverMusic);
                }
            }
            else {
                renderText(renderer, font, pausedText, gameOverRect, { 255, 255, 255 });
            
            }
        }

        SDL_RenderPresent(renderer);
    }

    cleanupAudio(MenuMusic, GameplayMusic, GameOverMusic, ButtonClick, PointSound, BombSound, GoldenSound, HealthSound);
    textures.clear();
    SDL_Quit();
    IMG_Quit();
    TTF_Quit();

    return 0;
}