package systems

import "../ecs"
import "../renderer"

RenderWorld :: proc(_world : ^ecs.Entity_World, _renderer : ^renderer.Renderer) -> bool {
    BuildSpriteInstances(_world, &_renderer.sprite_batcher.instances)

    if len(_renderer.sprite_batcher.instances) == 0 do return true

    renderer.UploadInstancedata(_renderer, _renderer.sprite_batcher.instances[:])

    if !renderer.BeginWorldPass(_renderer) do return false
    renderer.SubmitSpriteInstances(_renderer, u32(len(_renderer.sprite_batcher.instances)))

    return true
}

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
