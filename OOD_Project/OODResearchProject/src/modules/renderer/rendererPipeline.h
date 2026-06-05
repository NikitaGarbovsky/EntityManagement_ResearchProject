#pragma once
#include "rendererData.h"
#include <array>
#include <span>

namespace renderer {

    bool BeginFrame(
        Renderer* _renderer,  
        std::array<int, 2> _viewport_size);

    bool BeginScenePass(Renderer* _renderer);

    void EndFrame(Renderer* _renderer);

    void UploadSpriteInstances(
        Renderer* _renderer, 
        std::span<const Sprite_Instance> _instances);

    void DrawSpriteInstances(
        Renderer* _renderer, 
        uint32_t _instance_count);

}