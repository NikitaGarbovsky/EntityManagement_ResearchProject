#include "app.h"
#include <iostream>
#include <fstream>
#include "appData.h"

#include "SDL3/SDL_events.h"
#include "../platform/platform_data.h"
#include "../platform/platform.h"

#include "../modules/renderer/rendererAdmin.h"
#include "../modules/renderer/rendererPipeline.h"
#include "../modules/systems/renderScene.h"


namespace application {
    
    void Init(AppState* _appState) {
        sdl3platform::init(&_appState->platform);

        auto fragShaderStr = ReadShaderFile("../Resources/Shaders/sprite_batch.frag.spv");
        auto vertShaderStr = ReadShaderFile("../Resources/Shaders/sprite_batch.vert.spv");

        renderer::Init(&_appState->renderer, &_appState->platform, vertShaderStr, fragShaderStr);
        _appState->scene.Init();
        sdl3platform::InitFrameStats(&_appState->stats);

        _appState->renderer.camera.position = { 960, 540 };
        _appState->renderer.camera.zoom = 1.0f;

        //_appState->scene.SpawnBatch(10000, static_cast<float>(_appState->platform.width), 5.0f);

        std::cout << "--- App Successfully Initialized\n";
    }
    
    void Run(AppState* _app) {
        std::cout << "--- Running Simulation\n";
        while (_app->platform.running) {
            sdl3platform::ExecuteSdlEvents(&_app->platform);

            // Check the batch of GameObjects should be spawned this loop,
            bool spawn = false;
            const size_t entityCount = _app->scene.GetObjectCount();
            const float dt = sdl3platform::TickFrameStats(&_app->stats, entityCount, spawn);

            if (spawn && entityCount < 200000) { // Spawn em
                for (int i = 0; i < 20000; i++) {
                    _app->scene.SpawnRandomSprite(static_cast<float>(_app->platform.width), 5.0f);
                }
            }

            std::array<int, 2> viewportsize = { _app->platform.width, _app->platform.height };

            size_t sim_start = SDL_GetPerformanceCounter();
            _app->scene.Update(dt, viewportsize[0], viewportsize[1], 5.0f);
            _app->stats.avg_sim_ms = (SDL_GetPerformanceCounter() - sim_start) * 1000.0 / (_app->stats.freq);
            
            double build_ms = 0.0;
            double upload_ms = 0.0;
            double draw_ms = 0.0;

            size_t render_sim_start = SDL_GetPerformanceCounter();
            if (renderer::BeginFrame(&_app->renderer, viewportsize)) {
                systems::RenderScene(_app->scene, _app->renderer, _app->render_instances, build_ms, upload_ms, draw_ms);
                renderer::EndFrame(&_app->renderer);
            }
            _app->stats.avg_render_ms = (SDL_GetPerformanceCounter() - render_sim_start) * 1000.0 / (_app->stats.freq);

            _app->stats.build_instances_ms = build_ms;
            _app->stats.upload_ms = upload_ms;
            _app->stats.draw_ms = draw_ms;
        }
    }
    
    void Shutdown(AppState* _appState) {
        _appState->scene.Clear();
        renderer::Shutdown(&_appState->renderer);
        sdl3platform::shutdown(&_appState->platform);
        std::cout << "Shutdown Successfully\n";
    }
}
