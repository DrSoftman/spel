#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define PLAYER_WIDTH 90 
#define PLAYER_HEIGHT 100
#define PLAYER_SPEED 6
#define JUMP_FORCE 15
#define GRAVITY 1

#define WALK_FRAME_COUNT 2 // Ändra talet beroende på hur många fler animationframes du vill ha
#define ANIMATION_SPEED 10

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    SDL_Window* window = SDL_CreateWindow("spel i C",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Ladda texturer
    SDL_Texture* backgroundTex = IMG_LoadTexture(renderer, "art/background.png");
    SDL_Texture* idleTex = IMG_LoadTexture(renderer, "art/player-sprites/idle_player.png");
    SDL_Texture* groundTex = IMG_LoadTexture(renderer, "art/ground.png");

    // Ladda gångframes
    SDL_Texture* walkFrames[WALK_FRAME_COUNT];
    walkFrames[0] = IMG_LoadTexture(renderer, "art/player-sprites/frame1_walk_player.png");
    walkFrames[1] = IMG_LoadTexture(renderer, "art/player-sprites/frame2_walk_player.png");

    // Kontrollera texturer
    if (!backgroundTex || !idleTex || !groundTex) {
        SDL_Log("Kunde inte ladda bakgrund eller idle-textur: %s", SDL_GetError());
        return 1;
    }
    for (int i = 0; i < WALK_FRAME_COUNT; i++) {
        if (!walkFrames[i]) {
            SDL_Log("Kunde inte ladda gångtextur %d: %s", i, SDL_GetError());
            return 1;
        }
    }

    // Spelarens egenskaper
    SDL_Rect player = {100, SCREEN_HEIGHT - 110, PLAYER_WIDTH, PLAYER_HEIGHT};
    int velocityY = 0;
    bool isJumping = false;

    SDL_Rect ground = {0, SCREEN_HEIGHT - 50, SCREEN_WIDTH, 50};

    bool running = true;
    SDL_Event event;

    bool facingRight = true;
    int animationTimer = 0;
    int animationFrame = 0;
    SDL_Texture* currentPlayerTex = idleTex;

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
                animationFrame = (animationFrame + 1) % WALK_FRAME_COUNT;
            }
            currentPlayerTex = walkFrames[animationFrame];
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
    SDL_DestroyTexture(groundTex);
    for (int i = 0; i < WALK_FRAME_COUNT; i++) {
        SDL_DestroyTexture(walkFrames[i]);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();

    return 0;
}
