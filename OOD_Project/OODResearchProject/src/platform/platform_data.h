#pragma once
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <cstdint>

namespace sdl3platform {

    struct Platform {
        SDL_Window* window;
        SDL_GPUDevice* gpu;
        bool running;
        int32_t width;
        int32_t height;
    };

    extern Platform gPlatform;
}

