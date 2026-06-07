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

    fmt.printfln("--- App Successfully Initialized")
}

// Runs the main loop of the application
Run :: proc(_app : ^AppState) {
    fmt.printfln("--- Running Simlutation")
    for _app.platform.running {
        platform.ExecuteSdlEvents(&_app.platform)

        // Get data for spawning time
        spawn : bool = false
        entityCount := len(_app.world.transforms.data)
        // Check if the batches of entities should be spawned this loop.
        dt := platform.TickFrameStats(&_app.stats, entityCount, &spawn)
        if spawn && entityCount < 200000 { // Spawn it
            for i := 0; i < 20000; i += 1 {
                systems.SpawnEntity(&_app.world, f32(_app.platform.width), 5)
            }
        }

        // Run and record performance of the movement simulation
        sim_start := sdl.GetPerformanceCounter()
        systems.SimulateMovement(&_app.world, dt, f32(_app.platform.width), f32(_app.platform.height), 5)
        _app.stats.avg_sim_ms = f64(sdl.GetPerformanceCounter() - sim_start) * 1000.0 / f64(_app.stats.freq)

        viewport_size := math.Vector2f32{f32(_app.platform.width), f32(_app.platform.height)}

        _app.stats.build_instances_ms = 0
        _app.stats.upload_ms = 0
        _app.stats.draw_ms = 0

        render_start := sdl.GetPerformanceCounter()
        if renderer.BeginFrame(&_app.renderer, viewport_size) {
            _ = systems.RenderWorld(
                &_app.world,
                &_app.renderer,
                &_app.render_instances,
                _app.stats.freq,
                &_app.stats.build_instances_ms,
                &_app.stats.upload_ms,
                &_app.stats.draw_ms,
            )
            renderer.EndFrame(&_app.renderer)
        }
        _app.stats.avg_render_ms = f64(sdl.GetPerformanceCounter() - render_start) * 1000.0 / f64(_app.stats.freq)
    }
}

Shutdown :: proc(_app : ^AppState) {
    renderer.Shutdown(&_app.renderer)
    platform.Shutdown(&_app.platform)
    fmt.println("Shutdown Successfully")
}
