#include "GameObject.h"
#include <random>

namespace gameobject {

    float RandRange(float min, float max) {
        static std::mt19937 rng{ std::random_device{}() };
        std::uniform_real_distribution<float> dist(min, max);
        return dist(rng);
    }

    SpriteObject::SpriteObject(Transform _transform, Velocity _velocity, Sprite _sprite) noexcept
        : transform_(_transform), velocity_(_velocity), sprite_(_sprite) {
    }

    void SpriteObject::Reset(float _viewport_w, float _velocitySpeed) {
        active = true;

        transform_.position = {
            RandRange(0.0f, _viewport_w),
            RandRange(0.0f, 10.0f)
        };

        velocity_.linear = {
            RandRange(-10.0f, 10.0f),
            RandRange(10.0f, 30.0f) * _velocitySpeed
        };

        sprite_.size = { 5.0f, 5.0f };
        sprite_.color = {
            RandRange(0.0f, 1.0f),
            RandRange(0.0f, 1.0f),
            RandRange(0.0f, 1.0f),
            1.0f
        };
    }

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