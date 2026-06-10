#include "Scene.h"
#include <algorithm>
#include <random>
#include <iostream>

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

    void Scene::Init() {
        Clear();

        // Initialize all the pool resources upfront.
        pool.reserve(reserveCount_);
        freeIndices.reserve(reserveCount_);
        activeIndices.reserve(reserveCount_);

        for (std::size_t i = 0; i < reserveCount_; ++i) {
            pool.emplace_back(std::make_unique<gameobject::SpriteObject>());
            freeIndices.push_back(i);
        }
    }

    void Scene::Clear() {
        pool.clear();
        freeIndices.clear();
        activeIndices.clear();
        activeCount_ = 0;
    }

    std::size_t Scene::AcquireSlot() {
        if (freeIndices.empty()) {
            return INVALID_INDEX;
        }

        std::size_t idx = freeIndices.back();
        freeIndices.pop_back();
        return idx;
    }

    void Scene::ReleaseSlot(std::size_t _index) {
        if (_index >= pool.size()) {
            return;
        }

        pool[_index]->setActive(false);
        freeIndices.push_back(_index);
    }

    void Scene::SpawnRandomSprite(float _viewport_w, float _velocitySpeed) {
        const std::size_t idx = AcquireSlot();
        if (idx == INVALID_INDEX) {
            return;
        }

        // Reset this sprites position from the pool.
        pool[idx]->Reset(_viewport_w, _velocitySpeed);
        activeIndices.push_back(idx);
        ++activeCount_;
    }

    void Scene::Update(float _dt, float _viewport_w, float _viewport_h, float _velocitySpeed, int &_deathCount) {
        if (gustActive_)
        {
            gustTimer_ -= _dt;

            if (gustTimer_ <= 0.0f)
            {
                gustActive_ = false;
                gustTimer_ = 6.0f;

                std::cout << "gust ended\n";
            }
        }
        else
        {
            gustTimer_ -= _dt;

            if (gustTimer_ <= 0.0f)
            {
                gustActive_ = true;
                gustTimer_ = 2.0f;

                std::cout << "gust activated\n";
            }
        }
        for (std::size_t i = 0; i < activeIndices.size(); ++i) {
            const std::size_t idx = activeIndices[i];
            auto& obj = pool[idx];

            obj->Update(_dt, gustActive_, GUST_FORCE);

            if (gustActive_)
            {
                obj->ApplyGustDamage(
                    GUST_FORCE,
                    DAMAGE_SCALE,
                    _dt);
            }

            // If we're off screen, or we died
            if (obj->isOffscreen(_viewport_h) || obj->IsDead()) {
                _deathCount++;
                // release resources, give slot to free list,
                ReleaseSlot(idx);

                // remove from active list
                activeIndices[i] = activeIndices.back();
                activeIndices.pop_back();
                --activeCount_;

                continue;
            }
        }
    }

    void Scene::BuildRenderInstances(std::vector<renderer::Sprite_Instance>& _out) const {
        _out.clear();
        _out.reserve(activeCount_);

        for (const std::size_t idx : activeIndices) {
            const auto& obj = pool[idx];
            if (!obj || !obj->isActive()) {
                continue;
            }

            renderer::Sprite_Instance inst{};
            obj->BuildSpriteInstance(inst);
            _out.push_back(inst);
        }
    }

}