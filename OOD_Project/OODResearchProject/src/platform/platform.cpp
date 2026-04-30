#include <SDL3/SDL.h>
#include "platform.h"
#include "platform_data.h"

namespace platform {
    
void init() {
    if (!SDL_Init(SDL_INIT_VIDEO))     {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
    }
    
    window = SDL_CreateWindow(
        "OOD Research Project",
        1920,
        1080,
        0
    );
    
    if (!window) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
    }
    
    running = true;
    SDL_Log("Platform Successfully Initialized");
}
    
void shutdown() {
    if (window) {
        SDL_DestroyWindow(window);    
        window = nullptr;
    }
    SDL_Quit();
}
    
}

