#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define PLAYER_WIDTH 50
#define PLAYER_HEIGHT 60
#define PLAYER_SPEED 5
#define JUMP_FORCE 15
#define GRAVITY 1

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    SDL_Window* window = SDL_CreateWindow("spel i C",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture* backgroundTex = IMG_LoadTexture(renderer, "art/background.png");
    SDL_Texture* idleTex = IMG_LoadTexture(renderer, "art/player-sprites/idle_player.png");
    SDL_Texture* walkFrame1Tex = IMG_LoadTexture(renderer, "art/player-sprites/frame1_walk_player.png");
    SDL_Texture* walkFrame2Tex = IMG_LoadTexture(renderer, "art/player-sprites/frame2_walk_player.png");
    SDL_Texture* groundTex = IMG_LoadTexture(renderer, "art/ground.png");

    SDL_Texture* currentPlayerTex = idleTex;

    if (!backgroundTex || !idleTex || !walkFrame1Tex || !walkFrame2Tex || !groundTex) {
        SDL_Log("Kunde inte ladda texturer: %s", SDL_GetError());
        return 1;
    }

    SDL_Rect player = {100, SCREEN_HEIGHT - 110, PLAYER_WIDTH, PLAYER_HEIGHT};
    int velocityY = 0;
    bool isJumping = false;

    SDL_Rect ground = {0, SCREEN_HEIGHT - 50, SCREEN_WIDTH, 50};

    bool running = true;
    SDL_Event event;

    // Riktning
    bool facingRight = true;

    // Animation
    int animationTimer = 0;
    int animationFrame = 0;
    const int ANIMATION_SPEED = 10;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
        }

        const Uint8* keystates = SDL_GetKeyboardState(NULL);
        bool isWalking = false;

        if (keystates[SDL_SCANCODE_LEFT]) {
            player.x -= PLAYER_SPEED;
            facingRight = false;
            isWalking = true;
        }
        if (keystates[SDL_SCANCODE_RIGHT]) {
            player.x += PLAYER_SPEED;
            facingRight = true;
            isWalking = true;
        }
        if (keystates[SDL_SCANCODE_SPACE]) {
            if (!isJumping) {
                velocityY = -JUMP_FORCE;
                isJumping = true;
            }
        }

        // Gravitationslogik
        velocityY += GRAVITY;
        player.y += velocityY;

        if (player.y + PLAYER_HEIGHT >= ground.y) {
            player.y = ground.y - PLAYER_HEIGHT;
            velocityY = 0;
            isJumping = false;
        }

        // Animation uppdatering
        if (isWalking) {
            animationTimer++;
            if (animationTimer >= ANIMATION_SPEED) {
                animationTimer = 0;
                animationFrame = (animationFrame + 1) % 2;
            }

            if (animationFrame == 0)
                currentPlayerTex = walkFrame1Tex;
            else
                currentPlayerTex = walkFrame2Tex;
        } else {
            currentPlayerTex = idleTex;
            animationTimer = 0;
            animationFrame = 0;
        }

        // Rendering
        SDL_RenderClear(renderer);

        SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, backgroundTex, NULL, &bgRect);
        SDL_RenderCopy(renderer, groundTex, NULL, &ground);

        SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
        SDL_RenderCopyEx(renderer, currentPlayerTex, NULL, &player, 0, NULL, flip);

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // cirka 60 FPS
    }

    // Rensa resurser
    SDL_DestroyTexture(backgroundTex);
    SDL_DestroyTexture(idleTex);
    SDL_DestroyTexture(walkFrame1Tex);
    SDL_DestroyTexture(walkFrame2Tex);
    SDL_DestroyTexture(groundTex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();

    return 0;
}
