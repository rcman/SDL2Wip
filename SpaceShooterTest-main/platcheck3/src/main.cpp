#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>

// Constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PLATFORM_WIDTH = 40;
const int PLATFORM_HEIGHT = 20;
const int CHARACTER_WIDTH = 32;
const int CHARACTER_HEIGHT = 32;
const int JUMP_VELOCITY = -15; // Adjust as needed

// Player structure
struct Player {
    SDL_Rect rect;
    int velocityX;
    int velocityY;
    bool isJumping;

    Player(int x, int y) : velocityX(0), velocityY(0), isJumping(false) {
        rect.x = x;
        rect.y = y;
        rect.w = CHARACTER_WIDTH;
        rect.h = CHARACTER_HEIGHT;
    }
};

// Platform structure
struct Platform {
    SDL_Rect rect;
    int velocityY;

    Platform(int x, int y) : velocityY(1) {
        rect.x = x;
        rect.y = y;
        rect.w = PLATFORM_WIDTH;
        rect.h = PLATFORM_HEIGHT;
    }
};

int main(int argc, char* args[]) {
    // SDL initialization
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Moving Platforms", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Event e;

    // Load textures
    SDL_Surface* playerSurface = IMG_Load("player.png");
    SDL_Texture* playerTexture = SDL_CreateTextureFromSurface(renderer, playerSurface);
    SDL_FreeSurface(playerSurface);

    SDL_Surface* platformSurface = IMG_Load("platform.png");
    SDL_Texture* platformTexture = SDL_CreateTextureFromSurface(renderer, platformSurface);
    SDL_FreeSurface(platformSurface);

    // Initialize player
    Player player(SCREEN_WIDTH / 2 - CHARACTER_WIDTH / 2, SCREEN_HEIGHT - CHARACTER_HEIGHT - 50);

    // Initialize platforms
// Initialize platforms at different levels
std::vector<Platform> platforms;
for (int i = 0; i < 9; ++i) {
    int platformY = SCREEN_HEIGHT - PLATFORM_HEIGHT - i * 100;  // Adjust the multiplier as needed
    platforms.push_back(Platform(i * 150, platformY));
}

    bool quit = false;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            // Handle other events if needed
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                        player.velocityX = -5;
                        break;
                    case SDLK_RIGHT:
                        player.velocityX = 5;
                        break;
                    case SDLK_SPACE:
                        if (!player.isJumping) {
                            player.velocityY = JUMP_VELOCITY;
                            player.isJumping = true;
                        }
                        break;
                }
            }
            else if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                    case SDLK_RIGHT:
                        player.velocityX = 0;
                        break;
                }
            }
        }

        // Update player position based on platform movement
        player.rect.x += player.velocityX;
        player.rect.y += player.velocityY;

        // Simulate gravity
        if (player.rect.y < SCREEN_HEIGHT - CHARACTER_HEIGHT) {
            player.velocityY += 1; // Adjust as needed
        } else {
            player.velocityY = 0;
            player.isJumping = false;
            player.rect.y = SCREEN_HEIGHT - CHARACTER_HEIGHT;
        }

        // Check for collision with platforms
        for (Platform& platform : platforms) {
            if (player.rect.y + CHARACTER_HEIGHT >= platform.rect.y && player.rect.y <= platform.rect.y + PLATFORM_HEIGHT &&
                player.rect.x + CHARACTER_WIDTH >= platform.rect.x && player.rect.x <= platform.rect.x + PLATFORM_WIDTH) {
                player.velocityY = 0;
                player.isJumping = false;
                player.rect.y = platform.rect.y - CHARACTER_HEIGHT;
            }
        }

        // Update platform positions and reset when they go below the screen
        for (Platform& platform : platforms) {
            platform.rect.y += platform.velocityY;

            if (platform.rect.y > SCREEN_HEIGHT) {
                platform.rect.y = -PLATFORM_HEIGHT;
            }
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Set background color to black
        SDL_RenderClear(renderer);

        // Render platforms
        for (const Platform& platform : platforms) {
            SDL_RenderCopy(renderer, platformTexture, NULL, &platform.rect);
        }

        // Render player
        SDL_RenderCopy(renderer, playerTexture, NULL, &player.rect);

        SDL_RenderPresent(renderer);
	SDL_Delay(30);
    }

    // Cleanup
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(platformTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

