#include "GameObject.h"

namespace gameobject {

    SpriteObject::SpriteObject(Transform _transform, Velocity _velocity, Sprite _sprite) noexcept
        : transform_(_transform), velocity_(_velocity), sprite_(_sprite) {}

    void SpriteObject::Update(float _dt) {
        if (!active) {
            return;
        }

        transform_.position[0] += velocity_.linear[0] * _dt;
        transform_.position[1] += velocity_.linear[1] * _dt;
    }

    void SpriteObject::BuildSpriteInstance(renderer::Sprite_Instance& _out) const {
        _out.position = transform_.position;
        _out.size = sprite_.size;
        _out.color = sprite_.color;
    }

    bool SpriteObject::isOffscreen(float _viewport_h) const {
        return transform_.position[1] > _viewport_h + 20.0f;
    }

}