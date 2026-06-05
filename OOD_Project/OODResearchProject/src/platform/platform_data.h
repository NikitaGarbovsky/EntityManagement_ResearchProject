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

    struct FrameStats {
        uint64_t freq;
        uint64_t last_counter;
        double accum_seconds;
        double accumSpawnThreshhold;
        int frame_count;
        double fps;
        double ms_per_frame;
        double sim_time_ms;
        double render_ms;
    };

    void InitFrameStats(FrameStats* _stats);
    float TickFrameStats(FrameStats* _stats, int _count, bool& _spawnThisFrame);

    extern Platform gPlatform;
}