package systems

import "../ecs"
import "../components"
import "core:math/rand"

SimulateMovement :: proc(_world : ^ecs.Entity_World, _dt : f32, _viewport_w, _viewport_h : f32, _velocitySpeed : f32) {
    for i := len(_world.transforms.entities) - 1; i >= 0; i -= 1 {
        e := _world.transforms.entities[i]

        vel, ok := ecs.GetComponent(&_world.velocities, e)
        if !ok do continue

        _world.transforms.data[i].pos.x += vel.linear.x * _dt
        _world.transforms.data[i].pos.y += vel.linear.y * _dt

        if _world.transforms.data[i].pos.y > _viewport_h + 20 {
            ecs.DeleteEntity(_world, e)
            SpawnEntity(_world, _viewport_w, _velocitySpeed)
        }
    }
}

SpawnEntity :: proc(_world : ^ecs.Entity_World, _viewport_w : f32, _velocitySpeed : f32) {
    e := ecs.CreateEntity(_world)

    ecs.AddComponentToEntityWorld(_world, &_world.transforms, e,
        components.Transform{pos = {rand.float32_range(0, _viewport_w), rand.float32_range(0, 10)}, rot = 0})

    ecs.AddComponentToEntityWorld(_world, &_world.velocities, e,
        components.Velocity{linear = {rand.float32_range(-10, 10), rand.float32_range(10, 30) * _velocitySpeed}})

    ecs.AddComponentToEntityWorld(_world, &_world.sprites, e,
        components.Sprite{
            size = {5, 5},
            color = {rand.float32(), rand.float32(), rand.float32(), 1},
        })
}
