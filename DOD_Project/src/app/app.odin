package application

import "core:fmt"

import math "core:math/linalg"
import sdl "vendor:sdl3"
import "../platform"
import "../modules/renderer"
import "../modules/systems"
import "../modules/ecs"

shader_frag_batch := #load("../../Resources/Shaders/sprite_batch.frag.spv")
shader_vert_batch := #load("../../Resources/Shaders/sprite_batch.vert.spv")

// Initializes the application
Init :: proc(_app : ^AppState) {
    _app.render_instances = make([dynamic]renderer.Sprite_Instance, 0, 200000)

    platform.Init(&_app.platform)
    renderer.Init(&_app.renderer, &_app.platform, shader_vert_batch, shader_frag_batch)
    ecs.Init(&_app.world)
    platform.InitFrameStats(&_app.stats)

    _app.renderer.camera.position = {960, 540}
    _app.renderer.camera.zoom = 1
}

// Runs the main loop of the application
Run :: proc(_app : ^AppState) {
    for _app.platform.running {
        platform.ExecuteSdlEvents(&_app.platform)

        // Check if a entity should be spawned this loop,
        entityCount := len(_app.world.transforms.data)
        dt, spawn := platform.TickFrameStats(&_app.stats, entityCount)
        if spawn { // Spawn it
            systems.SpawnEntity(&_app.world, f32(_app.platform.width), 5)
        }
        systems.SpawnEntity(&_app.world, f32(_app.platform.width), 5)
        systems.SpawnEntity(&_app.world, f32(_app.platform.width), 5)
        systems.SpawnEntity(&_app.world, f32(_app.platform.width), 5)
        systems.SpawnEntity(&_app.world, f32(_app.platform.width), 5)
        systems.SpawnEntity(&_app.world, f32(_app.platform.width), 5)
        systems.SpawnEntity(&_app.world, f32(_app.platform.width), 5)
        systems.SpawnEntity(&_app.world, f32(_app.platform.width), 5)

        // Run and record performance of the movement simulation
        sim_start := sdl.GetPerformanceCounter()
        systems.SimulateMovement(&_app.world, dt, f32(_app.platform.width), f32(_app.platform.height), 5)
        _app.stats.sim_time_ms = f64(sdl.GetPerformanceCounter() - sim_start) * 1000.0 / f64(_app.stats.freq)

        viewport_size := math.Vector2f32{f32(_app.platform.width), f32(_app.platform.height)}

        // Render and record performance of the render frame
        render_sim_start := sdl.GetPerformanceCounter()
        if renderer.BeginFrame(&_app.renderer, viewport_size) {
            _ = systems.RenderWorld(&_app.world, &_app.renderer, &_app.render_instances)
            renderer.EndFrame(&_app.renderer)
        }
        _app.stats.render_ms = f64(sdl.GetPerformanceCounter() - render_sim_start) * 1000.0 / f64(_app.stats.freq)
    }
}

Shutdown :: proc(_app : ^AppState) {
    renderer.Shutdown(&_app.renderer)
    platform.Shutdown(&_app.platform)
    fmt.println("Shutdown Successfully")
}
