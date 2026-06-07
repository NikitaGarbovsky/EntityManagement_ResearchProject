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
        int test_duration_sec;
        uint64_t freq;
        uint64_t last_counter;

        double accum_seconds;
        int frame_count;

        double fps;
        double avg_frame_ms;
        double avg_sim_ms;
        double avg_render_ms;

        double build_instances_ms;
        double upload_ms;
        double draw_ms;

        double accum_frame_ms;
        double accum_sim_ms;
        double accum_render_ms;
        double accum_build_instances_ms;
        double accum_upload_ms;
        double accum_draw_ms;
    };

    void InitFrameStats(FrameStats* _stats);
    float TickFrameStats(FrameStats* _stats, int _count, bool& _spawnThisFrame);

    extern Platform gPlatform;
}