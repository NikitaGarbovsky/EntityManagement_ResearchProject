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
        _stats->avg_frame_ms = 0.0;

        _stats->avg_sim_ms = 0.0;
        _stats->avg_render_ms = 0.0;
        _stats->build_instances_ms = 0.0;
        _stats->upload_ms = 0.0;
        _stats->draw_ms = 0.0;

        _stats->accum_frame_ms = 0.0;
        _stats->accum_sim_ms = 0.0;
        _stats->accum_render_ms = 0.0;
        _stats->accum_build_instances_ms = 0.0;
        _stats->accum_upload_ms = 0.0;
        _stats->accum_draw_ms = 0.0;
        _stats->test_duration_sec = 0.0;
        _stats->entitiesKilledLastFrame = 0;
    }

    float TickFrameStats(FrameStats* _stats, int _count) {
        const uint64_t now = SDL_GetPerformanceCounter();
        const uint64_t delta_counts = now - _stats->last_counter;
        _stats->last_counter = now;

        const double raw_dt = static_cast<double>(delta_counts) /
            static_cast<double>(_stats->freq);
        const float dt = static_cast<float>(raw_dt);

        _stats->accum_seconds += raw_dt;
        _stats->frame_count += 1;

        _stats->accum_frame_ms += raw_dt * 1000.0;
        _stats->accum_sim_ms += _stats->avg_sim_ms;
        _stats->accum_render_ms += _stats->avg_render_ms;
        _stats->accum_build_instances_ms += _stats->build_instances_ms;
        _stats->accum_upload_ms += _stats->upload_ms;
        _stats->accum_draw_ms += _stats->draw_ms;

        if (_stats->accum_seconds >= 1.0) {
            _stats->fps = static_cast<double>(_stats->frame_count) / _stats->accum_seconds;
            _stats->avg_frame_ms = _stats->accum_frame_ms / static_cast<double>(_stats->frame_count);
            _stats->avg_sim_ms = _stats->accum_sim_ms / static_cast<double>(_stats->frame_count);
            _stats->avg_render_ms = _stats->accum_render_ms / static_cast<double>(_stats->frame_count);
            _stats->build_instances_ms = _stats->accum_build_instances_ms / static_cast<double>(_stats->frame_count);
            _stats->upload_ms = _stats->accum_upload_ms / static_cast<double>(_stats->frame_count);
            _stats->draw_ms = _stats->accum_draw_ms / static_cast<double>(_stats->frame_count);
            _stats->test_duration_sec += 1;
            std::cout << std::fixed << std::setprecision(1)
                << "Duration: " << _stats->test_duration_sec
                << ", FPS: " << _stats->fps
                << "  Total Frame MS: " << std::setprecision(2) << _stats->avg_frame_ms
                << "  Sim MS: " << std::setprecision(3) << _stats->avg_sim_ms
                << ", Render MS: " << _stats->avg_render_ms
                << ", Build MS: " << _stats->build_instances_ms
                << ", Upload MS: " << _stats->upload_ms
                << ", Draw MS: " << _stats->draw_ms << '\n';

            std::cout << "EntityCount: " << _count << '\n';
            std::cout << "Entities Killed Last Frame: " <<
                _stats->entitiesKilledLastFrame << '\n';
            _stats->accum_seconds = 0.0;
            _stats->frame_count = 0;

            _stats->accum_frame_ms = 0.0;
            _stats->accum_sim_ms = 0.0;
            _stats->accum_render_ms = 0.0;
            _stats->accum_build_instances_ms = 0.0;
            _stats->accum_upload_ms = 0.0;
            _stats->accum_draw_ms = 0.0;
            _stats->entitiesKilledLastFrame = 0;
        }

        return dt;
    }

}