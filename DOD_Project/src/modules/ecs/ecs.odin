package ecs

import components "../components"

Init :: proc(_world : ^Entity_World) {
    _world.next_entity = 0

    _world.transforms.index_of = make(map[Entity]int)
    _world.sprites.index_of = make(map[Entity]int)
    _world.velocities.index_of = make(map[Entity]int)
}

CreateEntity :: proc(_world : ^Entity_World) -> Entity {
    if len(_world.free_entities) > 0 {
        e := _world.free_entities[len(_world.free_entities) - 1]
        pop(&_world.free_entities)
        return e
    }

    e := Entity{id = _world.next_entity}
    _world.next_entity += 1
    return e
}

DeleteEntity :: proc(_world : ^Entity_World, _entityToDelete : Entity) {
    RemoveComponent(&_world.transforms, _entityToDelete)
    RemoveComponent(&_world.sprites, _entityToDelete)
    RemoveComponent(&_world.velocities, _entityToDelete)

    append(&_world.free_entities, _entityToDelete)
}

@private 
AddComponent :: proc(_compStore : ^Component_Store($T), _entity : Entity, _value: T) {
    if idx, exists := _compStore.index_of[_entity]; exists {
        _compStore.data[idx] = _value
        return
    }

    idx := len(_compStore.data)
    append(&_compStore.data, _value)
    append(&_compStore.entities, _entity)
    _compStore.index_of[_entity] = idx
}

AddComponentToEntityWorld :: proc(_world : ^Entity_World, _compStore : ^Component_Store($T), _entity : Entity, _value : T) {
    AddComponent(_compStore, _entity, _value)
}

@private
RemoveComponent :: proc(_compStore : ^Component_Store($T), _entity : Entity) {
    idx, exists := _compStore.index_of[_entity]

    if !exists do return

    last_index := len(_compStore.data) - 1
    last_entity := _compStore.entities[last_index]

    // Swaps the last component to the removed index spot, 
    _compStore.data[idx] = _compStore.data[last_index]
    _compStore.entities[idx] = _compStore.entities[last_index]
    _compStore.index_of[last_entity] = idx

    // then removes last index.
    pop(&_compStore.data)
    pop(&_compStore.entities)
    delete_key(&_compStore.index_of, _entity)
}

RemoveComponentFromEntityWorld :: proc(_world : ^Entity_World, _compStore : ^Component_Store($T), _entity : Entity) {
    RemoveComponent(_compStore, _entity)
}

HasComponent :: proc(_compStore : ^Component_Store($T), _entity : Entity)  -> bool {
    _, exists := _compStore.index_of[_entity]
    return exists
}

GetComponent :: proc(_compStore : ^Component_Store($T), _entity : Entity) -> (^T, bool) {
    idx, exists := _compStore.index_of[_entity]
    if !exists do return nil, false

    return &_compStore.data[idx], true
}