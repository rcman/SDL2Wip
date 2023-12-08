#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>

// Function to load a texture from file
SDL_Texture* LoadTexture(const char* filePath, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(filePath);
    if (!surface) {
        std::cerr << "Error loading image: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    return texture;
}

// Function to check collision between two rectangles
bool CheckCollision(const SDL_Rect& a, const SDL_Rect& b) {
    return (a.x < b.x + b.w &&
            a.x + a.w > b.x &&
            a.y < b.y + b.h &&
            a.y + a.h > b.y);
}

int main(int argc, char* argv[]) {
    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow("Vertical Shooting Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Load player and bullet textures
    SDL_Texture* playerTexture = LoadTexture("player.bmp", renderer);
    SDL_Texture* bulletTexture = LoadTexture("smaller.bmp", renderer);

    // Game variables
    int playerX = 400; // Initial player position
    const int playerY = 500;
    const int playerSpeed = 15;
    const int bulletSpeed = 10;
    const int enemySpawnRate = 5;
    const int enemySpeed = 2;
    const int enemyWidth = 32;
    const int enemyHeight = 32;

    struct Bullet {
        int x, y;
    };

    struct Enemy {
        int x, y;
    };

    std::vector<Bullet> bullets;
    std::vector<Enemy> enemies;


// Main game loop
SDL_Event event;
bool quit = false;

while (!quit) {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit = true;
        } else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_LEFT) {
                playerX -= playerSpeed;
            } else if (event.key.keysym.sym == SDLK_RIGHT) {
                playerX += playerSpeed;
            } else if (event.key.keysym.sym == SDLK_SPACE) {
                // Shoot a bullet
                Bullet newBullet = {playerX + 12, playerY}; // Adjust position for the center of the player ship
                bullets.push_back(newBullet);
            }
        }
    }

        // Clear the screen
        SDL_RenderClear(renderer);

        // Move and render bullets
        for (auto& bullet : bullets) {
            bullet.y -= bulletSpeed; // Move bullets upwards
            SDL_Rect bulletRect = {bullet.x, bullet.y, 8, 8};
            SDL_RenderCopy(renderer, bulletTexture, nullptr, &bulletRect);
        }

        // Create new enemies periodically
        if (rand() % 100 < enemySpawnRate) {
            Enemy newEnemy = {rand() % 800, 0}; // Random X position at the top
            enemies.push_back(newEnemy);
        }

        // Move and render enemies
        for (auto& enemy : enemies) {
            enemy.y += enemySpeed; // Move enemies downwards
            SDL_Rect enemyRect = {enemy.x, enemy.y, enemyWidth, enemyHeight};
            SDL_RenderCopy(renderer, playerTexture, nullptr, &enemyRect);
        }

	// Check for collisions
for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
    for (auto enemyIt = enemies.begin(); enemyIt != enemies.end();) {
        if (CheckCollision({bulletIt->x, bulletIt->y, 8, 8}, {enemyIt->x, enemyIt->y, enemyWidth, enemyHeight})) {
            // Handle collision (e.g., remove bullet and enemy)
            bulletIt = bullets.erase(bulletIt);
            enemyIt = enemies.erase(enemyIt);
        } else {
            ++enemyIt;
        }
    }

    if (bulletIt != bullets.end()) {
        ++bulletIt;
    }
}




        // Render the player
        SDL_Rect playerRect = {playerX, playerY, 32, 32};
        SDL_RenderCopy(renderer, playerTexture, nullptr, &playerRect);

        // Present the renderer
        SDL_RenderPresent(renderer);
    }

    // Clean up
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(bulletTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

