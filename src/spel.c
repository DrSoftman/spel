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

    SDL_Window* window = SDL_CreateWindow("Plattformsspel i C",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture* backgroundTex = IMG_LoadTexture(renderer, "art/background.png");
    SDL_Texture* playerTex = IMG_LoadTexture(renderer, "art/player.png");
    SDL_Texture* groundTex = IMG_LoadTexture(renderer, "art/ground.png");

    if (!backgroundTex || !playerTex || !groundTex) {
        SDL_Log("Kunde inte ladda texturer: %s", SDL_GetError());
        return 1;
    }

    SDL_Rect player = {100, SCREEN_HEIGHT - 110, PLAYER_WIDTH, PLAYER_HEIGHT};
    int velocityY = 0;
    bool isJumping = false;

    SDL_Rect ground = {0, SCREEN_HEIGHT - 50, SCREEN_WIDTH, 50};

    bool running = true;
    SDL_Event event;

    // 🔄 Ny variabel för riktning
    bool facingRight = true;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
        }

        const Uint8* keystates = SDL_GetKeyboardState(NULL);
        if (keystates[SDL_SCANCODE_LEFT]) {
            player.x -= PLAYER_SPEED;
            facingRight = false; // ⬅️ Vänd åt vänster
        }
        if (keystates[SDL_SCANCODE_RIGHT]) {
            player.x += PLAYER_SPEED;
            facingRight = true; // ➡️ Vänd åt höger
        }
        if (keystates[SDL_SCANCODE_SPACE]) {
            if (!isJumping) {
                velocityY = -JUMP_FORCE;
                isJumping = true;
            }
        }

        velocityY += GRAVITY;
        player.y += velocityY;

        if (player.y + PLAYER_HEIGHT >= ground.y) {
            player.y = ground.y - PLAYER_HEIGHT;
            velocityY = 0;
            isJumping = false;
        }

        SDL_RenderClear(renderer);

        SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, backgroundTex, NULL, &bgRect);

        SDL_RenderCopy(renderer, groundTex, NULL, &ground);

        // Rendera spelaren med spegelvändning
        SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
        SDL_RenderCopyEx(renderer, playerTex, NULL, &player, 0, NULL, flip);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyTexture(backgroundTex);
    SDL_DestroyTexture(playerTex);
    SDL_DestroyTexture(groundTex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();

    return 0;
}
