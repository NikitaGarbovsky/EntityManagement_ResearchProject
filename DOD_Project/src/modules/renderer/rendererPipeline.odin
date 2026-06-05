package renderer

import sdl "vendor:sdl3"
import "core:log"
import math "core:math/linalg"
import glm "core:math/linalg/glsl"
import "core:mem"
import "core:fmt"

/// SDL3 GPU Renderer Pipeline:
///
/// 0. Allocate gpu resources, configure: Init, InitSpriteRendererResources, InitSpritePipeline
/// ========== Core loop ==========
/// 1. Get command buffer & swapchain texture: BeginFrame
/// 2. Collect all sprite instances this frame, upload to buffer: UploadSpriteInstances
/// 3. Start render pass: BeginWorldPass
/// 4. Draw sprites: DrawSpriteInstances
/// 5. EndFrame 

// Acquire command buffer & swapchain texture resources for frame.
BeginFrame :: proc(_renderer : ^Renderer, _viewport_size : math.Vector2f32) -> bool {
    _renderer.camera.viewport_size = _viewport_size

    _renderer.cmd_buf = sdl.AcquireGPUCommandBuffer(_renderer.gpu)
    if _renderer.cmd_buf == nil {
        log.errorf("AcquireGPUCommandBuffer failed: {}", sdl.GetError())
        return false
    }

    ok := sdl.WaitAndAcquireGPUSwapchainTexture(
        _renderer.cmd_buf, _renderer.window,
        &_renderer.swapchain_tex, nil, nil)
    if !ok || _renderer.swapchain_tex == nil {
        log.errorf("WaitAndAcquireGPUSwapchainTexture failed: {}", sdl.GetError())
        return false
    }

    return true
}

// Begin the entity world pass. 
BeginWorldPass :: proc(_renderer : ^Renderer) -> bool {
    if _renderer.cmd_buf == nil || _renderer.swapchain_tex == nil do return false

    color_target := sdl.GPUColorTargetInfo{
            texture = _renderer.swapchain_tex,
            load_op = .CLEAR,
            clear_color = {
            _renderer.clear_color[0],
            _renderer.clear_color[1],
            _renderer.clear_color[2],
            _renderer.clear_color[3],
        },
        store_op = .STORE,
    }

    _renderer.render_pass = sdl.BeginGPURenderPass(_renderer.cmd_buf, &color_target, 1, nil)
    if _renderer.render_pass == nil {
        log.errorf("BeginGPURenderPass failed: {}", sdl.GetError())
        return false
    }

    return true
}

// End the frame
EndFrame :: proc(_renderer : ^Renderer) {
    if _renderer.render_pass != nil {
        sdl.EndGPURenderPass(_renderer.render_pass)
        _renderer.render_pass = nil
    }

    if _renderer.cmd_buf != nil {
        ok := sdl.SubmitGPUCommandBuffer(_renderer.cmd_buf)
        if !ok do log.errorf("SubmitGPUCommandBuffer failed: {}", sdl.GetError())
        _renderer.cmd_buf = nil
    }

    _renderer.swapchain_tex = nil
}

// Uploads all the sprite_instance data to the gpu using the pre-configured pipeline
UploadSpriteInstances :: proc(_renderer : ^Renderer, _instances : []Sprite_Instance) {
    if len(_instances) == 0 do return

    bytes_needed := u32(len(_instances)) * size_of(Sprite_Instance)
    //fmt.printfln("%v",bytes_needed)
    assert(bytes_needed <= _renderer.sprite_draw_resources.max_instances * size_of(Sprite_Instance))

    // Grant access to cpu to send data
    gpu_buffer_ptr := sdl.MapGPUTransferBuffer(_renderer.gpu, _renderer.sprite_draw_resources.instance_transfer, true)
    if gpu_buffer_ptr == nil {
        log.errorf("MapGPUTransferBuffer failed: {}", sdl.GetError())
        return
    }

    // Copy the sprite data to the gpu buffer
    mem.copy(gpu_buffer_ptr, raw_data(_instances), int(bytes_needed))

    // Remove access
    sdl.UnmapGPUTransferBuffer(_renderer.gpu, _renderer.sprite_draw_resources.instance_transfer)

    // Create the copy pass, upload data to it, cycle the buffer for next frame.
    copy_pass := sdl.BeginGPUCopyPass(_renderer.cmd_buf)
    sdl.UploadToGPUBuffer(copy_pass,
        sdl.GPUTransferBufferLocation{transfer_buffer = _renderer.sprite_draw_resources.instance_transfer, offset = 0},
        sdl.GPUBufferRegion{buffer = _renderer.sprite_draw_resources.instance_buffer, offset = 0, size = bytes_needed},
        true)
    sdl.EndGPUCopyPass(copy_pass)
}

// Uploads vertex shader uniform data and draws all the instanced sprites.
DrawSpriteInstances :: proc(_renderer : ^Renderer, _instance_count : u32) {
    if _renderer.render_pass == nil || _renderer.sprite_pipeline == nil || _instance_count == 0 do return

    // Push the camera view_proj matrix for the upcoming draw
    global_vs := Sprite_Global_VS_Uniform{view_proj = CameraViewProjMatrix(&_renderer.camera)}
    sdl.PushGPUVertexUniformData(_renderer.cmd_buf, 0, &global_vs, u32(size_of(Sprite_Global_VS_Uniform)))

     // Bind the graphics pipeline state to the current render pass
    sdl.BindGPUGraphicsPipeline(_renderer.render_pass, _renderer.sprite_pipeline)

    // Bind the geometry vertex buffer and per-sprite instance data buffer
    vb_bindings := [2]sdl.GPUBufferBinding{
        {buffer = _renderer.sprite_draw_resources.quad_vb, offset = 0},
        {buffer = _renderer.sprite_draw_resources.instance_buffer, offset = 0},
    }
    sdl.BindGPUVertexBuffers(_renderer.render_pass, 0, &vb_bindings[0], 2)
    // Bind the 16-bit element index buffer for quad layout
    sdl.BindGPUIndexBuffer(
        _renderer.render_pass,
        sdl.GPUBufferBinding{buffer = _renderer.sprite_draw_resources.quad_ib, offset = 0},
        ._16BIT,
    )

    // Submit the instanced draw command to the GPU: 6 indices per quad, N instances
    sdl.DrawGPUIndexedPrimitives(_renderer.render_pass, 6, _instance_count, 0, 0, 0)
}