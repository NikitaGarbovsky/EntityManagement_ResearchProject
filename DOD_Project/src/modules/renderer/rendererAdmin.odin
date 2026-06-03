package renderer

import sdl "vendor:sdl3"
import "core:log"
import glm "core:math/linalg/glsl"
import "../../platform"

Init :: proc(_renderer : ^Renderer, _platform : ^platform.Platform, _vert_code, _frag_code : []u8) -> bool {
    _renderer.gpu = _platform.gpu
    _renderer.window = _platform.window
    _renderer.clear_color = {0.0, 0.0, 0.0, 1.0}

    _renderer.camera.position = glm.vec2{0, 0}
    _renderer.camera.zoom = 1.0
    _renderer.camera.viewport_size = glm.vec2{1920, 1080}

    if !InitSpriteRendererResources(_renderer, 200000) do return false
    if !InitSpritePipeline(_renderer, _vert_code, _frag_code) do return false

    return true
}


// Initializes all GPU resources for the sprite renderer
InitSpriteRendererResources :: proc(_renderer : ^Renderer, _max_instances : u32) -> bool {
    instancer := &_renderer.sprite_draw_resources
    instancer.max_instances = _max_instances

    quad_vertices := [4]Quad_Vertex{
        {local_pos = {0.0, 0.0}},
        {local_pos = {1.0, 0.0}},
        {local_pos = {1.0, 1.0}},
        {local_pos = {0.0, 1.0}},
    }
    quad_indices := [6]u16{0, 1, 2, 0, 2, 3}

    quad_vb_size := u32(len(quad_vertices)) * u32(size_of(Quad_Vertex))
    quad_ib_size := u32(len(quad_indices)) * u32(size_of(u16))
    instance_buf_size  := _max_instances * u32(size_of(Sprite_Instance))

    instancer.quad_vb = sdl.CreateGPUBuffer(_renderer.gpu, sdl.GPUBufferCreateInfo{usage = {.VERTEX}, size = quad_vb_size})
    if instancer.quad_vb == nil {
        log.errorf("CreateGPUBuffer quad_vb failed: {}", sdl.GetError())
        return false
    }

    instancer.quad_ib = sdl.CreateGPUBuffer(_renderer.gpu, sdl.GPUBufferCreateInfo{usage = {.INDEX}, size = quad_ib_size})
    if instancer.quad_ib == nil {
        log.errorf("CreateGPUBuffer quad_ib failed: {}", sdl.GetError())
        return false
    }

    instancer.instance_buffer = sdl.CreateGPUBuffer(_renderer.gpu, sdl.GPUBufferCreateInfo{usage = {.VERTEX}, size = instance_buf_size})
    if instancer.instance_buffer == nil {
        log.errorf("CreateGPUBuffer instance_buffer failed: {}", sdl.GetError())
        return false
    }

    instancer.instance_transfer = sdl.CreateGPUTransferBuffer(_renderer.gpu, sdl.GPUTransferBufferCreateInfo{usage = .UPLOAD, size = instance_buf_size})
    if instancer.instance_transfer == nil {
        log.errorf("CreateGPUTransferBuffer instance_transfer failed: {}", sdl.GetError())
        return false
    }

    // One-time upload of the shared quad geometry
    quad_vb_xfer := sdl.CreateGPUTransferBuffer(_renderer.gpu, sdl.GPUTransferBufferCreateInfo{usage = .UPLOAD, size = quad_vb_size})
    if quad_vb_xfer == nil {
        log.errorf("CreateGPUTransferBuffer quad_vb_xfer failed: {}", sdl.GetError())
        return false
    }
    defer sdl.ReleaseGPUTransferBuffer(_renderer.gpu, quad_vb_xfer)

    quad_ib_xfer := sdl.CreateGPUTransferBuffer(_renderer.gpu, sdl.GPUTransferBufferCreateInfo{usage = .UPLOAD, size = quad_ib_size})
    if quad_ib_xfer == nil {
        log.errorf("CreateGPUTransferBuffer quad_ib_xfer failed: {}", sdl.GetError())
        return false
    }
    defer sdl.ReleaseGPUTransferBuffer(_renderer.gpu, quad_ib_xfer)

    mapped_vb := ([^]Quad_Vertex)(sdl.MapGPUTransferBuffer(_renderer.gpu, quad_vb_xfer, false))
    if mapped_vb == nil {
        log.errorf("MapGPUTransferBuffer quad_vb_xfer failed: {}", sdl.GetError())
        return false
    }
    copy(mapped_vb[:len(quad_vertices)], quad_vertices[:])
    sdl.UnmapGPUTransferBuffer(_renderer.gpu, quad_vb_xfer)

    mapped_ib := ([^]u16)(sdl.MapGPUTransferBuffer(_renderer.gpu, quad_ib_xfer, false))
    if mapped_ib == nil {
        log.errorf("MapGPUTransferBuffer quad_ib_xfer failed: {}", sdl.GetError())
        return false
    }
    copy(mapped_ib[:len(quad_indices)], quad_indices[:])
    sdl.UnmapGPUTransferBuffer(_renderer.gpu, quad_ib_xfer)

    cmd_buf := sdl.AcquireGPUCommandBuffer(_renderer.gpu)
    if cmd_buf == nil {
        log.errorf("AcquireGPUCommandBuffer (batcher init) failed: {}", sdl.GetError())
        return false
    }

    copy_pass := sdl.BeginGPUCopyPass(cmd_buf)
    sdl.UploadToGPUBuffer(copy_pass,
        sdl.GPUTransferBufferLocation{transfer_buffer = quad_vb_xfer, offset = 0},
        sdl.GPUBufferRegion{buffer = instancer.quad_vb, offset = 0, size = quad_vb_size},
        false)
    sdl.UploadToGPUBuffer(copy_pass,
        sdl.GPUTransferBufferLocation{transfer_buffer = quad_ib_xfer, offset = 0},
        sdl.GPUBufferRegion{buffer = instancer.quad_ib, offset = 0, size = quad_ib_size},
        false)
    sdl.EndGPUCopyPass(copy_pass)

    if !sdl.SubmitGPUCommandBuffer(cmd_buf) {
        log.errorf("SubmitGPUCommandBuffer (batcher init) failed: {}", sdl.GetError())
        return false
    }

    return true
}

InitSpritePipeline :: proc(_renderer : ^Renderer, _vert_code, _frag_code : []u8) -> bool {
    vert_shader := sdl.CreateGPUShader(_renderer.gpu, sdl.GPUShaderCreateInfo{
        code_size = len(_vert_code),
        code = raw_data(_vert_code),
        entrypoint = "main",
        format = {.SPIRV},
        stage = .VERTEX,
        num_uniform_buffers = 4,
    })
    if vert_shader == nil {
        log.errorf("CreateGPUShader vertex failed: {}", sdl.GetError())
        return false
    }

    frag_shader := sdl.CreateGPUShader(_renderer.gpu, sdl.GPUShaderCreateInfo{
        code_size = len(_frag_code),
        code = raw_data(_frag_code),
        entrypoint = "main",
        format = {.SPIRV},
        stage = .FRAGMENT,
    })
    if frag_shader == nil {
        log.errorf("CreateGPUShader fragment failed: {}", sdl.GetError())
        sdl.ReleaseGPUShader(_renderer.gpu, vert_shader)
        return false
    }

    vertex_buffer_descs := [2]sdl.GPUVertexBufferDescription{
        {
            slot = 0,
            pitch = u32(size_of(Quad_Vertex)),
            input_rate = .VERTEX,
            instance_step_rate = 0,
        },
        {
            slot = 1,
            pitch = u32(size_of(Sprite_Instance)),
            input_rate = .INSTANCE,
            instance_step_rate = 0,
        },
    }

    vertex_attrs := [4]sdl.GPUVertexAttribute{
        {location = 0, buffer_slot = 0, format = .FLOAT2, offset = u32(offset_of(Quad_Vertex, local_pos))},
        {location = 1, buffer_slot = 1, format = .FLOAT2, offset = u32(offset_of(Sprite_Instance, position))},
        {location = 2, buffer_slot = 1, format = .FLOAT2, offset = u32(offset_of(Sprite_Instance, size))},
        {location = 3, buffer_slot = 1, format = .FLOAT4, offset = u32(offset_of(Sprite_Instance, color))},
    }

    color_target_desc := sdl.GPUColorTargetDescription{
        format = sdl.GetGPUSwapchainTextureFormat(_renderer.gpu, _renderer.window),
        blend_state = sdl.GPUColorTargetBlendState{
            src_color_blendfactor = .SRC_ALPHA,
            dst_color_blendfactor = .ONE_MINUS_SRC_ALPHA,
            color_blend_op = .ADD,
            src_alpha_blendfactor = .SRC_ALPHA,
            dst_alpha_blendfactor = .ONE_MINUS_SRC_ALPHA,
            alpha_blend_op = .ADD,
            enable_blend = true,
            enable_color_write_mask = false,
        },
    }

    _renderer.sprite_pipeline = sdl.CreateGPUGraphicsPipeline(_renderer.gpu, sdl.GPUGraphicsPipelineCreateInfo{
        vertex_shader = vert_shader,
        fragment_shader = frag_shader,

        vertex_input_state = sdl.GPUVertexInputState{
            vertex_buffer_descriptions = &vertex_buffer_descs[0],
            num_vertex_buffers = 2,
            vertex_attributes = &vertex_attrs[0],
            num_vertex_attributes = 4,
        },

        primitive_type = .TRIANGLELIST,

        rasterizer_state = sdl.GPURasterizerState{
            fill_mode = .FILL,
            cull_mode = .NONE,
            front_face = .COUNTER_CLOCKWISE,
            enable_depth_clip = true,
        },

        multisample_state = sdl.GPUMultisampleState{
            sample_count = ._1,
        },

        depth_stencil_state = sdl.GPUDepthStencilState{
            compare_op = .ALWAYS,
            enable_depth_test = false,
        },

        target_info = sdl.GPUGraphicsPipelineTargetInfo{
            color_target_descriptions = &color_target_desc,
            num_color_targets = 1,
            depth_stencil_format = .INVALID,
            has_depth_stencil_target  = false,
        },
    })

    sdl.ReleaseGPUShader(_renderer.gpu, vert_shader)
    sdl.ReleaseGPUShader(_renderer.gpu, frag_shader)

    if _renderer.sprite_pipeline == nil {
        log.errorf("CreateGPUGraphicsPipeline sprite failed: {}", sdl.GetError())
        return false
    }

    return true
}

ShutdownSpriteRendererResources :: proc(_renderer : ^Renderer) {
    dr := &_renderer.sprite_draw_resources

    if dr.quad_vb != nil { sdl.ReleaseGPUBuffer(_renderer.gpu, dr.quad_vb); dr.quad_vb = nil }
    if dr.quad_ib != nil { sdl.ReleaseGPUBuffer(_renderer.gpu, dr.quad_ib); dr.quad_ib = nil }
    if dr.instance_buffer != nil { sdl.ReleaseGPUBuffer(_renderer.gpu, dr.instance_buffer); dr.instance_buffer = nil }
    if dr.instance_transfer != nil { sdl.ReleaseGPUTransferBuffer(_renderer.gpu, dr.instance_transfer); dr.instance_transfer = nil }
}

Shutdown :: proc(_renderer : ^Renderer) {
    ShutdownSpriteRendererResources(_renderer)

    if _renderer.sprite_pipeline != nil {
        sdl.ReleaseGPUGraphicsPipeline(_renderer.gpu, _renderer.sprite_pipeline)
        _renderer.sprite_pipeline = nil
    }
}