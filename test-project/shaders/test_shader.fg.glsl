#version 450

layout(location = 0) in vec4 in_color;
layout(location = 1) in vec2 in_uv;

layout(location = 0) out vec4 out_color;

layout(set = 1, binding = 1) uniform sampler2D tex;

void main()
{
    out_color = in_color * texture(tex, in_uv);
}