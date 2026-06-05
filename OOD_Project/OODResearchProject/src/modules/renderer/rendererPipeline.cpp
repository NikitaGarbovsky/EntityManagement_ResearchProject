#include "rendererPipeline.h"
#include <iostream>
#include <cassert>
#include "rendererCamera.h"

namespace renderer {
    
    bool BeginFrame(Renderer* _renderer, std::array<int, 2> _viewport_size) {
        _renderer->camera.viewport_size = _viewport_size;

        _renderer->cmd_buf = SDL_AcquireGPUCommandBuffer(_renderer->gpu);
        if (_renderer->cmd_buf == nullptr) {
            std::cerr << "AcquireGPUCommandBuffer failed: " << SDL_GetError() << "\n";
            return false;
        }

        bool ok = SDL_WaitAndAcquireGPUSwapchainTexture(
            _renderer->cmd_buf,
            _renderer->window,
            &_renderer->swapchain_tex,
            nullptr,
            nullptr
        );

        if (!ok || _renderer->swapchain_tex == nullptr) {
            std::cerr << "WaitAndAcquireGPUSwapchainTexture failed: " << SDL_GetError() << "\n";
            return false;
        }

        return true;
    }
    bool BeginScenePass(Renderer* _renderer) {
        if (_renderer->cmd_buf == nullptr || _renderer->swapchain_tex == nullptr) {
            return false;
        }

        SDL_GPUColorTargetInfo color_target{
            .texture = _renderer->swapchain_tex,
            .clear_color = {
                _renderer->clear_color[0],
                _renderer->clear_color[1],
                _renderer->clear_color[2],
                _renderer->clear_color[3]
            },
            .load_op = SDL_GPU_LOADOP_CLEAR,
            .store_op = SDL_GPU_STOREOP_STORE
        };

        _renderer->render_pass = SDL_BeginGPURenderPass(_renderer->cmd_buf, &color_target, 1, nullptr);
        if (_renderer->render_pass == nullptr) {
            std::cerr << "BeginGPURenderPass failed: " << SDL_GetError() << "\n";
            return false;
        }

        return true;
    }

    void EndFrame(Renderer* renderer) {
        if (renderer->render_pass != nullptr) {
            SDL_EndGPURenderPass(renderer->render_pass);
            renderer->render_pass = nullptr;
        }

        if (renderer->cmd_buf != nullptr) {
            bool ok = SDL_SubmitGPUCommandBuffer(renderer->cmd_buf);
            if (!ok) {
                std::cerr << "SubmitGPUCommandBuffer failed: " << SDL_GetError() << "\n";
            }
            renderer->cmd_buf = nullptr;
        }

        renderer->swapchain_tex = nullptr;
    }

    void UploadSpriteInstances(Renderer* _renderer, std::span<const Sprite_Instance> _instances) {
        // 1. Early out if empty
        if (_instances.empty()) return;

        // 2. Calculate bytes needed and assert bounds
        uint32_t bytes_needed = static_cast<uint32_t>(_instances.size_bytes());

        if (bytes_needed >= _renderer->sprite_draw_resources.max_instances * sizeof(Sprite_Instance)) {
            // #TODO: close application here
        }
        
        // 3. Grant CPU access to the transfer buffer
        void* gpu_buffer_ptr = SDL_MapGPUTransferBuffer(
            _renderer->gpu,
            _renderer->sprite_draw_resources.instance_transfer,
            true
        );

        if (gpu_buffer_ptr == nullptr) {
            std::cerr << "MapGPUTransferBuffer failed: " << SDL_GetError() << "\n";
            return;
        }

        // 4. Copy the sprite data to the GPU buffer
        std::memcpy(gpu_buffer_ptr, _instances.data(), bytes_needed);

        // 5. Remove CPU access
        SDL_UnmapGPUTransferBuffer(_renderer->gpu, _renderer->sprite_draw_resources.instance_transfer);

        // 6. Create copy pass, upload, and cycle
        SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(_renderer->cmd_buf);

        SDL_GPUTransferBufferLocation source_location{
            .transfer_buffer = _renderer->sprite_draw_resources.instance_transfer,
            .offset = 0
        };

        SDL_GPUBufferRegion destination_region{
            .buffer = _renderer->sprite_draw_resources.instance_buffer,
            .offset = 0,
            .size = bytes_needed
        };

        SDL_UploadToGPUBuffer(copy_pass, &source_location, &destination_region, true);
        SDL_EndGPUCopyPass(copy_pass);
    }

    void DrawSpriteInstances(Renderer* _renderer, uint32_t _instance_count) {
        // 1. Early out if resources or render pass aren't active/bound
        if (_renderer->render_pass == nullptr ||
            _renderer->sprite_pipeline == nullptr ||
            _instance_count == 0) {
            return;
        }

        // 2. Push the camera view_proj matrix uniform for the vertex shader
        Sprite_Global_VS_Uniform global_vs{
            CameraViewProjMatrix(&_renderer->camera)
        };

        SDL_PushGPUVertexUniformData(
            _renderer->cmd_buf,
            0,
            &global_vs,
            static_cast<uint32_t>(sizeof(Sprite_Global_VS_Uniform))
        );

        // 3. Bind the graphics pipeline state to the current render pass
        SDL_BindGPUGraphicsPipeline(_renderer->render_pass, _renderer->sprite_pipeline);

        // 4. Bind the geometry vertex buffer (slot 0) and instance buffer (slot 1)
        SDL_GPUBufferBinding vb_bindings[2] = {
            {.buffer = _renderer->sprite_draw_resources.quad_vb, .offset = 0 },
            {.buffer = _renderer->sprite_draw_resources.instance_buffer, .offset = 0 }
        };

        // Pass the pointer to the first element and specify 2 bindings
        SDL_BindGPUVertexBuffers(_renderer->render_pass, 0, &vb_bindings[0], 2);

        // 5. Bind the 16-bit element index buffer for the quad layout
        SDL_GPUBufferBinding ib_binding{
            .buffer = _renderer->sprite_draw_resources.quad_ib,
            .offset = 0
        };

        SDL_BindGPUIndexBuffer(
            _renderer->render_pass,
            &ib_binding,
            SDL_GPU_INDEXELEMENTSIZE_16BIT 
        );

        // 6. Submit the instanced draw command: 6 indices per quad, N instances
        SDL_DrawGPUIndexedPrimitives(
            _renderer->render_pass,
            6,
            _instance_count,
            0,
            0,
            0
        );
    }

}



