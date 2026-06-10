package platform

import sdl "vendor:sdl3"
import "core:fmt"

Platform :: struct {
    window : ^sdl.Window,
    gpu : ^sdl.GPUDevice,
    running : bool,
    width : i32,
    height : i32,
}

Frame_Stats :: struct {
    test_duration_sec : u64,
    freq : u64,
    last_counter : u64,

    // 1-second accumulation window
    accum_seconds : f64,
    frame_count : i32,

    // Current averaged results
    fps : f64,
    avg_frame_ms : f64,
    avg_sim_ms : f64,
    avg_render_ms : f64,

    build_instances_ms : f64,
    upload_ms : f64,
    draw_ms : f64,

    accum_frame_ms : f64,
    accum_sim_ms : f64,
    accum_render_ms : f64,
    accum_build_instances_ms : f64,
    accum_upload_ms : f64,
    accum_draw_ms : f64,
    entityDeathsLastFrame : int,
}

InitFrameStats :: proc(stats : ^Frame_Stats) {
    stats.freq = sdl.GetPerformanceFrequency()
    stats.last_counter = sdl.GetPerformanceCounter()
}

// Returns the delta time in seconds since the last call.
// Prints FPS + per-frame timings once per second.
TickFrameStats :: proc(_stats: ^Frame_Stats, _count: int, _spawnThisFrame: ^bool) -> f32 {
    now := sdl.GetPerformanceCounter()
    delta_counts := now - _stats.last_counter
    _stats.last_counter = now

    raw_dt := f64(delta_counts) / f64(_stats.freq)
    dt := f32(raw_dt)

    _stats.accum_seconds += raw_dt
    _stats.frame_count += 1

    _stats.accum_frame_ms += raw_dt * 1000.0
    _stats.accum_sim_ms += _stats.avg_sim_ms
    _stats.accum_render_ms += _stats.avg_render_ms
    _stats.accum_build_instances_ms += _stats.build_instances_ms
    _stats.accum_upload_ms += _stats.upload_ms
    _stats.accum_draw_ms += _stats.draw_ms

    _spawnThisFrame^ = false

    if _stats.accum_seconds >= 1.0 {
        _stats.fps = f64(_stats.frame_count) / _stats.accum_seconds
        _stats.avg_frame_ms = _stats.accum_frame_ms / f64(_stats.frame_count)
        _stats.avg_sim_ms = _stats.accum_sim_ms / f64(_stats.frame_count)
        _stats.avg_render_ms = _stats.accum_render_ms / f64(_stats.frame_count)
        _stats.build_instances_ms = _stats.accum_build_instances_ms / f64(_stats.frame_count)
        _stats.upload_ms = _stats.accum_upload_ms / f64(_stats.frame_count)
        _stats.draw_ms = _stats.accum_draw_ms / f64(_stats.frame_count)
        _stats.test_duration_sec += 1
        fmt.printfln(
            "Duration: %v FPS: %.1f  Total Frame MS: %.2f  Sim MS: %.3f, Render MS: %.3f, Build MS: %.3f, Upload MS: %.3f, Draw MS: %.3f",
            _stats.test_duration_sec,
            _stats.fps,
            _stats.avg_frame_ms,
            _stats.avg_sim_ms,
            _stats.avg_render_ms,
            _stats.build_instances_ms,
            _stats.upload_ms,
            _stats.draw_ms,
        )

        fmt.printfln("EntityCount: %d", _count)
        fmt.printfln("Deaths Last Frame: %d", _stats.entityDeathsLastFrame)
        _spawnThisFrame^ = true
        _stats.accum_seconds = 0
        _stats.frame_count = 0

        _stats.accum_frame_ms = 0
        _stats.accum_sim_ms = 0
        _stats.accum_render_ms = 0
        _stats.accum_build_instances_ms = 0
        _stats.accum_upload_ms = 0
        _stats.accum_draw_ms = 0
        _stats.entityDeathsLastFrame = 0
    }

    
    return dt
}
