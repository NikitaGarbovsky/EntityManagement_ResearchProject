#include "renderScene.h"
#include "../renderer/rendererPipeline.h"

namespace systems {

    bool RenderScene(scene::Scene& _scene, renderer::Renderer& _renderer, std::vector<renderer::Sprite_Instance>& _out_instances) {
        _scene.BuildRenderInstances(_out_instances);

        if (!renderer::BeginScenePass(&_renderer)) {
            return false;
        }

        if (!_out_instances.empty()) {
            renderer::UploadSpriteInstances(&_renderer, _out_instances);
            renderer::DrawSpriteInstances(&_renderer, static_cast<uint32_t>(_out_instances.size()));
        }

        return true;
    }

}