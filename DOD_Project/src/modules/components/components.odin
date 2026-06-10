package components

import math "core:math/linalg"

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
}

Health :: struct {
    enabled : bool,
    current : f32,
    max : f32,
}