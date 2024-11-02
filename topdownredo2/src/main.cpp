#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>

using namespace std;

const int num=8; //checkpoints
int points[num][2] = {300, 610,
                      1270,430,
                      1380,2380,
                      1900,2460,
                      1970,1700,
                      2550,1680,
                      2560,3150,
                      500, 3300};

struct Car
{
    float x,y,speed,angle; int n;

    Car() {speed=2; angle=0; n=0;}

void move()
{
    float new_x = x + sin(angle) * speed;
    float new_y = y - cos(angle) * speed;

    // Make sure new coordinates are within the bounds of the background
    if (new_x > 0 && new_x < 1280 && new_y > 0 && new_y < 960) {
        x = new_x;
        y = new_y;
    }
}
    void findTarget()
    {
        float tx=points[n][0];
        float ty=points[n][1];
        float beta = angle-atan2(tx-x,-ty+y);
        if (sin(beta)<0) angle+=0.005*speed; else angle-=0.005*speed;
        if ((x-tx)*(x-tx)+(y-ty)*(y-ty)<25*25) n=(n+1)%num;
    }
};


int main(int argc, char* args[])
{
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Surface* surface = NULL;

    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Create window
    window = SDL_CreateWindow("Car Racing Game!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024,768, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load images
    surface = IMG_Load("background.png");
    SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("car.png");
    SDL_Texture* carTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // Set texture smoothing
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, 640, 480);

    SDL_Rect backgroundRect = {0, 0, 1280, 960};

    SDL_Rect carRect = {0, 0, 44, 44};
    SDL_Point carCenter = {22, 22};

    float R=22;

    const int N = 5;
Car car[N];
int initialPositions[N][2] = { {400, 630},
                               {480, 630},
                               {560, 630},
                               {640, 630},
                               {720, 630} };
for(int i = 0; i < N; i++)
{
    car[i].x = initialPositions[i][0];
    car[i].y = initialPositions[i][1];
    car[i].speed = 7 + i;
}

    float speed=0,angle=0;
    float maxSpeed=12.0;
    float acc=0.2, dec=0.3;
    float turnSpeed=0.08;

    int offsetX=0,offsetY=0;

    SDL_Event event;
    bool isRunning = true;

    while (isRunning)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                isRunning = false;
            }
        }

        bool Up=0,Right=0,Down=0,Left=0;
        const Uint8* state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_UP]) Up=1;
    if (state[SDL_SCANCODE_RIGHT]) Right=1;
    if (state[SDL_SCANCODE_DOWN]) Down=1;
    if (state[SDL_SCANCODE_LEFT]) Left=1;

    //car movement
    if (Up && speed<maxSpeed)
        if (speed < 0)  speed += dec;
        else  speed += acc;

    if (Down && speed>-maxSpeed)
        if (speed > 0) speed -= dec;
        else  speed -= acc;

    if (!Up && !Down)
        if (speed - dec > 0) speed -= dec;
        else if (speed + dec < 0) speed += dec;
        else speed = 0;

    if (Right && speed!=0)  angle += turnSpeed * speed/maxSpeed;
    if (Left && speed!=0)   angle -= turnSpeed * speed/maxSpeed;

    car[0].speed = speed;
    car[0].angle = angle;

    for(int i=0;i<N;i++) car[i].move();
    for(int i=1;i<N;i++) car[i].findTarget();

    //collision
    for(int i=0;i<N;i++)
    for(int j=0;j<N;j++)
    {
        int dx=0, dy=0;
        while (dx*dx+dy*dy<4*R*R)
        {
            car[i].x+=dx/10.0;
            car[i].x+=dy/10.0;
            car[j].x-=dx/10.0;
            car[j].y-=dy/10.0;
            dx = car[i].x-car[j].x;
            dy = car[i].y-car[j].y;
            if (!dx && !dy) break;
        }
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    if (car[0].x>320) offsetX = car[0].x-320;
    if (car[0].y>240) offsetY = car[0].y-240;

    backgroundRect.x = -offsetX;
    backgroundRect.y = -offsetY;
    SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundRect);

    SDL_Color colors[10] = { {255, 0, 0, 255}, {0, 255, 0, 255}, {255, 0, 255, 255}, {0, 0, 255, 255}, {255, 255, 255, 255} };

    for(int i=0;i<N;i++)
    {
        carRect.x = car[i].x - offsetX - 22;
        carRect.y = car[i].y - offsetY - 22;
        SDL_RenderCopyEx(renderer, carTexture, NULL, &carRect, car[i].angle*180/M_PI, &carCenter, SDL_FLIP_NONE);
        SDL_SetTextureColorMod(carTexture, colors[i].r, colors[i].g, colors[i].b);
    }
	SDL_Delay(3);
    SDL_RenderPresent(renderer);

}

SDL_DestroyRenderer(renderer);
SDL_DestroyWindow(window);

SDL_Quit();
return 0;
}
