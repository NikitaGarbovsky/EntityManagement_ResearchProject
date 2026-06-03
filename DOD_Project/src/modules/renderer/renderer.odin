package renderer

import sdl "vendor:sdl3"
import "core:log"
import math "core:math/linalg"
import glm "core:math/linalg/glsl"


BeginFrame :: proc(_renderer : ^Renderer, viewport_size : math.Vector2f32) -> bool {
    _renderer.camera.viewport_size = viewport_size

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

BeginWorldPass :: proc(_renderer : ^Renderer) -> bool {
    if _renderer.cmd_buf == nil || _renderer.swapchain_tex == nil do return false

    color_target := sdl.GPUColorTargetInfo{
        texture     = _renderer.swapchain_tex,
        load_op     = .CLEAR,
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

UploadSpriteInstances :: proc(_renderer : ^Renderer, _instances : []Sprite_Instance) {
    if len(_instances) == 0 do return

    bytes_needed := u32(len(_instances)) * size_of(Sprite_Instance)
    assert(bytes_needed <= _renderer.sprite_draw_resources.max_instances * size_of(Sprite_Instance))

    mapped_raw := sdl.MapGPUTransferBuffer(_renderer.gpu, _renderer.sprite_draw_resources.instance_transfer, true)
    if mapped_raw == nil {
        log.errorf("MapGPUTransferBuffer failed: {}", sdl.GetError())
        return
    }

    copy(([^]Sprite_Instance)(mapped_raw)[:len(_instances)], _instances)
    sdl.UnmapGPUTransferBuffer(_renderer.gpu, _renderer.sprite_draw_resources.instance_transfer)

    copy_pass := sdl.BeginGPUCopyPass(_renderer.cmd_buf)
    sdl.UploadToGPUBuffer(copy_pass,
        sdl.GPUTransferBufferLocation{transfer_buffer = _renderer.sprite_draw_resources.instance_transfer, offset = 0},
        sdl.GPUBufferRegion{buffer = _renderer.sprite_draw_resources.instance_buffer, offset = 0, size = bytes_needed},
        true)
    sdl.EndGPUCopyPass(copy_pass)
}

DrawSpriteInstances :: proc(_renderer : ^Renderer, instance_count : u32) {
    if _renderer.render_pass == nil || _renderer.sprite_pipeline == nil || instance_count == 0 do return

    global_vs := Sprite_Global_VS_Uniform{view_proj = CameraViewProjMatrix(&_renderer.camera)}
    sdl.PushGPUVertexUniformData(_renderer.cmd_buf, 0, &global_vs, u32(size_of(Sprite_Global_VS_Uniform)))

    sdl.BindGPUGraphicsPipeline(_renderer.render_pass, _renderer.sprite_pipeline)

    vb_bindings := [2]sdl.GPUBufferBinding{
        {buffer = _renderer.sprite_draw_resources.quad_vb, offset = 0},
        {buffer = _renderer.sprite_draw_resources.instance_buffer, offset = 0},
    }
    sdl.BindGPUVertexBuffers(_renderer.render_pass, 0, &vb_bindings[0], 2)
    sdl.BindGPUIndexBuffer(
        _renderer.render_pass,
        sdl.GPUBufferBinding{buffer = _renderer.sprite_draw_resources.quad_ib, offset = 0},
        ._16BIT,
    )

    sdl.DrawGPUIndexedPrimitives(_renderer.render_pass, 6, instance_count, 0, 0, 0)
}