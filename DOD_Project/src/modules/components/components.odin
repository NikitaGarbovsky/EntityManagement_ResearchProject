package components

import math "core:math/linalg"

Component_Flag :: enum u16 {
    Transform,
    Velocity,
    Sprite,
}

Transform :: struct {
    pos : math.Vector2f32,
    rot : f32,
}

// Linear velocity in world units per second
Velocity :: struct {
    linear : math.Vector2f32,
}

Sprite :: struct {
    size : math.Vector2f32,
    color : [4]f32,
    // origin : [2]f32,
    // layer : i32,
}
