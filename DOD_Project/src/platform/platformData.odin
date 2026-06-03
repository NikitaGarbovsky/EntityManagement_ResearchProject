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
    accum_seconds : f64,
    accumSpawnThreshhold : f64,
    frame_count : i32,
    fps : f64,
    ms_per_frame : f64,
    sim_time_ms : f64,
    render_ms : f64,
}

InitFrameStats :: proc(stats : ^Frame_Stats) {
    stats.freq = sdl.GetPerformanceFrequency()
    stats.last_counter = sdl.GetPerformanceCounter()
}

// Returns the delta time in seconds since the last call.
// Prints FPS + per-frame timings once per second.
TickFrameStats :: proc(stats : ^Frame_Stats, _count : int) -> (dt : f32, spawnThisFrame : bool) {
    now := sdl.GetPerformanceCounter()
    delta_counts := now - stats.last_counter
    stats.last_counter = now

    raw_dt := f64(delta_counts) / f64(stats.freq)
    dt = f32(raw_dt)

    stats.accum_seconds += raw_dt
    stats.accumSpawnThreshhold += raw_dt
    stats.frame_count += 1

    if stats.accumSpawnThreshhold >= 0.0001 {
        spawnThisFrame = true
        stats.accumSpawnThreshhold = 0
    }

    if stats.accum_seconds >= 1.0 {
        stats.fps = f64(stats.frame_count) / stats.accum_seconds
        stats.ms_per_frame = 1000.0 / stats.fps
        fmt.printfln("FPS: %.1f  Total Frame MS: %.2f  Sim MS: %.3f, Render MS: %.3f",
            stats.fps, stats.ms_per_frame, stats.sim_time_ms, stats.render_ms)
            fmt.printfln("EntityCount: %d", _count)
        stats.accum_seconds = 0
        stats.frame_count = 0
    }

    return dt, spawnThisFrame
}
