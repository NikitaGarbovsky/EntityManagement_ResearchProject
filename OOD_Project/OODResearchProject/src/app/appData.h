#pragma once

#include <vector>
#include "../platform/platform_data.h"
#include "../modules/renderer/rendererData.h"
#include "../modules/scene/Scene.h"
#include <string>

namespace application {

    std::string ReadShaderFile(const char* filename);

    struct AppState {
        sdl3platform::Platform platform;
        scene::Scene scene;
        renderer::Renderer renderer;
        sdl3platform::FrameStats stats;
        std::vector<renderer::Sprite_Instance> render_instances;
    };

    extern AppState gAppState;
}