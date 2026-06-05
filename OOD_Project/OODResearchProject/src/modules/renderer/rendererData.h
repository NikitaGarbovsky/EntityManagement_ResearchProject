#pragma once
#include <SDL3/sdl.h>
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace renderer {

    struct Camera2D {
        std::array<float, 2> position; // World-space center of the camera
        float zoom; // 1 = default, 2 = zoom in, 0.5 = zoom out
        std::array<int, 2> viewport_size; // pixels
    };

    struct Rect2D {
        std::array<float, 2> min;
        std::array<float, 2> max;
    };

    // Data for the rendered Sprite
    struct Sprite_Instance {
        std::array<float, 2> position;
        std::array<float, 2> size;
        std::array<float, 4> color;
    };

    // Holds resources data for sprites send to the renderer.
    struct Sprite_DrawResources {
        SDL_GPUBuffer* quad_vb;
        SDL_GPUBuffer* quad_ib;
        SDL_GPUBuffer* instance_buffer;
        SDL_GPUTransferBuffer* instance_transfer;
        uint32_t max_instances;
    };

    struct Renderer {
        SDL_GPUDevice* gpu;
        SDL_Window* window;
        SDL_GPUCommandBuffer* cmd_buf;
        SDL_GPUTexture* swapchain_tex;
        SDL_GPURenderPass* render_pass;
        SDL_GPUGraphicsPipeline* sprite_pipeline;

        std::array<float, 4> clear_color;
        Camera2D camera;
        Sprite_DrawResources sprite_draw_resources;
    };

    struct Sprite_Global_VS_Uniform {
        glm::mat4 view_proj;
    };

    struct Quad_Vertex {
        std::array<float, 2> local_pos;
    };

    

    

}