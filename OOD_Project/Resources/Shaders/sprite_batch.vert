#version 460

layout(location = 0) in vec2 inlocalpos;
layout(location = 1) in vec2 inPosition;
layout(location = 2) in vec2 inSize;
layout(location = 3) in vec4 inColor;

layout(set = 1, binding = 0, std140) uniform SpriteGlobalVS {
    mat4 view_proj;
} ubo;

layout(location = 0) out vec4 v_color;

void main() {

    vec2 worldPos = inlocalpos * inSize + inPosition;

    gl_Position = ubo.view_proj * vec4(worldPos, 0, 1.0);

    v_color = inColor;
}