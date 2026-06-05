package systems

import "core:fmt"
import sdl "vendor:sdl3"

import "../ecs"
import "../renderer"

ms_from_ticks :: proc(delta: u64, freq: u64) -> f64 {
    return f64(delta) * 1000.0 / f64(freq)
}

// Step by step process of rendering all the entities with sprite components.
RenderWorld :: proc(
    _world : ^ecs.Entity_World,
    _renderer : ^renderer.Renderer,
    _out_instances : ^[dynamic]renderer.Sprite_Instance,
    _freq : u64,
    build_instances_ms : ^f64,
    upload_ms : ^f64,
    draw_ms : ^f64,
) -> bool {
    clear(_out_instances)

    build_start := sdl.GetPerformanceCounter()
    BuildSpriteInstances(_world, _out_instances)
    build_instances_ms^ = ms_from_ticks(sdl.GetPerformanceCounter() - build_start, _freq)

    if len(_out_instances) == 0 {
        upload_ms^ = 0
        draw_ms^ = 0
        return true
    }

    upload_start := sdl.GetPerformanceCounter()
    renderer.UploadSpriteInstances(_renderer, _out_instances[:])
    upload_ms^ = ms_from_ticks(sdl.GetPerformanceCounter() - upload_start, _freq)

    if !renderer.BeginWorldPass(_renderer) {
        draw_ms^ = 0
        return false
    }

    draw_start := sdl.GetPerformanceCounter()
    renderer.DrawSpriteInstances(_renderer, u32(len(_out_instances)))
    draw_ms^ = ms_from_ticks(sdl.GetPerformanceCounter() - draw_start, _freq)

    return true
}

// Collect all the sprite_instances needing to be rendered.
BuildSpriteInstances :: proc(_world : ^ecs.Entity_World, _out_instances : ^[dynamic]renderer.Sprite_Instance) {
    clear(_out_instances)

    count := len(_world.transforms.data)
    for i := 0; i < count; i += 1 {
        t := _world.transforms.data[i]
        s := _world.sprites.data[i]

        append(_out_instances,
            renderer.Sprite_Instance{
                position = {t.pos.x, t.pos.y},
                size = {s.size.x, s.size.y},
                color = s.color,
            })
    }
}