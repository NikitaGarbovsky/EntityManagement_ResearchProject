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

        if (RandRange(0, 1) > 0.5) {
            health_ = Health{
                .enabled = true,
                .current = 100.0f,
                .max = 100.0f
            };
        }
        
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

    bool SpriteObject::HasHealth() const {
        return health_.enabled;
    }

    void SpriteObject::AddHealth(float maxHealth = 100.0f) {
        health_.enabled = true;
        health_.current = maxHealth;
        health_.max = maxHealth;
    }

    void SpriteObject::RemoveHealth() {
        health_.enabled = false;
    }

    void SpriteObject::Damage(float _amount) {
        if (!health_.enabled) {
            return;
        }

        health_.current -= _amount;

        if (health_.current < 0.0f) {
            health_.current = 0.0f;
        }
    }

    bool SpriteObject::IsDead() const {
        return health_.enabled &&
            health_.current <= 0.0f;
    }
}