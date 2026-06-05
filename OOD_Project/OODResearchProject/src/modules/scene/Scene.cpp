#include "Scene.h"
#include <algorithm>
#include <random>

namespace scene {
    // Helper for random number gen.
    float RandRange(float _min, float _max) {
        static std::mt19937 rng{ std::random_device{}() };
        std::uniform_real_distribution<float> dist(_min, _max);
        return dist(rng);
    }

    Scene::Scene(std::size_t _reserveCount)
        : reserveCount_(_reserveCount) {
    }

    std::unique_ptr<gameobject::GameObject> Scene::MakeSpriteGO(float _viewport_w, float _velocitySpeed) {
        gameobject::Transform transform{};
        transform.position = {
            RandRange(0.0f, _viewport_w),
            RandRange(0.0f, 10.0f)
        };

        gameobject::Velocity velocity{};
        velocity.linear = {
            RandRange(-10.0f, 10.0f),
            RandRange(10.0f, 30.0f) * _velocitySpeed
        };

        gameobject::Sprite sprite{};
        sprite.size = { 5.0f, 5.0f };
        sprite.color = {
            RandRange(0.0f, 1.0f),
            RandRange(0.0f, 1.0f),
            RandRange(0.0f, 1.0f),
            1.0f
        };

        return std::make_unique<gameobject::SpriteObject>(transform, velocity, sprite);
    }

    void Scene::Init() {
        objects.clear();
        objects.reserve(reserveCount_);
    }

    void Scene::Clear() {
        objects.clear();
    }

    void Scene::SpawnSprite(float _x, float _y, float _vx, float _vy, const gameobject::Sprite& _sprite) {
        gameobject::Transform transform{};
        transform.position = { _x, _y };

        gameobject::Velocity velocity{};
        velocity.linear = { _vx, _vy };

        objects.emplace_back(std::make_unique<gameobject::SpriteObject>(transform, velocity, _sprite));
    }

    void Scene::SpawnRandomSprite(float _viewport_w, float _velocitySpeed) {
        objects.emplace_back(MakeSpriteGO(_viewport_w, _velocitySpeed));
    }

    void Scene::SpawnBatch(std::size_t _count, float _viewport_w, float _velocitySpeed) {
        objects.reserve(objects.size() + _count);

        for (std::size_t i = 0; i < _count; ++i) {
            SpawnRandomSprite(_viewport_w, _velocitySpeed);
        }
    }

    void Scene::Update(float _dt, float _viewport_w, float _viewport_h, float _velocitySpeed) {
        for (auto& obj : objects) {
            if (!obj || !obj->isActive()) {
                continue;
            }

            obj->Update(_dt);

            // Culled,
            if (obj->isOffscreen(_viewport_h)) {
                // Creates a new object and assigns the existing obj ptr to it, 
                // freeing unique_ptr memory, allocating new memory. 
                obj = MakeSpriteGO(_viewport_w, _velocitySpeed);
            }
        }
    }

    void Scene::BuildRenderInstances(std::vector<renderer::Sprite_Instance>& _out) const {
        _out.clear();
        _out.reserve(objects.size());

        for (const auto& obj : objects) {
            if (!obj || !obj->isActive()) {
                continue;
            }

            renderer::Sprite_Instance inst{};
            obj->BuildSpriteInstance(inst);
            _out.push_back(inst);
        }
    }

}