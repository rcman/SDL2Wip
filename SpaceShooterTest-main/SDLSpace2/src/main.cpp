#include <SDL2/SDL.h>
#include <stdbool.h>
#include <SDL2/SDL_ttf.h>

// Define screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Define ship dimensions
const int SHIP_WIDTH = 32;
const int SHIP_HEIGHT = 32;

// Define bullet dimensions
const int BULLET_WIDTH = 10;
const int BULLET_HEIGHT = 10;
const int MAX_BULLETS = 25;

// Structure for a bullet
typedef struct {
    int x, y;  // Bullet position
    bool active;  // Whether the bullet is active or not
} Bullet;

// Structure for the game state
typedef struct {
    int shipX, shipY;  // Ship position
    Bullet bullets[MAX_BULLETS];  // Array of bullets
    int bulletSpeed;  // Speed of the bullets
    int fireRate;  // Rate at which bullets can be fired
    int fireCooldown;  // Cooldown for firing bullets
} GameState;

// Function to initialize the game state
void initGameState(GameState *state) {
    state->shipX = SCREEN_WIDTH / 2 - SHIP_WIDTH / 2;
    state->shipY = SCREEN_HEIGHT - SHIP_HEIGHT - 10;
    state->bulletSpeed = 5;
    state->fireRate = 50;  // Time between shots in milliseconds
    state->fireCooldown = 0;

    // Initialize bullets
    for (int i = 0; i < MAX_BULLETS; ++i) {
        state->bullets[i].active = false;
    }
}

// Function to update the game state
void updateGameState(GameState *state) {
    // Update ship position based on input
    const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
    if (keyboardState[SDL_SCANCODE_LEFT] && state->shipX > 0) {
        state->shipX -= 5;
    }
    if (keyboardState[SDL_SCANCODE_RIGHT] && state->shipX < SCREEN_WIDTH - SHIP_WIDTH) {
        state->shipX += 5;
    }

    // Update bullets
    for (int i = 0; i < MAX_BULLETS; ++i) {
        if (state->bullets[i].active) {
            state->bullets[i].y -= state->bulletSpeed;

            // Deactivate bullets that go off the top of the screen
            if (state->bullets[i].y + BULLET_HEIGHT < 0) {
                state->bullets[i].active = false;
            }
        }
    }

    // Fire bullets based on input and cooldown
    if (keyboardState[SDL_SCANCODE_SPACE] && state->fireCooldown <= 0) {
        for (int i = 0; i < MAX_BULLETS; ++i) {
            if (!state->bullets[i].active) {
                state->bullets[i].x = state->shipX + SHIP_WIDTH / 2 - BULLET_WIDTH / 2;
                state->bullets[i].y = state->shipY;
                state->bullets[i].active = true;
                break;  // Only fire one bullet per frame
            }
        }

        state->fireCooldown = state->fireRate;
    }

    // Update fire cooldown
    if (state->fireCooldown > 0) {
        state->fireCooldown -= 16;  // Assuming a frame duration of 16 milliseconds
    }
}

// Function to render the game state and additional information
void renderGameState(SDL_Renderer *renderer, const GameState *state) {
    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render the ship
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect shipRect = {state->shipX, state->shipY, SHIP_WIDTH, SHIP_HEIGHT};
    SDL_RenderFillRect(renderer, &shipRect);

    // Render the bullets
    for (int i = 0; i < MAX_BULLETS; ++i) {
        if (state->bullets[i].active) {
            SDL_Rect bulletRect = {state->bullets[i].x, state->bullets[i].y, BULLET_WIDTH, BULLET_HEIGHT};
            SDL_RenderFillRect(renderer, &bulletRect);
        }
    }

    // Render text
    SDL_Color textColor = {255, 255, 255, 255};
    TTF_Font *font = TTF_OpenFont("Roboto-Black.ttf", 24); // Replace with the path to your font file

    if (font != NULL) {
        SDL_Surface *textSurface = TTF_RenderText_Solid(font, "Press Q to Quit", textColor);
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        SDL_Rect textRect = {SCREEN_WIDTH / 2 - textSurface->w / 2, 10, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
        TTF_CloseFont(font);
    } else {
        // Handle font loading error
        fprintf(stderr, "Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
    }

    // Present the renderer
    SDL_RenderPresent(renderer);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("Space Shooter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    GameState gameState;
    initGameState(&gameState);

    SDL_Event event;
    bool quit = false;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_q) {
                    quit = true;
                }
            }
        }

        updateGameState(&gameState);
        renderGameState(renderer, &gameState);
        SDL_Delay(16);  // Cap the frame rate to approximately 60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}

