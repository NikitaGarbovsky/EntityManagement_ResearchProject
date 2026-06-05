#include "rendererAdmin.h"
#include <cstdint>
#include <span>

namespace renderer {
	void Init(
		renderer::Renderer* _renderer,
		sdl3platform::Platform* _platform,
		std::string _vert_code,
		std::string _frag_code) {

		_renderer->gpu = _platform->gpu;
		_renderer->window = _platform->window;
		_renderer->clear_color = { 0.0, 0.0, 1.0, 1.0 };

		_renderer->camera.position = { 0.0, 0.0 };
		_renderer->camera.zoom = 1.0;
		_renderer->camera.viewport_size = { 1920, 1080 };

		bool ok = InitSpriteRendererResources(_renderer, 200000); SDL_assert(ok);
		bool ok1 = InitSpritePipeline(
            _renderer, 
            std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(_vert_code.data()), _vert_code.size()),
            std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(_frag_code.data()), _frag_code.size())); 
        SDL_assert(ok1);
	}

    bool InitSpriteRendererResources(Renderer* _renderer, uint32_t _max_instances) {
        Sprite_DrawResources* instancer = &_renderer->sprite_draw_resources;
        _renderer->sprite_draw_resources.max_instances = _max_instances;

        Quad_Vertex quad_vertices[4] = {
            {.local_pos = {0.0f, 0.0f} },
            {.local_pos = {1.0f, 0.0f} },
            {.local_pos = {1.0f, 1.0f} },
            {.local_pos = {0.0f, 1.0f} }
        };
        uint16_t quad_indices[6] = { 0, 1, 2, 0, 2, 3 };

        uint32_t quad_vb_size = static_cast<uint32_t>(sizeof(quad_vertices));
        uint32_t quad_ib_size = static_cast<uint32_t>(sizeof(quad_indices));
        uint32_t instance_buf_size = _max_instances * static_cast<uint32_t>(sizeof(Sprite_Instance));

        SDL_GPUBufferCreateInfo vb_info{ .usage = SDL_GPU_BUFFERUSAGE_VERTEX, .size = quad_vb_size };
        instancer->quad_vb = SDL_CreateGPUBuffer(_renderer->gpu, &vb_info);
        if (instancer->quad_vb == nullptr) {
            std::cerr << "CreateGPUBuffer quad_vb failed: " << SDL_GetError() << "\n";
            return false;
        }

        SDL_GPUBufferCreateInfo ib_info{ .usage = SDL_GPU_BUFFERUSAGE_INDEX, .size = quad_ib_size };
        instancer->quad_ib = SDL_CreateGPUBuffer(_renderer->gpu, &ib_info);
        if (instancer->quad_ib == nullptr) {
            std::cerr << "CreateGPUBuffer quad_ib failed: " << SDL_GetError() << "\n";
            return false;
        }

        SDL_GPUBufferCreateInfo inst_info{ .usage = SDL_GPU_BUFFERUSAGE_VERTEX, .size = instance_buf_size };
        instancer->instance_buffer = SDL_CreateGPUBuffer(_renderer->gpu, &inst_info);
        if (instancer->instance_buffer == nullptr) {
            std::cerr << "CreateGPUBuffer instance_buffer failed: " << SDL_GetError() << "\n";
            return false;
        }

        SDL_GPUTransferBufferCreateInfo inst_xfer_info{ .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = instance_buf_size };
        instancer->instance_transfer = SDL_CreateGPUTransferBuffer(_renderer->gpu, &inst_xfer_info);
        if (instancer->instance_transfer == nullptr) {
            std::cerr << "CreateGPUTransferBuffer instance_transfer failed: " << SDL_GetError() << "\n";
            return false;
        }

        SDL_GPUTransferBufferCreateInfo vb_xfer_info{ .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = quad_vb_size };
        SDL_GPUTransferBuffer* quad_vb_xfer = SDL_CreateGPUTransferBuffer(_renderer->gpu, &vb_xfer_info);
        if (quad_vb_xfer == nullptr) {
            std::cerr << "CreateGPUTransferBuffer quad_vb_xfer failed: " << SDL_GetError() << "\n";
            return false;
        }

        SDL_GPUTransferBufferCreateInfo ib_xfer_info{ .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = quad_ib_size };
        SDL_GPUTransferBuffer* quad_ib_xfer = SDL_CreateGPUTransferBuffer(_renderer->gpu, &ib_xfer_info);
        if (quad_ib_xfer == nullptr) {
            std::cerr << "CreateGPUTransferBuffer quad_ib_xfer failed: " << SDL_GetError() << "\n";
            SDL_ReleaseGPUTransferBuffer(_renderer->gpu, quad_vb_xfer);
            return false;
        }

        auto cleanup_xfers = [&]() {
            SDL_ReleaseGPUTransferBuffer(_renderer->gpu, quad_vb_xfer);
            SDL_ReleaseGPUTransferBuffer(_renderer->gpu, quad_ib_xfer);
            };

        void* vertex_tbuf_ptr = SDL_MapGPUTransferBuffer(_renderer->gpu, quad_vb_xfer, false);
        if (vertex_tbuf_ptr == nullptr) {
            std::cerr << "MapGPUTransferBuffer quad_vb_xfer failed: " << SDL_GetError() << "\n";
            cleanup_xfers();
            return false;
        }
        std::memcpy(vertex_tbuf_ptr, quad_vertices, quad_vb_size);
        SDL_UnmapGPUTransferBuffer(_renderer->gpu, quad_vb_xfer);

        void* indices_tbuf_ptr = SDL_MapGPUTransferBuffer(_renderer->gpu, quad_ib_xfer, false);
        if (indices_tbuf_ptr == nullptr) {
            std::cerr << "MapGPUTransferBuffer quad_ib_xfer failed: " << SDL_GetError() << "\n";
            cleanup_xfers();
            return false;
        }
        std::memcpy(indices_tbuf_ptr, quad_indices, quad_ib_size);
        SDL_UnmapGPUTransferBuffer(_renderer->gpu, quad_ib_xfer);

        SDL_GPUCommandBuffer* cmd_buf = SDL_AcquireGPUCommandBuffer(_renderer->gpu);
        if (cmd_buf == nullptr) {
            std::cerr << "AcquireGPUCommandBuffer (batcher init) failed: " << SDL_GetError() << "\n";
            cleanup_xfers();
            return false;
        }

        SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(cmd_buf);

        SDL_GPUTransferBufferLocation vb_src{ .transfer_buffer = quad_vb_xfer, .offset = 0 };
        SDL_GPUBufferRegion vb_dst{ .buffer = instancer->quad_vb, .offset = 0, .size = quad_vb_size };
        SDL_UploadToGPUBuffer(copy_pass, &vb_src, &vb_dst, false);

        SDL_GPUTransferBufferLocation ib_src{ .transfer_buffer = quad_ib_xfer, .offset = 0 };
        SDL_GPUBufferRegion ib_dst{ .buffer = instancer->quad_ib, .offset = 0, .size = quad_ib_size };
        SDL_UploadToGPUBuffer(copy_pass, &ib_src, &ib_dst, false);

        SDL_EndGPUCopyPass(copy_pass);

        if (!SDL_SubmitGPUCommandBuffer(cmd_buf)) {
            std::cerr << "SubmitGPUCommandBuffer (batcher init) failed: " << SDL_GetError() << "\n";
            cleanup_xfers();
            return false;
        }

        cleanup_xfers();
        return true;
    }

    bool InitSpritePipeline(Renderer* _renderer, std::span<const uint8_t> _vert_code, std::span<const uint8_t> _frag_code) {
        SDL_GPUShaderCreateInfo vert_info{
            .code_size = _vert_code.size(),
            .code = _vert_code.data(),
            .entrypoint = "main",
            .format = SDL_GPU_SHADERFORMAT_SPIRV,
            .stage = SDL_GPU_SHADERSTAGE_VERTEX,
            .num_uniform_buffers = 4
        };
        SDL_GPUShader* vert_shader = SDL_CreateGPUShader(_renderer->gpu, &vert_info);
        if (vert_shader == nullptr) {
            std::cerr << "CreateGPUShader vertex failed: " << SDL_GetError() << "\n";
            return false;
        }

        SDL_GPUShaderCreateInfo frag_info{
            .code_size = _frag_code.size(),
            .code = _frag_code.data(),
            .entrypoint = "main",
            .format = SDL_GPU_SHADERFORMAT_SPIRV,
            .stage = SDL_GPU_SHADERSTAGE_FRAGMENT
        };
        SDL_GPUShader* frag_shader = SDL_CreateGPUShader(_renderer->gpu, &frag_info);
        if (frag_shader == nullptr) {
            std::cerr << "CreateGPUShader fragment failed: " << SDL_GetError() << "\n";
            SDL_ReleaseGPUShader(_renderer->gpu, vert_shader);
            return false;
        }

        SDL_GPUVertexBufferDescription vertex_buffer_descs[2] = {
            {
                .slot = 0,
                .pitch = static_cast<uint32_t>(sizeof(Quad_Vertex)),
                .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                .instance_step_rate = 0
            },
            {
                .slot = 1,
                .pitch = static_cast<uint32_t>(sizeof(Sprite_Instance)),
                .input_rate = SDL_GPU_VERTEXINPUTRATE_INSTANCE,
                .instance_step_rate = 0
            }
        };

        SDL_GPUVertexAttribute vertex_attrs[4] = {
            {.location = 0, .buffer_slot = 0, .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, 
            .offset = static_cast<uint32_t>(offsetof(Quad_Vertex, local_pos)) },
            {.location = 1, .buffer_slot = 1, .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, 
            .offset = static_cast<uint32_t>(offsetof(Sprite_Instance, position)) },
            {.location = 2, .buffer_slot = 1, .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, 
            .offset = static_cast<uint32_t>(offsetof(Sprite_Instance, size)) },
            {.location = 3, .buffer_slot = 1, .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4, 
            .offset = static_cast<uint32_t>(offsetof(Sprite_Instance, color)) }
        };

        SDL_GPUColorTargetDescription color_target_desc{
            .format = SDL_GetGPUSwapchainTextureFormat(_renderer->gpu, _renderer->window),
            .blend_state = {
                .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
                .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                .color_blend_op = SDL_GPU_BLENDOP_ADD,
                .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
                .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
                .enable_blend = true,
                .enable_color_write_mask = false
            }
        };

        SDL_GPUGraphicsPipelineCreateInfo pipeline_info{
            .vertex_shader = vert_shader,
            .fragment_shader = frag_shader,
            .vertex_input_state = {
                .vertex_buffer_descriptions = &vertex_buffer_descs[0],
                .num_vertex_buffers = 2,
                .vertex_attributes = &vertex_attrs[0],
                .num_vertex_attributes = 4
            },
            .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
            .rasterizer_state = {
                .fill_mode = SDL_GPU_FILLMODE_FILL,
                .cull_mode = SDL_GPU_CULLMODE_NONE,
                .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE,
                .enable_depth_clip = true
            },
            .multisample_state = {
                .sample_count = SDL_GPU_SAMPLECOUNT_1
            },
            .depth_stencil_state = {
                .compare_op = SDL_GPU_COMPAREOP_ALWAYS,
                .enable_depth_test = false
            },
            .target_info = {
                .color_target_descriptions = &color_target_desc,
                .num_color_targets = 1,
                .depth_stencil_format = SDL_GPU_TEXTUREFORMAT_INVALID,
                .has_depth_stencil_target = false
            }
        };

        _renderer->sprite_pipeline = SDL_CreateGPUGraphicsPipeline(_renderer->gpu, &pipeline_info);

        SDL_ReleaseGPUShader(_renderer->gpu, vert_shader);
        SDL_ReleaseGPUShader(_renderer->gpu, frag_shader);

        if (_renderer->sprite_pipeline == nullptr) {
            std::cerr << "CreateGPUGraphicsPipeline sprite failed: " << SDL_GetError() << "\n";
            return false;
        }

        return true;
    }

	void ShutdownSpriteRendererResources(
		struct Renderer* _renderer) {
        // #TODO: fill out
	}

	void Shutdown(struct Renderer* _renderer) {
        // #TODO: fill out
	}
}