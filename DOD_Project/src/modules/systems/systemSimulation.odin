package systems

import "../ecs"
import "../components"
import "core:math/rand"
import "core:math"
import "core:fmt"

GUST_FORCE : f32 : 150.0
DAMAGE_SCALE : f32 : 6.0

gust_timer : f32 = 6.0
gust_active : bool = false

SimulateMovement :: proc(_world : ^ecs.Entity_World, _dt : f32, _viewport_w, _viewport_h : f32, _velocitySpeed : f32, _deathCount : ^int) {
    if gust_active {
        gust_timer -= _dt

        if gust_timer <= 0 {
            gust_active = false
            gust_timer = 6.0

            fmt.println("gust ended")
        }
    } else {
        gust_timer -= _dt

        if gust_timer <= 0 {
            gust_active = true
            gust_timer = 2.0

            fmt.println("gust activated")
        }
    }

    for i := len(_world.transforms.data) - 1; i >= 0; i -= 1 {
        effective_velocity_y :=
            _world.velocities.data[i].linear.y

        if gust_active {
            effective_velocity_y -= GUST_FORCE
        }

        _world.transforms.data[i].pos.x +=
            _world.velocities.data[i].linear.x * _dt

        _world.transforms.data[i].pos.y +=
            effective_velocity_y * _dt

        // Damage all entities with health components enabled.
        if _world.healths.data[i].enabled {
            upward_velocity :=
                math.max(
                    0,
                    -effective_velocity_y)

            damage :=
                upward_velocity *
                DAMAGE_SCALE *
                _dt

            _world.healths.data[i].current -= damage

            if _world.healths.data[i].current <= 0 {
                _world.healths.data[i].enabled = false
            }
        }


        // Delete components and recycle entity id if below screen
        if _world.transforms.data[i].pos.y > _viewport_h + 20 || _world.healths.data[i].current <= 0 {
            _deathCount^ += 1
            e := _world.transforms.entities[i]
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

    ecs.AddComponentToEntityWorld(_world, &_world.healths, e, components.Health{ 
        enabled = true, 
        max = 100, 
        current = 100, 
    })
    
}
