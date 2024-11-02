#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <SDL2/SDL_ttf.h>

// Define screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Define ship dimensions
const int SHIP_WIDTH = 32;
const int SHIP_HEIGHT = 32;

// Define bullet dimensions
const int BULLET_WIDTH = 9;
const int BULLET_HEIGHT = 21;
const int MAX_BULLETS = 115;

// Structure for a bullet
typedef struct {
    int x, y;  // Bullet position
    bool active;  // Whether the bullet is active or not
} Bullet;

// Structure for the game state
typedef struct {
    int shipX, shipY;  // Ship position
    SDL_Texture *shipTexture;  // Texture for the ship
    Bullet bullets[MAX_BULLETS];  // Array of bullets
    SDL_Texture *bulletTexture;  // Texture for the bullet
    int bulletSpeed;  // Speed of the bullets
    int fireRate;  // Rate at which bullets can be fired
    int fireCooldown;  // Cooldown for firing bullets
} GameState;

// Function to initialize the game state
void initGameState(GameState *state, SDL_Renderer *renderer) {
    state->shipX = SCREEN_WIDTH / 2 - SHIP_WIDTH / 2;
    state->shipY = SCREEN_HEIGHT - SHIP_HEIGHT - 10;
    state->bulletSpeed = 5;
    state->fireRate = 50;  // Time between shots in milliseconds
    state->fireCooldown = 9;

    // Load ship texture
    SDL_Surface *shipSurface = IMG_Load("ship.bmp");
    if (!shipSurface) {
        fprintf(stderr, "Failed to load ship image: %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }
    state->shipTexture = SDL_CreateTextureFromSurface(renderer, shipSurface);
    SDL_FreeSurface(shipSurface);

    // Load bullet texture
    SDL_Surface *bulletSurface = IMG_Load("bullet.bmp");
    if (!bulletSurface) {
        fprintf(stderr, "Failed to load bullet image: %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }
    state->bulletTexture = SDL_CreateTextureFromSurface(renderer, bulletSurface);
    SDL_FreeSurface(bulletSurface);

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
   // if (state->fireCooldown > 0) {
   //     state->fireCooldown =-100;  // Assuming a frame duration of 16 milliseconds
   // }
}

// Function to render the game state
void renderGameState(SDL_Renderer *renderer, const GameState *state) {
    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render the ship
    SDL_Rect shipRect = {state->shipX, state->shipY, SHIP_WIDTH, SHIP_HEIGHT};
    SDL_RenderCopy(renderer, state->shipTexture, NULL, &shipRect);

    // Render the bullets
    for (int i = 0; i < MAX_BULLETS; ++i) {
        if (state->bullets[i].active) {
            SDL_Rect bulletRect = {state->bullets[i].x, state->bullets[i].y, BULLET_WIDTH, BULLET_HEIGHT};
            SDL_RenderCopy(renderer, state->bulletTexture, NULL, &bulletRect);
        }
    }

    // Present the renderer
    SDL_RenderPresent(renderer);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);  // Initialize SDL_image with BMP support
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("Space Shooter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    GameState gameState;
    initGameState(&gameState, renderer);

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

    SDL_DestroyTexture(gameState.shipTexture);
    SDL_DestroyTexture(gameState.bulletTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;
}

