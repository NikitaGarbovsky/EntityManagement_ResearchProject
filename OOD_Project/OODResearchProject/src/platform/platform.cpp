#include <SDL3/SDL.h>
#include "platform.h"
#include "platform_data.h"

namespace sdl3platform {
    
    void init(Platform* _platform) {
        if (!SDL_Init(SDL_INIT_VIDEO))     {
            SDL_Log("SDL_Init failed: %s", SDL_GetError());
        }
    
        _platform->width = 1920;
        _platform->height = 1080;

        _platform->window = SDL_CreateWindow(
            "OOD Research Project",
            _platform->width,
            _platform->height,
            0
        );
    
        if (!_platform->window) {
            SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
            SDL_Quit();
        }
    
        SDL_GPUShaderFormat formatFlags = SDL_GPU_SHADERFORMAT_SPIRV;
        _platform->gpu = SDL_CreateGPUDevice(formatFlags, false, NULL);
        SDL_ClaimWindowForGPUDevice(_platform->gpu, _platform->window);
    
        bool ok = SDL_SetGPUSwapchainParameters(_platform->gpu, _platform->window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_IMMEDIATE);
        if (!ok) {
            SDL_Log("SDL_SETGPUSWapchainParaters failed: %s", SDL_GetError());
            SDL_Quit();
        }

        _platform->running = true;
        SDL_Log("--- Platform Successfully Initialized");
    }
    
    void ExecuteSdlEvents(Platform* _platform) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            
            case SDL_EVENT_QUIT:
                _platform->running = false;
                break;
            default:
                break;
            }
        }
    }

    void shutdown(Platform* _platform) {
        if (_platform->window) {
            SDL_DestroyWindow(_platform->window);
            _platform->window = nullptr;
        }
        SDL_Quit();
    }
    
}

