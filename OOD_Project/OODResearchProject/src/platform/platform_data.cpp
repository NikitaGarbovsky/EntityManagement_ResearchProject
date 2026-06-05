#include "platform_data.h"

#include <iostream>
#include <iomanip>

namespace sdl3platform {

    Platform gPlatform{};

    void InitFrameStats(FrameStats* _stats) {
        _stats->freq = SDL_GetPerformanceFrequency();
        _stats->last_counter = SDL_GetPerformanceCounter();
        _stats->accum_seconds = 0.0;
        _stats->frame_count = 0;
        _stats->fps = 0.0;
        _stats->ms_per_frame = 0.0;
        _stats->sim_time_ms = 0.0;
        _stats->render_ms = 0.0;
    }

    float TickFrameStats(FrameStats* _stats, int _count, bool& _spawnThisFrame) {
        const uint64_t now = SDL_GetPerformanceCounter();
        const uint64_t delta_counts = now - _stats->last_counter;
        _stats->last_counter = now;

        const double raw_dt = static_cast<double>(delta_counts) /
            static_cast<double>(_stats->freq);
        const float dt = static_cast<float>(raw_dt);

        _stats->accum_seconds += raw_dt;
        _stats->frame_count += 1;

        _spawnThisFrame = false;


        if (_stats->accum_seconds >= 1.0) {
            _stats->fps = static_cast<double>(_stats->frame_count) / _stats->accum_seconds;
            _stats->ms_per_frame = 1000.0 / _stats->fps;

            std::cout << std::fixed << std::setprecision(1)
                << "FPS: " << _stats->fps
                << "  Total Frame MS: "
                << std::setprecision(2) << _stats->ms_per_frame
                << "  Sim MS: "
                << std::setprecision(3) << _stats->sim_time_ms
                << ", Render MS: "
                << _stats->render_ms << '\n';

            std::cout << "EntityCount: " << _count << '\n';
            _spawnThisFrame = true;
            _stats->accum_seconds = 0.0;
            _stats->frame_count = 0;
        }

        return dt;
    }

}