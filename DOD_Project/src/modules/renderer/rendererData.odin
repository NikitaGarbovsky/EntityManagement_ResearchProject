package renderer

import math "core:math/linalg"
import sdl "vendor:sdl3"
import glm "core:math/linalg/glsl"

Camera2D :: struct {
    position : glm.vec2, // World-space center of the camera
    zoom : f32, // 1 = default, 2 = zoom in, 0.5 = zoom out
    viewport_size : glm.vec2, // ixels
}

Rect2D :: struct {
    min : glm.vec2,
    max : glm.vec2,
}

Renderer :: struct {
    gpu : ^sdl.GPUDevice,
    window : ^sdl.Window,

    cmd_buf : ^sdl.GPUCommandBuffer,
    swapchain_tex : ^sdl.GPUTexture,
    render_pass : ^sdl.GPURenderPass,

    sprite_pipeline : ^sdl.GPUGraphicsPipeline, 

    clear_color : [4]f32,
    camera : Camera2D,
    sprite_draw_resources : Sprite_DrawResources,
}

// Holds the shared uniform data for the batched sprites
Sprite_Global_VS_Uniform :: struct {
    view_proj : glm.mat4 
}

Quad_Vertex :: struct {
    local_pos : [2]f32,
}

// Data for the rendered Sprite
Sprite_Instance :: struct {
    position : [2]f32,
    size : [2]f32,
    color : [4]f32,
}

// Holds resources data for sprites send to the renderer.
Sprite_DrawResources :: struct {
    quad_vb : ^sdl.GPUBuffer,
    quad_ib : ^sdl.GPUBuffer,

    instance_buffer : ^sdl.GPUBuffer,
    instance_transfer : ^sdl.GPUTransferBuffer,

    max_instances : u32,
}
