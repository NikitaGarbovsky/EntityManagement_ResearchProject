#pragma once

#include <array>
#include "../renderer/rendererData.h"

namespace gameobject {

    struct Transform {
        std::array<float, 2> position{ 0.0f, 0.0f };
        float rotation = 0.0f;
    };

    struct Velocity {
        std::array<float, 2> linear{ 0.0f, 0.0f };
    };

    struct Sprite {
        std::array<float, 2> size{ 1.0f, 1.0f };
        std::array<float, 4> color{ 1.0f, 1.0f, 1.0f, 1.0f };
    };

    struct Health {
        bool enabled = false;
        float current = 100.0f;
        float max = 100.0f;
    };

    class GameObject {
    public:
        virtual ~GameObject() = default;
        virtual void Update(float _dt) = 0;
        virtual void BuildSpriteInstance(renderer::Sprite_Instance& _out) const = 0;
        virtual bool isOffscreen(float _viewport_h) const = 0;

        bool isActive() const noexcept { return active; }
        void setActive(bool _active) noexcept { active = _active; }

    protected:
        GameObject() = default;
        bool active = true;
    };

    class SpriteObject final : public GameObject {
    public:
        SpriteObject() = default;
        SpriteObject(Transform _transform, Velocity _velocity, Sprite _sprite) noexcept;

        void Update(float _dt) override;
        void BuildSpriteInstance(renderer::Sprite_Instance& _out) const override;
        bool isOffscreen(float _viewport_h) const override;

        // Used to reset sprite position
        void Reset(float _viewport_w, float _velocitySpeed);

        void setTransform(const Transform& _transform) noexcept { transform_ = _transform; }
        void setVelocity(const Velocity& _velocity) noexcept { velocity_ = _velocity; }
        void setSprite(const Sprite& _sprite) noexcept { sprite_ = _sprite; }
        void setHealth(const Health& _health) noexcept { health_ = _health; }

        const Transform& GetTransform() const noexcept { return transform_; }
        const Velocity& GetVelocity() const noexcept { return velocity_; }
        const Sprite& GetSprite() const noexcept { return sprite_; }

        bool HasHealth() const;
        void AddHealth(float _maxHealth);
        void RemoveHealth();
        void Damage(float _amount);
        bool IsDead() const;
    private:
        Transform transform_{};
        Velocity velocity_{};
        Sprite sprite_{};
        Health health_{};
    };

}