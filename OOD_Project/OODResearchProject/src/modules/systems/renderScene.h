#pragma once

#include <vector>
#include "../renderer/rendererData.h"
#include "../scene/Scene.h"

namespace systems {
    bool RenderScene(scene::Scene& _scene, renderer::Renderer& _renderer, std::vector<renderer::Sprite_Instance>& _out_instances);
}