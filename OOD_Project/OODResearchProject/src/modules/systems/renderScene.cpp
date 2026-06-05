#include "renderScene.h"
#include "../renderer/rendererPipeline.h"
#include "SDL3/SDL.h"

namespace {

    double ToMs(uint64_t delta) {
        return (static_cast<double>(delta) * 1000.0) /
            static_cast<double>(SDL_GetPerformanceFrequency());
    }

}

namespace systems {

    bool RenderScene(
        scene::Scene& scene,
        renderer::Renderer& renderer,
        std::vector<renderer::Sprite_Instance>& out_instances,
        double& build_instances_ms,
        double& upload_ms,
        double& draw_ms
    ) {
        const uint64_t build_start = SDL_GetPerformanceCounter();
        scene.BuildRenderInstances(out_instances);
        build_instances_ms = ToMs(SDL_GetPerformanceCounter() - build_start);

        if (!renderer::BeginScenePass(&renderer)) {
            upload_ms = 0.0;
            draw_ms = 0.0;
            return false;
        }

        if (!out_instances.empty()) {
            const uint64_t upload_start = SDL_GetPerformanceCounter();
            renderer::UploadSpriteInstances(&renderer, out_instances);
            upload_ms = ToMs(SDL_GetPerformanceCounter() - upload_start);

            const uint64_t draw_start = SDL_GetPerformanceCounter();
            renderer::DrawSpriteInstances(&renderer, static_cast<uint32_t>(out_instances.size()));
            draw_ms = ToMs(SDL_GetPerformanceCounter() - draw_start);
        }
        else {
            upload_ms = 0.0;
            draw_ms = 0.0;
        }

        return true;
    }

}