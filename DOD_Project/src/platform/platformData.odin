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

    // Current frame samples
    frame_ms_sample : f64,
    sim_ms_sample : f64,
    render_ms_sample : f64,

    // Accumulators for averaging
    accum_frame_ms : f64,
    accum_sim_ms : f64,
    accum_render_ms : f64,
}

InitFrameStats :: proc(stats : ^Frame_Stats) {
    stats.freq = sdl.GetPerformanceFrequency()
    stats.last_counter = sdl.GetPerformanceCounter()
}

// Returns the delta time in seconds since the last call.
// Prints FPS + per-frame timings once per second.
TickFrameStats :: proc(_stats: ^Frame_Stats, _count: int) -> (dt: f32, spawnThisFrame: bool) {
    now := sdl.GetPerformanceCounter()
    delta_counts := now - _stats.last_counter
    _stats.last_counter = now

    raw_dt := f64(delta_counts) / f64(_stats.freq)
    dt = f32(raw_dt)

    _stats.accum_seconds += raw_dt
    _stats.frame_count += 1

    // Accumulate timing data for averaging
    _stats.accum_frame_ms += raw_dt * 1000.0
    _stats.accum_sim_ms += _stats.sim_ms_sample
    _stats.accum_render_ms += _stats.render_ms_sample

    if _stats.accum_seconds >= 1.0 {
        spawnThisFrame = true

        _stats.fps = f64(_stats.frame_count) / _stats.accum_seconds
        avg_frame_ms := _stats.accum_frame_ms / f64(_stats.frame_count)
        avg_sim_ms := _stats.accum_sim_ms / f64(_stats.frame_count)
        avg_render_ms := _stats.accum_render_ms / f64(_stats.frame_count)

        fmt.printfln(
            "FPS: %.1f  Total Frame MS: %.2f  Sim MS: %.3f, Render MS: %.3f",
            _stats.fps, avg_frame_ms, avg_sim_ms, avg_render_ms,
        )
        fmt.printfln("EntityCount: %d", _count)

        _stats.accum_seconds = 0
        _stats.frame_count = 0
        _stats.accum_frame_ms = 0
        _stats.accum_sim_ms = 0
        _stats.accum_render_ms = 0
    }

    return dt, spawnThisFrame
}
