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

        void SpawnSprite(float _x, float _y, float _vx, float _vy, const gameobject::Sprite& _sprite);
        void SpawnRandomSprite(float _viewport_w, float _velocitySpeed);
        void SpawnBatch(std::size_t _count, float _viewport_w, float _velocitySpeed);

        void Update(float _dt, float _viewport_w, float _viewport_h, float _velocitySpeed);
        void BuildRenderInstances(std::vector<renderer::Sprite_Instance>& _out) const;

        std::size_t GetObjectCount() const noexcept { return objects.size(); }

    private:
        static std::unique_ptr<gameobject::GameObject> MakeSpriteGO(float _viewport_w, float _velocitySpeed);

        std::size_t reserveCount_{ 200000 };
        std::vector<std::unique_ptr<gameobject::GameObject>> objects;
    };

}