package ecs

import components "../components"

Entity :: struct {
    id : u32
}

// Holds references to entities and their associated components
Entity_World :: struct {
    next_entity : u32,
    free_entities : [dynamic]Entity, 

    transforms : Component_Store(components.Transform),
    sprites : Component_Store(components.Sprite),
    velocities : Component_Store(components.Velocity),
}

// Holds references to the components and their associated entities for the ECS system.
Component_Store :: struct($T : typeid) {
    data : [dynamic]T, // Dense array of component data
    entities : [dynamic]Entity, // Paralell array of owning entities 
    index_of : map[Entity]int // Lookup from entity to index
}