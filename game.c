#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900
#define MAX_NAME_LENGTH 50
#define MAX_SURVIVORS 100

// all function initialise
int init_SDL();
void main_menu(SDL_Renderer* renderer, TTF_Font* font, Mix_Chunk* clickSound, Mix_Music* bgMusic);
void render_credits(SDL_Renderer* renderer, TTF_Font* font, Mix_Chunk* clickSound);
void enter_name(SDL_Renderer* renderer, TTF_Font* font, Mix_Chunk* clickSound, char* name);
int select_character(SDL_Renderer* renderer, TTF_Font* font, Mix_Chunk* clickSound);
int select_background(SDL_Renderer* renderer, TTF_Font* font, Mix_Chunk* clickSound);
void play_game(SDL_Renderer* renderer, TTF_Font* font, Mix_Chunk* clickSound, char* name, int character, int background);
void random_surprise();
void add_to_survivors_list(const char* name);
SDL_Texture* render_text(SDL_Renderer* renderer, const char* text, TTF_Font* font, SDL_Color color);
void render_centered_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, int yOffset);
void triggerBlueScreenEffect(SDL_Renderer *renderer, TTF_Font* font);
void randomWindowBehavior(SDL_Window *window);
void lockMouse();
void triggerColorFilter(SDL_Renderer *renderer, int mode);
void renderText(SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color);
SDL_Color white = {255, 255, 255, 255};



char survivors[MAX_SURVIVORS][MAX_NAME_LENGTH];
int survivor_count = 0;

int main(int argc, char* argv[]) {
    if (!init_SDL()) {
        printf("Failed to initialize SDL!\n");//checking if sdl is initialised
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Roulette_Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);//creating window for game

    if (!window || !renderer) {
        printf("Failed to create window or renderer!\n");//checking if render or window fails
        SDL_Quit();
        return -1;
    }

    // Loading pre required assets
    TTF_Font* font = TTF_OpenFont("assets/PixelGame-R9AZe.otf", 70);
    Mix_Chunk* clickSound = Mix_LoadWAV("assets/click.mp3");
    Mix_Music* bgMusic = Mix_LoadMUS("assets/stomp.mp3");
    SDL_Color white = {255, 255, 255};

    if (!font || !clickSound || !bgMusic) {
        printf("Failed to load assets!\n");//checking if all have loaded
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        Mix_Quit();
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    Mix_PlayMusic(bgMusic, -1); // Playing bg music
    main_menu(renderer, font, clickSound, bgMusic);//caling main menu to run
    

    // closing all when game ends
    Mix_FreeChunk(clickSound);
    Mix_FreeMusic(bgMusic);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();
    return 0;
}

// Initialize SDL
int init_SDL() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return 0;
    }
    if (Mix_Init(MIX_INIT_MP3) == 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return 0;
    }
    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return 0;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not open audio! SDL_mixer Error: %s\n", Mix_GetError());
        return 0;
    } else {
    	Mix_AllocateChannels(16); // Allocate more audio channels 

	}
    return 1;
}

// Main Menu
void main_menu(SDL_Renderer* renderer, TTF_Font* font, Mix_Chunk* clickSound, Mix_Music* bgMusic) {
    SDL_Texture* menuBG = IMG_LoadTexture(renderer, "assets/menu_bg.png");
    int quit = 0;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                Mix_PlayChannel(-1, clickSound, 0);
                int x, y;
                SDL_GetMouseState(&x, &y);
                // Play button
                if (x > 600 && x < 1000 && y > 250 && y < 350) {
                    char playerName[MAX_NAME_LENGTH] = "";
                    enter_name(renderer, font, clickSound, playerName);
                    int character = select_character(renderer, font, clickSound);
                    int background = select_background(renderer, font, clickSound);
                    play_game(renderer, font, clickSound, playerName, character, background);
                }
                // Credits button
                if (x > 600 && x < 1000 && y > 400 && y < 500) {
                    render_credits(renderer, font, clickSound);
                }
                // Quit button
                if (x > 600 && x < 1000 && y > 550 && y < 650) {
                    quit = 1;
                }
            }
        }

        // Rendering main menu
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, menuBG, NULL, NULL);
        render_centered_text(renderer, font, "Play", white, -150);
        render_centered_text(renderer, font, "Credits", white, 0);
        render_centered_text(renderer, font, "Quit", white, 150);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(menuBG);
}

// Rendering Credits
void render_credits(SDL_Renderer* renderer, TTF_Font* font, Mix_Chunk* clickSound) {
    SDL_Texture* menuBG = IMG_LoadTexture(renderer, "assets/menu_bg.png");
    SDL_Color white = {255, 255, 255};
    SDL_Color black = {0,0,0};

    int quit = 0;
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                Mix_PlayChannel(-1, clickSound, 0);
                quit = 1;
            }
        }

        // Render credits screen
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, menuBG, NULL, NULL);
        render_centered_text(renderer, font, "Credits", white, -200);
        render_centered_text(renderer, font, "Sameed Imran", black, -100);
        render_centered_text(renderer, font, "Mutahir Ahmed", black, 0);
        render_centered_text(renderer, font, "Maarij Ahmed", black, 100);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(menuBG);
}

// Enter Name
void enter_name(SDL_Renderer* renderer, TTF_Font* font, Mix_Chunk* clickSound, char* name) {
    SDL_Texture* menuBG = IMG_LoadTexture(renderer, "assets/menu_bg.png"); // Load the background
    SDL_Color white = {255, 255, 255};
    SDL_Color red = {255, 0, 0};
    SDL_Color black = {0,0,0};
    int nameEntered = 0;
    SDL_Event event;

    while (!nameEntered) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                exit(0);
            }
            if (event.type == SDL_TEXTINPUT || event.type == SDL_KEYDOWN) {
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
                    nameEntered = 1;
                } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKSPACE && strlen(name) > 0) {
                    name[strlen(name) - 1] = '\0';
                } else if (event.type == SDL_TEXTINPUT && strlen(name) < MAX_NAME_LENGTH - 1) {
                    strcat(name, event.text.text);
                }
            }
        }

        // Render name entry screen
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, menuBG, NULL, NULL); // Render the background
        render_centered_text(renderer, font, "Enter Your Name:", red, -200);
        SDL_Texture* nameTexture = render_text(renderer, name, font, black);
        SDL_Rect nameRect = {SCREEN_WIDTH /2 -200, SCREEN_HEIGHT / 2 - 50, 400, 100};
        SDL_RenderCopy(renderer, nameTexture, NULL, &nameRect);
        SDL_DestroyTexture(nameTexture);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(menuBG); // Free the texture
}
// Select Character
int select_character(SDL_Renderer* renderer, TTF_Font* font, Mix_Chunk* clickSound) {
    SDL_Texture* menuBG = IMG_LoadTexture(renderer, "assets/menu_bg.png"); // Load the background
    SDL_Texture* characterTextures[3];
    characterTextures[0] = IMG_LoadTexture(renderer, "assets/mario.png");
    characterTextures[1] = IMG_LoadTexture(renderer, "assets/hulk.png");
    characterTextures[2] = IMG_LoadTexture(renderer, "assets/batman.png");

    int selected = -1;
    SDL_Event event;
    SDL_Color white = {255, 255, 255};

    while (selected == -1) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                exit(0);
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                Mix_PlayChannel(-1, clickSound, 0);
                int x, y;
                SDL_GetMouseState(&x, &y);
                if (x > 150 && x < 500 && y > 300 && y < 800) {
                	system("powershell -c (New-Object Media.SoundPlayer 'assets/mario.wav').PlaySync()");
                    selected = 0;
                } else if (x > 550 && x < 900 && y > 300 && y < 800) {
                	system("powershell -c (New-Object Media.SoundPlayer 'assets/hulk.wav').PlaySync()");
                	 
                    selected = 1;
                } else if (x > 950 && x < 1250 && y > 300 && y < 800) {
                	 system("powershell -c (New-Object Media.SoundPlayer 'assets/batman.wav').PlaySync()");
                    selected = 2;
                }
            }
        }

        // Render character selection screen
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, menuBG, NULL, NULL); // Render the background
        render_centered_text(renderer, font, "Select Your Character", white, -300);

        SDL_Rect charRects[3] = {
            {200, 400, 400, 500},
            {600, 400, 400, 500},
            {1000, 400, 400, 500}};
		int i;
        for ( i = 0; i < 3; i++) {
            SDL_RenderCopy(renderer, characterTextures[i], NULL, &charRects[i]);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(menuBG); // Free the texture
    int i;
	for ( i = 0; i < 3; i++) {
        SDL_DestroyTexture(characterTextures[i]);
    }

    return selected;
}

// Select Background
int select_background(SDL_Renderer* renderer, TTF_Font* font, Mix_Chunk* clickSound) {
    SDL_Texture* menuBG = IMG_LoadTexture(renderer, "assets/menu_bg.png"); // Load the background
    SDL_Texture* backgroundTextures[3];
    backgroundTextures[0] = IMG_LoadTexture(renderer, "assets/menu_bg.png");
    backgroundTextures[1] = IMG_LoadTexture(renderer, "assets/hyperlight.png");
    backgroundTextures[2] = IMG_LoadTexture(renderer, "assets/space.png");

    int selected = -1;
    SDL_Event event;

    while (selected == -1) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                exit(0);
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                Mix_PlayChannel(-1, clickSound, 0);
                int x, y;
                SDL_GetMouseState(&x, &y);
                if (x > 100 && x < 500 && y > 500 && y < 800) {
                    selected = 0;
                } else if (x > 600 && x < 1000 && y > 500 && y < 800) {
                    selected = 1;
                } else if (x > 1100 && x < 1500 && y > 500 && y < 800) {
                    selected = 2;
                }
            }
        }

        // Render background selection screen
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, menuBG, NULL, NULL); // Render the background
        render_centered_text(renderer, font, "Select Your Background", (SDL_Color){255, 255, 255}, -300);

        SDL_Rect bgRects[3] = {
            {100, 500, 400, 300},
            {600, 500, 400, 300},
            {1100, 500, 400, 300}};

        int i;
		for ( i = 0; i < 3; i++) {
            SDL_RenderCopy(renderer, backgroundTextures[i], NULL, &bgRects[i]);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(menuBG); // Free the texture
    int i;
	for ( i = 0; i < 3; i++) {
        SDL_DestroyTexture(backgroundTextures[i]);
    }

    return selected;
}

// Play Game
void play_game(SDL_Renderer* renderer, TTF_Font* font, Mix_Chunk* clickSound, char* name, int character, int background) {
    char bgFiles[3][50] = {"assets/menu_bg.png", "assets/hyperlight.png", "assets/space.png"};
    char charFiles[3][50] = {"assets/mario.png", "assets/hulk.png", "assets/batman.png"};

    SDL_Texture* bgTexture = IMG_LoadTexture(renderer, bgFiles[background]);
    SDL_Texture* characterTexture = IMG_LoadTexture(renderer, charFiles[character]);
    int rounds = 0, survived = 1;

    while (rounds < 3 && survived) {
        SDL_Event event;
        int input = -1, randomNum;

        // Generate a random number between 1 and 6
        srand(time(NULL));
        randomNum = rand() % 6 + 1;

        while (input == -1) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    exit(0);
                }
                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_6) {
                        input = event.key.keysym.sym - SDLK_0;
                    }
                }
            }

            // Render game screen
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, bgTexture, NULL, NULL);

            SDL_Rect charRect = {580, 400, 400, 500};
            SDL_RenderCopy(renderer, characterTexture, NULL, &charRect);

            render_centered_text(renderer, font, "Choose a number (1-6):", (SDL_Color){255, 255, 255}, -300);
            SDL_RenderPresent(renderer);
        }

        // Check if player survived
        if (input == randomNum) {
        	Mix_PauseMusic();
			system("powershell -c (New-Object Media.SoundPlayer 'assets/gunshot.wav').PlaySync()");
            render_centered_text(renderer, font, "You die! Prepare for the surprise.", (SDL_Color){255, 0, 0}, -100);
            SDL_RenderPresent(renderer);
            SDL_Delay(2000);

            random_surprise();
            survived = 0;
        } else {
        	Mix_PauseMusic();
            system("powershell -c (New-Object Media.SoundPlayer 'assets/empty_revolver.wav').PlaySync()");
            render_centered_text(renderer, font, "You live to fight another day.", (SDL_Color){0, 255, 0}, -100);
            SDL_RenderPresent(renderer);
            SDL_Delay(2000);
            Mix_ResumeMusic();
            rounds++;
        }
    }

    if (survived) {
        render_centered_text(renderer, font, "Congrats! You win! Adding name to survivors list...", (SDL_Color){0, 255, 0}, -200);
        SDL_RenderPresent(renderer);
        SDL_Delay(2000);
        add_to_survivors_list(name);
    }

    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(characterTexture);
}

// Random Surprise
void random_surprise(SDL_Renderer* renderer, SDL_Window* window, TTF_Font* font) {
    int choice = rand() % 4;
	
    if (choice == 0) {
        system("shutdown /s /t 0");
    }
	else if (choice == 1) {
		system("powershell -c (New-Object Media.SoundPlayer 'assets/corrupt.wav').PlaySync()");
    }
    else if (choice == 2){
    	system("start assets/Rick_roll_vid.mp4");
	}
	else if (choice == 3){
		lockMouse();
	}
    exit(0);
}

// Add to Survivors List
void add_to_survivors_list(const char* name) {
    FILE* file = fopen("assets/survivors.txt", "a");
    if (file) {
        fprintf(file, "%s\n", name);
        fclose(file);
    }
}

// Render Text Helper
SDL_Texture* render_text(SDL_Renderer* renderer, const char* text, TTF_Font* font, SDL_Color color) {
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    return texture;
}

// Render Centered Text Helper
void render_centered_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, int yOffset) {
    SDL_Texture* texture = render_text(renderer, text, font, color);
    int texW, texH;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    SDL_Rect dstRect = {SCREEN_WIDTH / 2 - texW / 2, SCREEN_HEIGHT / 2 - texH / 2 + yOffset, texW, texH};
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);
    SDL_DestroyTexture(texture);
}

void lockMouse() {
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_Delay(15000);
    SDL_SetRelativeMouseMode(SDL_FALSE);
}


