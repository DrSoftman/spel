#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <math.h>
#include "../include/spel.h" // game macros

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

    // Add camera position variables
    float cameraX = 0;
    float cameraY = 0;
    float targetCameraX = 0;
    float targetCameraY = 0;

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

        // Update camera target position (center on player)
        targetCameraX = player.x - SCREEN_WIDTH / 2 + PLAYER_WIDTH / 2;
        targetCameraY = player.y - SCREEN_HEIGHT / 2 + PLAYER_HEIGHT / 2;

        // Apply camera smoothing with deadzone
        if (fabs(targetCameraX - cameraX) > CAMERA_DEADZONE) {
            cameraX += (targetCameraX - cameraX) * CAMERA_SMOOTHING;
        }
        if (fabs(targetCameraY - cameraY) > CAMERA_DEADZONE) {
            cameraY += (targetCameraY - cameraY) * CAMERA_SMOOTHING;
        }

        // Clamp camera to world bounds (optional)
        if (cameraX < 0) cameraX = 0;
        if (cameraY < 0) cameraY = 0;

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

        // Render repeating background with camera offset
        // Calculate how many background tiles we need to cover the screen
        int bgWidth = SCREEN_WIDTH;
        int bgHeight = SCREEN_HEIGHT;
        
        // Calculate the starting position for the first background tile
        int startX = (int)cameraX % bgWidth;
        int startY = (int)cameraY % bgHeight;
        
        // Render multiple background tiles to cover the entire screen
        for (int x = -1; x <= 1; x++) {
            for (int y = -1; y <= 1; y++) {
                SDL_Rect bgRect = {
                    x * bgWidth - startX,
                    y * bgHeight - startY,
                    bgWidth,
                    bgHeight
                };
                SDL_RenderCopy(renderer, backgroundTex, NULL, &bgRect);
            }
        }
        
        // Render repeating ground with camera offset
        // Calculate how many ground tiles we need to cover the screen width
        int groundWidth = SCREEN_WIDTH;
        int groundHeight = 50; // Height of the ground
        
        // Calculate the starting position for the first ground tile
        int groundStartX = (int)cameraX % groundWidth;
        
        // Render multiple ground tiles to cover the entire screen width
        for (int x = -1; x <= 1; x++) {
            SDL_Rect groundOffset = {
                x * groundWidth - groundStartX,
                ground.y - (int)cameraY,
                groundWidth,
                groundHeight
            };
            SDL_RenderCopy(renderer, groundTex, NULL, &groundOffset);
        }

        // Render player with camera offset
        SDL_Rect playerOffset = {player.x - (int)cameraX, player.y - (int)cameraY, player.w, player.h};
        SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
        SDL_RenderCopyEx(renderer, currentPlayerTex, NULL, &playerOffset, 0, NULL, flip);

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
