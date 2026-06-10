#pragma once

#include <cstddef>
#include <memory>
#include <vector>
#include "../gameobject/GameObject.h"
#include "../renderer/rendererData.h"

namespace scene {

    class Scene {
    public:
        explicit Scene(std::size_t _reserveCount = 200000);

        void Init();
        void Clear();

        // Spawns a random sprite with resources from the pool
        void SpawnRandomSprite(float _viewport_w, float _velocitySpeed);

        void Update(float _dt, float _viewport_w, float _viewport_h, float _velocitySpeed, int& _deathCount);
        void BuildRenderInstances(std::vector<renderer::Sprite_Instance>& _out) const;

        std::size_t GetObjectCount() const noexcept { return activeCount_; }

        bool IsGustActive() const { return gustActive_; }
    private:
        static constexpr std::size_t INVALID_INDEX = static_cast<std::size_t>(-1);

        std::size_t AcquireSlot();
        void ReleaseSlot(std::size_t _index);

        std::size_t reserveCount_{ 200000 };

        std::vector<std::unique_ptr<gameobject::SpriteObject>> pool;
        std::vector<std::size_t> freeIndices;
        std::vector<std::size_t> activeIndices;
        std::size_t activeCount_{ 0 };

        // Gust properties
        const float DAMAGE_SCALE = 6.0f;
        float gustTimer_ = 6.0f;
        bool gustActive_ = false;
        const float GUST_FORCE = 150.0f;
    };

}